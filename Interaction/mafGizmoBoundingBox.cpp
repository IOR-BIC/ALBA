/*=====================	====================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoBoundingBox.cpp,v $
  Language:  C++
  Date:      $Date: 2008-05-05 13:49:33 $
  Version:   $Revision: 1.8 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafGizmoBoundingBox.h"
#include "mafDecl.h"

// isa stuff
#include "mmiCompositorMouse.h"
#include "mmiGenericMouse.h"
#include "mmaMaterial.h"

// vme stuff
#include "mafVME.h"
#include "mafVMEGizmo.h"
#include "mafVMEOutput.h"

// vtk stuff
#include "vtkProperty.h"
#include "vtkOutlineSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMath.h"

//----------------------------------------------------------------------------
mafGizmoBoundingBox::mafGizmoBoundingBox(mafVME *input, mafObserver *listener,mafVME *parent)
//----------------------------------------------------------------------------
{

  m_Listener = listener;
  m_InputVme = input;
  m_BoxGizmo = NULL;
  m_BoxOutline = NULL;

  m_BoxOutline = vtkOutlineSource::New();
  m_BoxOutline->SetBounds(m_InputVme->GetOutput()->GetVTKData()->GetBounds());

  // create the gizmo
  m_BoxGizmo = mafVMEGizmo::New();  
  m_BoxGizmo->SetName("BoxGizmo");
  
  // since i'm working in local mode i reparent to input vme the gizmo
  m_BoxGizmo->SetData(m_BoxOutline->GetOutput());
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
  mafEventMacro(mafEvent(this,VME_SHOW,m_BoxGizmo,true));
}
//----------------------------------------------------------------------------
mafGizmoBoundingBox::~mafGizmoBoundingBox() 
//----------------------------------------------------------------------------
{
  m_BoxGizmo->SetBehavior(NULL);
  	
  mafEventMacro(mafEvent(this, VME_REMOVE, m_BoxGizmo));  
  m_BoxOutline->Delete();
}

//----------------------------------------------------------------------------
void mafGizmoBoundingBox::Highlight(bool highlight)
//----------------------------------------------------------------------------
{
  double hl = highlight ? 1 : 0;
  this->SetColor(1, hl, 0);
}

//----------------------------------------------------------------------------
void mafGizmoBoundingBox::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // forward events to the listener
  mafEventMacro(*maf_event);
}

//----------------------------------------------------------------------------
void mafGizmoBoundingBox::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_BoxGizmo->GetMaterial()->m_Prop->SetColor(col);
  m_BoxGizmo->GetMaterial()->m_Prop->SetAmbient(0);
  m_BoxGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
  m_BoxGizmo->GetMaterial()->m_Prop->SetSpecular(0);
}
//----------------------------------------------------------------------------
void mafGizmoBoundingBox::SetColor(double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(col);
}

//----------------------------------------------------------------------------
void mafGizmoBoundingBox::Show(bool show)
//----------------------------------------------------------------------------
{
  // use VTK opacity instead of VME_SHOW to speed up the render
  float opacity = show ? 1 : 0;
  m_BoxGizmo->GetMaterial()->m_Prop->SetOpacity(opacity);
}

//----------------------------------------------------------------------------
void mafGizmoBoundingBox::SetAbsPose(mafMatrix *absPose)
//----------------------------------------------------------------------------
{
  m_BoxGizmo->SetAbsMatrix(*absPose);
}

//----------------------------------------------------------------------------
void mafGizmoBoundingBox::SetPose(mafMatrix *pose)
//----------------------------------------------------------------------------
{
  m_BoxGizmo->SetAbsMatrix(*pose);
}

//----------------------------------------------------------------------------
mafMatrix *mafGizmoBoundingBox::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_BoxGizmo->GetOutput()->GetAbsMatrix();
}

//----------------------------------------------------------------------------
void mafGizmoBoundingBox::SetInput(mafVME *vme)
//----------------------------------------------------------------------------
{
  this->m_InputVme = vme;
  double b[6];
  m_InputVme->GetOutput()->GetBounds(b);
  m_BoxOutline->SetBounds(b);
}
          
//----------------------------------------------------------------------------
double *mafGizmoBoundingBox::GetBounds()
//----------------------------------------------------------------------------
{
  m_BoxGizmo->GetOutput()->GetVTKData()->Update();
  return m_BoxGizmo->GetOutput()->GetVTKData()->GetBounds();
}

//----------------------------------------------------------------------------
void mafGizmoBoundingBox::GetBounds(double bounds[6])
//----------------------------------------------------------------------------
{
  m_BoxGizmo->GetOutput()->GetVTKData()->GetBounds(bounds);
}

//----------------------------------------------------------------------------
void mafGizmoBoundingBox::SetBounds(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)  
//----------------------------------------------------------------------------
{
  double bounds[6] = {xmin, xmax, ymin, ymax, zmin, zmax};
  SetBounds(bounds);
}

//----------------------------------------------------------------------------
void mafGizmoBoundingBox::SetBounds(double bounds[6])  
//----------------------------------------------------------------------------
{
  m_BoxOutline->SetBounds(bounds);
  m_BoxGizmo->GetOutput()->GetVTKData()->Update();
}
