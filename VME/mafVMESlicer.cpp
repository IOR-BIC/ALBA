/*=========================================================================

 Program: MAF2
 Module: mafVMESlicer
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMESlicer.h"
#include "mafGUI.h"
#include "mafGUILutPreset.h"

#include "mmaMaterial.h"

#include "mafTransform.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include "mafDataPipeCustom.h"
#include "mafVMEVolume.h"
#include "mafVMEOutputSurface.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFDataPipe.h"
#include "vtkMath.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"
#include "vtkMAFVolumeSlicer.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkLinearTransform.h"
#include "vtkPointData.h"
#include "vtkTransform.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMESlicer)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMESlicer::mafVMESlicer()
//-------------------------------------------------------------------------
{
  m_UpdateVTKPropertiesFromMaterial = true;

  mafNEW(m_Transform);
	mafNEW(m_CopyTransform);
  mafVMEOutputSurface *output=mafVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  m_TextureRes = 512;
  m_Xspc = m_Yspc = 0.3;

  vtkNEW(m_PSlicer);
  vtkNEW(m_ISlicer);
	m_PSlicer->SetOutputTypeToPolyData();

	m_ISlicer->SetOutputDimentions(m_TextureRes,m_TextureRes,1);
	m_ISlicer->SetOutputSpacing(m_Xspc, m_Yspc, 1.0f);

  m_PSlicer->SetTextureConnection(m_ISlicer->GetOutputPort());
  
  vtkNEW(m_BackTransform);
  m_BackTransform->SetInputConnection(m_PSlicer->GetOutputPort());

  DependsOnLinkedNodeOn();

  // attach a datapipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);

  m_TrilinearInterpolationOn = true;
}

//-------------------------------------------------------------------------
mafVMESlicer::~mafVMESlicer()
//-------------------------------------------------------------------------
{
  //vtkDEL(m_BackTransformParent);
  vtkDEL(m_CopyTransform);
  vtkDEL(m_BackTransform);
  mafDEL(m_Transform);
  SetOutput(NULL);

  vtkDEL(m_PSlicer);
  vtkDEL(m_ISlicer);
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMESlicer::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
    lutPreset(4,GetMaterial()->m_ColorLut);
  }
  return material;
}

//-------------------------------------------------------------------------
int mafVMESlicer::DeepCopy(mafVME *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMESlicer *slicer = mafVMESlicer::SafeDownCast(a);
    mafVME *linked_node = slicer->GetSlicedVMELink();
    if (linked_node)
    {
      this->SetLink("SlicedVME", linked_node);
    }

    m_Transform->SetMatrix(slicer->m_Transform->GetMatrix());
    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetDependOnAbsPose(true);
      dpipe->SetInput(m_BackTransform->GetOutput());
      dpipe->SetNthInput(1,m_PSlicer->GetTexture());
    }
    m_SlicedName      = slicer->m_SlicedName;
    GetMaterial()->SetMaterialTexture(m_PSlicer->GetTexture());
    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMESlicer::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    return ret = (m_Transform->GetMatrix()==((mafVMESlicer *)vme)->m_Transform->GetMatrix() &&
                  m_SlicedName      == ((mafVMESlicer *)vme)->m_SlicedName &&
                  GetLink("SlicedVME") == ((mafVMESlicer *)vme)->GetLink("SlicedVME"));
  }
  return ret;
}

//-------------------------------------------------------------------------
mafGUI* mafVMESlicer::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  mafVME *vol = GetSlicedVMELink();
  m_SlicedName = vol ? vol->GetName() : _("none");
  m_Gui->Button(ID_VOLUME_LINK,&m_SlicedName,_("Volume"), _("Select the volume to be sliced"));

  m_Gui->Divider();
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMESlicer::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_VOLUME_LINK:
      {
        mafString title = _("Choose volume vme");
        e->SetId(VME_CHOOSE);
        e->SetArg((long)&mafVMESlicer::VolumeAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        mafVME *n = e->GetVme();
        if (n != NULL)
        {
          SetSlicedVMELink(n);
          m_SlicedName = n->GetName();
          m_Gui->Update();
          e->SetId(CAMERA_UPDATE);
          e->SetArg(0);
          ForwardUpEvent(e);
        }
      }
      break;
      default:
        mafVME::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}
//-------------------------------------------------------------------------
mafVMEOutputSurface *mafVMESlicer::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
  return (mafVMEOutputSurface *)GetOutput();
}
//-------------------------------------------------------------------------
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
bool mafVMESlicer::IsDataAvailable()
//-------------------------------------------------------------------------
{
	if(GetSlicedVMELink())
    return GetSlicedVMELink()->IsDataAvailable();
	else
		return false;
}

//-------------------------------------------------------------------------
void mafVMESlicer::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}
//-----------------------------------------------------------------------
mafVME *mafVMESlicer::GetSlicedVMELink()
//-----------------------------------------------------------------------
{
  return GetLink("SlicedVME");
}
//-----------------------------------------------------------------------
void mafVMESlicer::SetSlicedVMELink(mafVME *node)
//-----------------------------------------------------------------------
{
  SetLink("SlicedVME", node);
  Modified();
}
//-----------------------------------------------------------------------
void mafVMESlicer::InternalPreUpdate()
//-----------------------------------------------------------------------
{
  mafVME *vol = GetSlicedVMELink();
  if(vol)
  {
    vtkDataSet *vtkdata = vol->GetOutput()->GetVTKData();
    if (vtkdata)
    {
      double pos[3];
      float vectX[3],vectY[3], n[3];

			//transform
			m_CopyTransform->SetMatrix(m_Transform->GetMatrix());
			m_CopyTransform->Update();

			mafSmartPointer<mafTransform> slicedVMETransform;
			slicedVMETransform->SetMatrix(vol->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
			slicedVMETransform->Update();
			slicedVMETransform->Invert();
			slicedVMETransform->Update();

			mafSmartPointer<mafTransform> parentTransform;
			parentTransform->SetMatrix(GetParent()->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
			parentTransform->Update();

			parentTransform->Concatenate(slicedVMETransform,0);
			parentTransform->Update();

			m_CopyTransform->Concatenate(parentTransform,0);
			m_CopyTransform->Update();
      m_CopyTransform->GetPosition(pos);
      m_CopyTransform->GetVersor(0, vectX);
      m_CopyTransform->GetVersor(1, vectY);

      vtkMath::Normalize(vectX);
      vtkMath::Normalize(vectY);
      vtkMath::Cross(vectY, vectX, n);
      vtkMath::Normalize(n);
      vtkMath::Cross(n, vectX, vectY);
      vtkMath::Normalize(vectY);

      vtkDataArray *scalars = vtkdata->GetPointData()->GetScalars();
      if (scalars == NULL)
      {
        return;
      }

			m_PSlicer->Update();

      
	  if (m_UpdateVTKPropertiesFromMaterial == true)
	  {
		  GetMaterial()->UpdateProp();
	  }
	  

      m_ISlicer->SetInputData(vtkdata);
      m_ISlicer->SetPlaneOrigin(pos);
      m_ISlicer->SetPlaneAxisX(vectX);
      m_ISlicer->SetPlaneAxisY(vectY);
			m_ISlicer->Update();

			m_PSlicer->SetInputData(vtkdata);
			m_PSlicer->SetPlaneOrigin(pos);
			m_PSlicer->SetPlaneAxisX(vectX);
			m_PSlicer->SetPlaneAxisY(vectY);
			m_PSlicer->Update();

      m_BackTransform->SetTransform(m_CopyTransform->GetVTKTransform()->GetInverse());
			m_BackTransform->Update();

			vtkImageData *texture = m_PSlicer->GetTexture();
			texture->AllocateScalars(scalars->GetDataType(),scalars->GetNumberOfComponents());
			texture->Modified();

			GetMaterial()->SetMaterialTexture(texture);
			texture->GetScalarRange(GetMaterial()->m_TableRange);
      /*m_BackTransformParent->SetTransform(transform->GetInverse());
      m_BackTransform->SetInput(m_BackTransformParent->GetOutput());
      m_BackTransform->Update();*/


			mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());

			dpipe->SetInput(m_BackTransform->GetOutput());
			dpipe->SetNthInput(1,m_ISlicer->GetOutput());
				
    }
  }

  m_SlicedName = vol ? vol->GetName() : _("none");
}

