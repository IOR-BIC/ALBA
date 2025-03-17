/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoBoundingBox
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaGizmoBoundingBox.h"
#include "albaDecl.h"

// isa stuff
#include "albaInteractorCompositorMouse.h"
#include "albaInteractorGenericMouse.h"
#include "mmaMaterial.h"

// vme stuff
#include "albaVME.h"
#include "albaVMEGizmo.h"
#include "albaVMEOutput.h"

// vtk stuff
#include "vtkProperty.h"
#include "vtkOutlineSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMath.h"

//----------------------------------------------------------------------------
albaGizmoBoundingBox::albaGizmoBoundingBox(albaVME *input, albaObserver *listener,albaVME *parent)
//----------------------------------------------------------------------------
{

  m_Listener = listener;
  m_InputVme = input;
  m_BoxGizmo = NULL;
  m_BoxOutline = NULL;

  m_BoxOutline = vtkOutlineSource::New();
  m_BoxOutline->SetBounds(m_InputVme->GetOutput()->GetVTKData()->GetBounds());
	m_BoxOutline->Update();

  // create the gizmo
  m_BoxGizmo = albaVMEGizmo::New();  
  m_BoxGizmo->SetName("BoxGizmo");
  
  // since i'm working in local mode i reparent to input vme the gizmo
  m_BoxGizmo->SetDataConnection(m_BoxOutline->GetOutputPort());
	m_BoxGizmo->GetOutput()->GetVTKData()->ComputeBounds();
	if(parent)
		m_BoxGizmo->ReparentTo(parent);
	else
		m_BoxGizmo->ReparentTo(m_InputVme);
  
  // set cone gizmo material property and initial color to red
  this->SetColor(1, 0, 0);

  // hide vme
  this->Show(false);

  // ask the manager to create the pipeline
  GetLogicManager()->VmeShow(m_BoxGizmo, true);
}
//----------------------------------------------------------------------------
albaGizmoBoundingBox::~albaGizmoBoundingBox() 
//----------------------------------------------------------------------------
{
  m_BoxGizmo->SetBehavior(NULL);
  	
  GetLogicManager()->VmeRemove(m_BoxGizmo);
  m_BoxOutline->Delete();
}

//----------------------------------------------------------------------------
void albaGizmoBoundingBox::Highlight(bool highlight)
//----------------------------------------------------------------------------
{
  double hl = highlight ? 1 : 0;
  this->SetColor(1, hl, 0);
}

//----------------------------------------------------------------------------
void albaGizmoBoundingBox::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  // forward events to the listener
  albaEventMacro(*alba_event);
}

//----------------------------------------------------------------------------
void albaGizmoBoundingBox::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_BoxGizmo->GetMaterial()->m_Prop->SetColor(col);
  m_BoxGizmo->GetMaterial()->m_Prop->SetAmbient(0);
  m_BoxGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
  m_BoxGizmo->GetMaterial()->m_Prop->SetSpecular(0);
}
//----------------------------------------------------------------------------
void albaGizmoBoundingBox::SetColor(double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(col);
}

//----------------------------------------------------------------------------
void albaGizmoBoundingBox::Show(bool show)
//----------------------------------------------------------------------------
{
  // use VTK opacity instead of vme show to speed up the render
  float opacity = show ? 1 : 0;
  m_BoxGizmo->GetMaterial()->m_Prop->SetOpacity(opacity);
}

//----------------------------------------------------------------------------
void albaGizmoBoundingBox::SetAbsPose(albaMatrix *absPose)
//----------------------------------------------------------------------------
{
  m_BoxGizmo->SetAbsMatrix(*absPose);
}

//----------------------------------------------------------------------------
void albaGizmoBoundingBox::SetPose(albaMatrix *pose)
//----------------------------------------------------------------------------
{
  m_BoxGizmo->SetAbsMatrix(*pose);
}

//----------------------------------------------------------------------------
albaMatrix *albaGizmoBoundingBox::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_BoxGizmo->GetOutput()->GetAbsMatrix();
}

//----------------------------------------------------------------------------
void albaGizmoBoundingBox::SetInput(albaVME *vme)
//----------------------------------------------------------------------------
{
  this->m_InputVme = vme;
  double b[6];
  m_InputVme->GetOutput()->GetBounds(b);
  m_BoxOutline->SetBounds(b);
}
          
//----------------------------------------------------------------------------
double *albaGizmoBoundingBox::GetBounds()
//----------------------------------------------------------------------------
{
  return m_BoxGizmo->GetOutput()->GetVTKData()->GetBounds();
}

//----------------------------------------------------------------------------
void albaGizmoBoundingBox::GetBounds(double bounds[6])
//----------------------------------------------------------------------------
{
  m_BoxGizmo->GetOutput()->GetVTKData()->GetBounds(bounds);
}

//----------------------------------------------------------------------------
void albaGizmoBoundingBox::SetBounds(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)  
//----------------------------------------------------------------------------
{
  double bounds[6] = {xmin, xmax, ymin, ymax, zmin, zmax};
  SetBounds(bounds);
}

//----------------------------------------------------------------------------
void albaGizmoBoundingBox::SetBounds(double bounds[6])  
//----------------------------------------------------------------------------
{
  m_BoxOutline->SetBounds(bounds);
	m_BoxOutline->Update();
}
//----------------------------------------------------------------------------
albaMatrix * albaGizmoBoundingBox::GetPose()
//----------------------------------------------------------------------------
{
  return m_BoxGizmo->GetOutput()->GetMatrix();
}
