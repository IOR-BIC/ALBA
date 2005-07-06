/*=====================	====================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoBoundingBox.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-06 13:50:23 $
  Version:   $Revision: 1.1 $
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
mafGizmoBoundingBox::mafGizmoBoundingBox(mafVME *input, mafObserver *listener)
//----------------------------------------------------------------------------
{

  m_Listener = listener;
  InputVme = input;
  BoxGizmo = NULL;
  BoxOutline = NULL;

  BoxOutline = vtkOutlineSource::New();
  BoxOutline->SetBounds(InputVme->GetOutput()->GetVTKData()->GetBounds());

  // create the gizmo
  BoxGizmo = mafVMEGizmo::New();  
  BoxGizmo->SetName("BoxGizmo");
  
  // since i'm working in local mode i reparent to input vme the gizmo
  BoxGizmo->ReparentTo(InputVme);
  BoxGizmo->SetData(BoxOutline->GetOutput());
  
  // box gizmo data
//  BoxGizmoData = new mafVmeData(BoxGizmo, this);
//  BoxGizmo->SetClientData(BoxGizmoData);

  // set cone gizmo material property and initial color to red
  this->SetColor(1, 0, 0);

  // hide gizmo at creation
  this->Show(false);

  // add the gizmo to the tree
  mafEventMacro(mafEvent(this, VME_ADD, BoxGizmo));
}
//----------------------------------------------------------------------------
mafGizmoBoundingBox::~mafGizmoBoundingBox() 
//----------------------------------------------------------------------------
{
  BoxGizmo->SetBehavior(NULL);
  	
  //----------------------
	// No leaks so somebody is performing this...
	// wxDEL(BoxGizmoData[i]);
	//----------------------
  mafEventMacro(mafEvent(this, VME_REMOVE, BoxGizmo));  
  BoxGizmo->Delete();
  BoxOutline->Delete();
}

//----------------------------------------------------------------------------
void mafGizmoBoundingBox::Highlight(bool highlight)
//----------------------------------------------------------------------------
{
  if (highlight == true)
  {
     // Highlight the box by setting its color to yellow 
     this->SetColor(1, 1, 0);
  } 
  else
  {   
     // set box col to red
     this->SetColor(1, 0, 0);
  }
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
  BoxGizmo->GetMaterial()->m_Prop->SetColor(col);
  BoxGizmo->GetMaterial()->m_Prop->SetAmbient(0);
  BoxGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
  BoxGizmo->GetMaterial()->m_Prop->SetSpecular(0);
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
  float opacity = show ? 1 : 0;
  BoxGizmo->GetMaterial()->m_Prop->SetOpacity(opacity);
}

//----------------------------------------------------------------------------
void mafGizmoBoundingBox::SetAbsPose(mafMatrix *absPose)
//----------------------------------------------------------------------------
{
  BoxGizmo->SetAbsMatrix(*absPose);
}

//----------------------------------------------------------------------------
mafMatrix *mafGizmoBoundingBox::GetAbsPose()
//----------------------------------------------------------------------------
{
  return BoxGizmo->GetOutput()->GetAbsMatrix();
}

//----------------------------------------------------------------------------
void mafGizmoBoundingBox::SetInput(mafVME *vme)
//----------------------------------------------------------------------------
{
  this->InputVme = vme;
  double b[6];
  InputVme->GetOutput()->GetBounds(b);
  BoxOutline->SetBounds(b);
}
          
//----------------------------------------------------------------------------
double *mafGizmoBoundingBox::GetBounds()
//----------------------------------------------------------------------------
{
  BoxGizmo->GetOutput()->GetVTKData()->Update();
  return BoxGizmo->GetOutput()->GetVTKData()->GetBounds();
}

//----------------------------------------------------------------------------
void mafGizmoBoundingBox::GetBounds(double bounds[6])
//----------------------------------------------------------------------------
{
  BoxGizmo->GetOutput()->GetVTKData()->GetBounds(bounds);
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
  BoxOutline->SetBounds(bounds);
  BoxGizmo->GetOutput()->GetVTKData()->Update();
}
