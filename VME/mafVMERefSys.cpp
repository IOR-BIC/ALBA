/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMERefSys.cpp,v $
  Language:  C++
  Date:      $Date: 2006-07-13 09:08:55 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone, Paolo Quadrani
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


#include "mafVMERefSys.h"
#include "mafTransform.h"
#include "mafVMEOutputSurface.h"
#include "mafDataPipeCustom.h"
#include "mafTagArray.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include "mmaMaterial.h"
#include "mmgGui.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFDataPipe.h"
#include "vtkPoints.h"
#include "vtkGlyph3D.h"
#include "vtkPolyData.h"
#include "vtkArrowSource.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkAppendPolyData.h"
#include "vtkPointData.h"
#include "vtkLineSource.h"
#include "vtkUnsignedCharArray.h"
#include "vtkAxes.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMERefSys)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMERefSys::mafVMERefSys()
//-------------------------------------------------------------------------
{
  mafNEW(m_Transform);
  mafVMEOutputSurface *output=mafVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  // attach a datapipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  SetDataPipe(dpipe);

  m_ScaleFactor = 1.0;

  vtkUnsignedCharArray *data;
  float scalar_red[3]   = {255,0,0};
  float scalar_green[3] = {0,255,0};
  float scalar_blu[3]   = {0,0,255};

  m_XArrow = vtkArrowSource::New();
  m_XArrow->SetShaftRadius(m_XArrow->GetTipRadius()/5);
  m_XArrow->SetTipResolution(40);
  m_XArrow->SetTipRadius(m_XArrow->GetTipRadius()/2);
  m_XArrow->Update();

  m_XAxisTransform = vtkTransform::New();
  m_XAxisTransform->PostMultiply();
  m_XAxisTransform->Update();

  m_XAxis = vtkTransformPolyDataFilter::New();
  m_XAxis->SetInput(m_XArrow->GetOutput());
  m_XAxis->SetTransform(m_XAxisTransform);
  m_XAxis->Update();	

  int points = m_XArrow->GetOutput()->GetNumberOfPoints();  

  m_YArrow = vtkArrowSource::New();
  m_YArrow->SetShaftRadius(m_YArrow->GetTipRadius() / 5);
  m_YArrow->SetTipResolution(40);
  m_YArrow->SetTipRadius(m_YArrow->GetTipRadius() / 2);

  m_YAxisTransform = vtkTransform::New();
  m_YAxisTransform->PostMultiply();
  m_YAxisTransform->RotateZ(90);
  m_YAxisTransform->Update();

  m_YAxis = vtkTransformPolyDataFilter::New();
  m_YAxis->SetInput(m_YArrow->GetOutput());
  m_YAxis->SetTransform(m_YAxisTransform);
  m_YAxis->Update();

  m_ZArrow = vtkArrowSource::New();
  m_ZArrow->SetShaftRadius(m_ZArrow->GetTipRadius() / 5);
  m_ZArrow->SetTipResolution(40);
  m_ZArrow->SetTipRadius(m_ZArrow->GetTipRadius() / 2);
  m_ZArrow->Update();

  m_ZAxisTransform = vtkTransform::New();
  m_ZAxisTransform->PostMultiply();
  m_ZAxisTransform->RotateY(-90);
  m_ZAxisTransform->Update();

  m_ZAxis  = vtkTransformPolyDataFilter::New();
  m_ZAxis->SetInput(m_ZArrow->GetOutput());
  m_ZAxis->SetTransform(m_ZAxisTransform);
  m_ZAxis->Update();

  data = vtkUnsignedCharArray::New();
  data->SetName("AXES");
  data->SetNumberOfComponents(3);
  data->SetNumberOfTuples(points * 3);
  int i;
  for (i = 0; i < points; i++)
    data->SetTuple(i, scalar_red);
  for (i = points; i < 2*points; i++)
    data->SetTuple(i, scalar_green);
  for (i = 2*points; i < 3*points; i++)
    data->SetTuple(i, scalar_blu);

  // this filter do not copy the scalars also if all input 
  m_Axes = vtkAppendPolyData::New();    
  m_Axes->AddInput(m_XAxis->GetOutput()); // data has the scalars.
  m_Axes->AddInput(m_YAxis->GetOutput());
  m_Axes->AddInput(m_ZAxis->GetOutput());
  m_Axes->Update();

  m_ScaleAxisTransform = vtkTransform::New();
  m_ScaleAxisTransform->Scale(m_ScaleFactor,m_ScaleFactor,m_ScaleFactor);
  m_ScaleAxisTransform->Update();

  vtkMAFSmartPointer<vtkPolyData> axes_surface;
  axes_surface = m_Axes->GetOutput();
  axes_surface->SetSource(NULL);
  axes_surface->GetPointData()->SetScalars(data);
  vtkDEL(data);

  m_ScaleAxis  = vtkTransformPolyDataFilter::New();
  m_ScaleAxis->SetInput(axes_surface.GetPointer());
  m_ScaleAxis->SetTransform(m_ScaleAxisTransform);
  m_ScaleAxis->Update();

  dpipe->SetInput(m_ScaleAxis->GetOutput());
}