//-----------------------------------------------------------------------
void mafVMESlicer::InternalUpdate()
//-----------------------------------------------------------------------
{
  mafVME *vol = GetSlicedVMELink();
  if(vol)
  {
    vol->Update();
    if (vtkDataSet *vtkdata=vol->GetOutput()->GetVTKData())
    {
      m_PSlicer->Update();
      m_ISlicer->Update();
			m_BackTransform->Update();

			mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());

			dpipe->SetInput(m_BackTransform->GetOutput());
			dpipe->SetNthInput(1,m_ISlicer->GetOutput());


      vtkDataArray *scalars = vtkdata->GetPointData()->GetScalars();
      if (scalars == NULL)
      {
        return;
      }

      vtkImageData *texture = m_PSlicer->GetTexture();

			// set the texture in the output, must do it here, after setting slicer filter's input
			GetSurfaceOutput()->SetTexture(texture);
      GetMaterial()->SetMaterialTexture(texture);
			GetMaterial()->m_MaterialType = mmaMaterial::USE_TEXTURE;
			GetMaterial()->m_TextureMappingMode = mmaMaterial::PLANE_MAPPING;

      //GetMaterial()->SetMaterialTexture(texture);
      texture->GetScalarRange(GetMaterial()->m_TableRange);

	  if (m_UpdateVTKPropertiesFromMaterial == true)
	  {
		GetMaterial()->UpdateProp();
	  }
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

  mafMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
char** mafVMESlicer::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMESurface.xpm"
  return mafVMESurface_xpm;
}
//-------------------------------------------------------------------------
void mafVMESlicer::SetTrilinearInterpolation(bool on) 
//-------------------------------------------------------------------------
{
  m_TrilinearInterpolationOn = on;
  if(m_ISlicer)
  {
    m_ISlicer->SetTrilinearInterpolation(on == true);
  }
}