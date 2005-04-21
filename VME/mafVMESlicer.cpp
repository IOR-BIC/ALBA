/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMESlicer.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-21 22:03:41 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMESlicer.h"
#include "mafVMEVolume.h"
#include "mafTransform.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include "mafDataPipeCustom.h"

#include "vtkMAFDataPipe.h"
#include "vtkMath.h"
#include "vtkMAFSmartPointer.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"
#include "vtkVolumeSlicer.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMESlicer)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMESlicer::mafVMESlicer()
//-------------------------------------------------------------------------
{
  mafNEW(m_Transform);
  mafVMEOutputSurface *output=mafVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  // attach a datapipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe=mafDataPipeCustom::New();
  SetDataPipe(dpipe);

  m_TextureRes = 128;
  m_Xspc = m_Yspc = 0.3;

  //vtkMAFSmartPointer<vtkImageData> image_data;
  //this->SetNthOutput(0,image_data);

  //vtkMAFSmartPointer<vtkPolyData> slice_data;
  //this->SetNthOutput(1,slice_data);

  vtkMAFSmartPointer<vtkImageData> image;
  image->SetScalarTypeToUnsignedChar();
  image->SetNumberOfScalarComponents(3);
  image->SetExtent(0, m_TextureRes - 1, 0, m_TextureRes - 1, 0, 0);
  image->SetUpdateExtent(0, m_TextureRes - 1, 0, m_TextureRes - 1, 0, 0);
  image->SetSpacing(m_Xspc, m_Yspc, 1.f);

  vtkMAFSmartPointer<vtkPolyData> slice;

  vtkNEW(m_PSlicer);
  vtkNEW(m_ISlicer);
  m_PSlicer->SetOutput(slice);
  m_PSlicer->SetTexture(image);
  m_ISlicer->SetOutput(image);
  
  dpipe->GetVTKDataPipe()->SetNthInput(0,slice);
  dpipe->GetVTKDataPipe()->SetNthInput(1,image);

  // set the texture in the output, must do it here, after setting slicer filter's input
  GetSurfaceOutput()->SetTexture((vtkImageData *)((mafDataPipeCustom *)GetDataPipe())->GetVTKDataPipe()->GetOutput(1));
}

//-------------------------------------------------------------------------
mafVMESlicer::~mafVMESlicer()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);
  SetOutput(NULL);

  vtkDEL(m_PSlicer);
  vtkDEL(m_ISlicer);
}

//-------------------------------------------------------------------------
int mafVMESlicer::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMESlicer *vme_slicer=mafVMESlicer::SafeDownCast(a);
    m_Transform->SetMatrix(vme_slicer->m_Transform->GetMatrix());
    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMESlicer::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    return m_Transform->GetMatrix()==((mafVMESlicer *)vme)->m_Transform->GetMatrix();
  }
  return false;
}


//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputSurface *mafVMESlicer::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
  return (mafVMEOutputSurface *)GetOutput();
}

void mafVMESlicer::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
bool mafVMESlicer::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
void mafVMESlicer::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}

//-----------------------------------------------------------------------
void mafVMESlicer::InternalPreUpdate()
//-----------------------------------------------------------------------
{
  mafVME *vol = mafVMEVolume::SafeDownCast(GetParent());
  if(vol)
  {
    if (vtkDataSet *vtkdata=vol->GetOutput()->GetVTKData())
    {
      double pos[3],vectX[3],vectY[3], n[3];

      mafTransform trans;
      trans.SetMatrix(*(GetOutput()->GetMatrix()));
      trans.GetPosition(pos);
      trans.GetVersor(0, vectX);
      trans.GetVersor(1, vectY);

      vtkMath::Normalize(vectX);
      vtkMath::Normalize(vectY);
      vtkMath::Cross(vectY, vectX, n);
      vtkMath::Normalize(n);
      vtkMath::Cross(n, vectX, vectY);
      vtkMath::Normalize(vectY);

      if (vtkdata->IsA("vtkRectilinearGrid"))
      {
        vtkdata->Update();
      }

      m_PSlicer->SetInput(vtkdata);
      m_PSlicer->SetPlaneOrigin(pos);
      m_PSlicer->SetPlaneAxisX(vectX);
      m_PSlicer->SetPlaneAxisY(vectY);

      m_ISlicer->SetInput(vtkdata);
      m_ISlicer->SetPlaneOrigin(pos);
      m_ISlicer->SetPlaneAxisX(vectX);
      m_ISlicer->SetPlaneAxisY(vectY); 
    }
  }
}

//-----------------------------------------------------------------------
void mafVMESlicer::InternalUpdate()
//-----------------------------------------------------------------------
{
  mafVME *vol = mafVMEVolume::SafeDownCast(GetParent());
  if(vol)
  {
    if (vtkDataSet *vtkdata=vol->GetOutput()->GetVTKData())
    {
      // set filter windowing, must do it here after updating 
      // the input. Note: this is executed before the Execute of
      // the VTK pipe, so before the vtkVolumeSlicer computes
      // its output data.
      double srange[2],w,l;
      vtkdata->GetScalarRange(srange);
      w = srange[1] - srange[0];
      l = (srange[1] + srange[0]) * 0.5;
      m_ISlicer->SetWindow(w);
      m_ISlicer->SetLevel(l);
    }
  }
}
//-----------------------------------------------------------------------
int mafVMESlicer::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMESlicer::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}

//-----------------------------------------------------------------------
void mafVMESlicer::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  m_Transform->GetMatrix().Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
//char** mafVMESlicer::GetIcon() 
//-------------------------------------------------------------------------
//{
//  #include "mafVMESlicer.xpm"
//  return mafVMESlicer_xpm;
//}