//-------------------------------------------------------------------------
mafVMERefSys::~mafVMERefSys()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);
  SetOutput(NULL);

  vtkDEL(m_XArrow);
  vtkDEL(m_XAxisTransform);
  m_XAxis->SetTransform(NULL);
  vtkDEL(m_XAxis);

  vtkDEL(m_YArrow);
  vtkDEL(m_YAxisTransform);
  m_YAxis->SetTransform(NULL);
  vtkDEL(m_YAxis);

  vtkDEL(m_ZArrow);
  vtkDEL(m_ZAxisTransform);
  m_ZAxis->SetTransform(NULL);
  vtkDEL(m_ZAxis);

  vtkDEL(m_Axes);	
  vtkDEL(m_ScaleAxisTransform);	
  m_ScaleAxis->SetTransform(NULL);
  vtkDEL(m_ScaleAxis);
}
//-------------------------------------------------------------------------
int mafVMERefSys::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMERefSys *vme_ref_sys=mafVMERefSys::SafeDownCast(a);
    m_Transform->SetMatrix(vme_ref_sys->m_Transform->GetMatrix());
    vme_ref_sys->SetScaleFactor(m_ScaleFactor);
    return MAF_OK;
  }  
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool mafVMERefSys::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    return (m_Transform->GetMatrix() == ((mafVMERefSys *)vme)->m_Transform->GetMatrix() &&
            m_ScaleFactor == ((mafVMERefSys *)vme)->GetScaleFactor());
  }
  return false;
}
//-------------------------------------------------------------------------
int mafVMERefSys::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==MAF_OK)
  {
    // force material allocation
    GetMaterial();
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
mafVMEOutputSurface *mafVMERefSys::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
  return (mafVMEOutputSurface *)GetOutput();
}

//-------------------------------------------------------------------------
void mafVMERefSys::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
bool mafVMERefSys::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
void mafVMERefSys::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear();
}

//-------------------------------------------------------------------------
void mafVMERefSys::SetScaleFactor(double scale)
//-------------------------------------------------------------------------
{
  m_ScaleFactor = scale;
  if (m_Gui)
  {
    m_Gui->Update();
  }
  m_ScaleAxisTransform->Identity();
  m_ScaleAxisTransform->Scale(m_ScaleFactor,m_ScaleFactor,m_ScaleFactor);
  m_ScaleAxisTransform->Update();
  m_ScaleAxis->Update();
  mafEvent cam_event(this,CAMERA_UPDATE);
  this->ForwardUpEvent(cam_event);
  Modified();
}

//-------------------------------------------------------------------------
double mafVMERefSys::GetScaleFactor()
//-------------------------------------------------------------------------
{
  return m_ScaleFactor;
}
//-----------------------------------------------------------------------
int mafVMERefSys::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    parent->StoreDouble("m_ScaleFactor", m_ScaleFactor);
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMERefSys::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);
      node->RestoreDouble("m_ScaleFactor", m_ScaleFactor);
      return MAF_OK;
    }
  }
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
mmaMaterial *mafVMERefSys::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
    if (m_Output)
    {
      ((mafVMEOutputSurface *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}
//-------------------------------------------------------------------------
mmgGui* mafVMERefSys::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  m_Gui->Double(ID_SCALE_FACTOR,"scale",&m_ScaleFactor);

  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMERefSys::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_SCALE_FACTOR:
      {
        SetScaleFactor(m_ScaleFactor);
      }
      break;
      default:
      mafNode::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}
//-----------------------------------------------------------------------
void mafVMERefSys::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  mafMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
char **mafVMERefSys::GetIcon()
//-------------------------------------------------------------------------
{
#include "mafVMESurface.xpm"
  return mafVMESurface_xpm;
}
