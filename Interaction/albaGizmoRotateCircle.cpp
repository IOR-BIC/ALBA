/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoRotateCircle
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

#include "albaGizmoRotateCircle.h"
#include "albaRefSys.h"

// wxwin stuff
#include "wx/string.h"
#include "albaDecl.h"

// isa stuff
#include "albaInteractorCompositorMouse.h"
#include "albaInteractorGenericMouse.h"

// vme stuff
#include "mmaMaterial.h"
#include "albaVME.h"
#include "albaVMEOutput.h"
#include "albaVMEGizmo.h"
#include "albaMatrix.h"

// vtk stuff
#include "vtkDiskSource.h"
#include "vtkCleanPolyData.h"
#include "vtkTubeFilter.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkProperty.h"

//----------------------------------------------------------------------------
albaGizmoRotateCircle::albaGizmoRotateCircle(albaVME *input, albaObserver *listener, albaString name)
//----------------------------------------------------------------------------
{
  this->SetIsActive(false);
  
  // default axis is X
  m_ActiveAxis = X;
  m_IsaComp = NULL;
  m_Radius = -1;

  m_Listener = listener;
  m_InputVme = input;

  //-----------------
  // create pipeline stuff
  //-----------------
  CreatePipeline();

  //-----------------
  // create isa stuff
  //-----------------
  CreateISA();

  //-----------------
  // create vme gizmo stuff
  //-----------------
  // build the circle gizmo
  m_GizmoCircle = albaVMEGizmo::New();
  m_GizmoCircle->SetName(name);
  m_GizmoCircle->SetDataConnection(m_RotatePDF->GetOutputPort());
  m_GizmoCircle->SetMediator(m_Listener);

  // assign isa to S1 and S2;
  m_GizmoCircle->SetBehavior(m_IsaComp);
  
  // set the axis to X axis
  this->SetAxis(m_ActiveAxis);

  // get the abs matrix from the input vme
  m_AbsInputMatrix = m_InputVme->GetOutput()->GetAbsMatrix();
  m_InputVme->GetOutput()->Update();
  
  // and set it as gizmo pose
  SetAbsPose(m_AbsInputMatrix);
  
  // and gizmo refsys
  SetRefSysMatrix(m_AbsInputMatrix);

  // add the gizmo to the tree, this should increase reference count  
  m_GizmoCircle->ReparentTo(m_InputVme->GetRoot());

}
//----------------------------------------------------------------------------
albaGizmoRotateCircle::~albaGizmoRotateCircle() 
//----------------------------------------------------------------------------
{
  m_GizmoCircle->SetBehavior(NULL);
  
  vtkDEL(m_Circle);
  vtkDEL(m_CleanCircle);
  vtkDEL(m_CircleTF);
  vtkDEL(m_RotationTr); 
  vtkDEL(m_RotatePDF); 
	//----------------------
	// No leaks so somebody is performing this...
	//----------------------
  vtkDEL(m_IsaComp); 
  
	m_GizmoCircle->ReparentTo(NULL);
}
//----------------------------------------------------------------------------
void albaGizmoRotateCircle::CreatePipeline() 
//----------------------------------------------------------------------------
{
  // calculate diagonal of InputVme space bounds 
  double b[6],p1[3],p2[3],d;
	if(m_InputVme->IsA("albaVMEGizmo"))
		m_InputVme->GetOutput()->GetVTKData()->GetBounds(b);
	else
		m_InputVme->GetOutput()->GetBounds(b);
  p1[0] = b[0];
  p1[1] = b[2];
  p1[2] = b[4];
	p2[0] = (b[1] != -1) ? b[1] : 100;
	p2[1] = (b[3] != -1) ? b[3] : 100;
	p2[2] = (b[5] != -1) ? b[5] : 100;
  d = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));
    
  // create circle
  m_Circle = vtkDiskSource::New();
	m_Circle->SetCircumferentialResolution(50);
	m_Circle->SetInnerRadius(d / 2);
	m_Circle->SetOuterRadius(d / 2);

  m_Radius = d / 2;

  // clean the circle polydata
  m_CleanCircle = vtkCleanPolyData::New();
	m_CleanCircle->SetInputConnection(m_Circle->GetOutputPort());

  // tube filter the circle 
  m_CircleTF = vtkTubeFilter::New();
  m_CircleTF->SetInputConnection(m_CleanCircle->GetOutputPort());
  m_CircleTF->SetRadius(d / 200);
  m_CircleTF->SetNumberOfSides(20);
  
  // create rotation transform and rotation TPDF
  m_RotationTr = vtkTransform::New();
  m_RotationTr->Identity();

  m_RotatePDF = vtkTransformPolyDataFilter::New();
  m_RotatePDF->SetTransform(m_RotationTr);
  m_RotatePDF->SetInputConnection(m_CircleTF->GetOutputPort());

}
//----------------------------------------------------------------------------
void albaGizmoRotateCircle::CreateISA()
//----------------------------------------------------------------------------
{
  // Create isa compositor and assign behaviors to IsaGen ivar.
  // Default isa constrain rotation around X axis.
  m_IsaComp = albaInteractorCompositorMouse::New();

  // default behavior is activated by mouse left and is constrained to X axis,
  // default ref sys is input vme abs matrix
  m_IsaGen = m_IsaComp->CreateBehavior(MOUSE_LEFT);
  m_IsaGen->SetVME(m_InputVme);
  m_IsaGen->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK); 
  m_IsaGen->GetRotationConstraint()->GetRefSys()->SetTypeToCustom(m_AbsInputMatrix);
  m_IsaGen->GetPivotRefSys()->SetTypeToCustom(m_AbsInputMatrix);
  m_IsaGen->EnableRotation(true);

  //isa will send events to this
  m_IsaGen->SetListener(this);
}
//----------------------------------------------------------------------------
void albaGizmoRotateCircle::SetAxis(int axis) 
//----------------------------------------------------------------------------
{
  // this should be called when the gizmo
  // is created; gizmos are not highlighted
  
  // register the axis
  m_ActiveAxis = axis;
  
  // rotate the gizmo components to match the specified axis
  if (m_ActiveAxis == X)
  {
    // set rotation to move gizmo normal to X
    m_RotationTr->Identity();
    m_RotationTr->RotateY(90);
    
    // set the color to red
    this->SetColor(1, 0, 0);

     // change the axis constrain  
    m_IsaGen->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK);
  }
  else if (m_ActiveAxis == Y)
  {
    // set rotation to move gizmo normal to Y 
    m_RotationTr->Identity();
    m_RotationTr->RotateX(90);
 
    // set the color to green
    this->SetColor(0, 1, 0);

    // change the Gizmo constrain
    m_IsaGen->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
  }
  else if (m_ActiveAxis == Z)
  {
    // reset circle orientation to move gizmo normal to Z
    m_RotationTr->Identity();
  
    // set the color to blue
    this->SetColor(0, 0, 1);
   
    // change the Gizmo constrain
    m_IsaGen->GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE);
  }    
}
//----------------------------------------------------------------------------
void albaGizmoRotateCircle::Highlight(bool highlight)
//----------------------------------------------------------------------------
{
  if (highlight == true)
  {
   // Highlight the circle by setting its color to yellow 
   this->SetColor(1, 1, 0);
  } 
  else
  {
   // restore original color 
   if (m_ActiveAxis == X)
   {
    // X circle to red
    this->SetColor(1, 0, 0);
   } 
   else if (m_ActiveAxis == Y)
   {
     // Y circle to green
    this->SetColor(0, 1, 0);
   }
   else if (m_ActiveAxis == Z)
   {     
    // Z circle to blue
    this->SetColor(0, 0, 1);
   } 
  }
}
//----------------------------------------------------------------------------
void  albaGizmoRotateCircle::SetRadius(double radius)
//----------------------------------------------------------------------------
{
  m_Radius = radius;
  m_Circle->SetInnerRadius(radius);
	m_Circle->SetOuterRadius(radius);
}
//----------------------------------------------------------------------------
void albaGizmoRotateCircle::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  // mouse down change gizmo selection status
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    if (e->GetId() == ID_TRANSFORM)
    {
      if (e->GetArg() == albaInteractorGenericMouse::MOUSE_DOWN)
      {
        this->SetIsActive(true);
      }
      // forward events to the listener
      e->SetSender(this);
      albaEventMacro(*e);
    }
    else
    {
      albaEventMacro(*e);
    }
  }
}
/** Gizmo color */
//----------------------------------------------------------------------------
void albaGizmoRotateCircle::SetColor(double col[3])
//----------------------------------------------------------------------------
{
    m_GizmoCircle->GetMaterial()->m_Prop->SetColor(col);
	m_GizmoCircle->GetMaterial()->m_Prop->SetAmbient(0);
	m_GizmoCircle->GetMaterial()->m_Prop->SetDiffuse(1);
	m_GizmoCircle->GetMaterial()->m_Prop->SetSpecular(0);
}
//----------------------------------------------------------------------------
void albaGizmoRotateCircle::SetColor(double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(col);
}
//----------------------------------------------------------------------------
void albaGizmoRotateCircle::Show(bool show)
//----------------------------------------------------------------------------
{
  GetLogicManager()->VmeShow(m_GizmoCircle, show);
}
//----------------------------------------------------------------------------
void albaGizmoRotateCircle::SetAbsPose(albaMatrix *absPose)
//----------------------------------------------------------------------------
{
  m_GizmoCircle->SetAbsMatrix(*absPose);
  SetRefSysMatrix(absPose);
}
//----------------------------------------------------------------------------
void albaGizmoRotateCircle::SetRefSysMatrix(albaMatrix *matrix)
//----------------------------------------------------------------------------
{  
  m_IsaGen->GetRotationConstraint()->GetRefSys()->SetTypeToCustom(matrix);
  m_IsaGen->GetPivotRefSys()->SetTypeToCustom(matrix);
}
//----------------------------------------------------------------------------
albaMatrix *albaGizmoRotateCircle::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_GizmoCircle->GetOutput()->GetAbsMatrix();
}
//----------------------------------------------------------------------------
void albaGizmoRotateCircle::SetInput(albaVME *vme)
//----------------------------------------------------------------------------
{
  // register the input vme
  this->m_InputVme = vme; 

  // set the pose and the refsys
  SetAbsPose(vme->GetOutput()->GetAbsMatrix()); 
}
//---------------------------------------------------------------------------
albaInteractorGenericInterface *albaGizmoRotateCircle::GetInteractor()
//----------------------------------------------------------------------------
{
  return m_IsaGen;
}

int albaGizmoRotateCircle::GetAxis()
{
 return m_ActiveAxis;
}

double albaGizmoRotateCircle::GetRadius()
{
  return m_Radius;
}

void albaGizmoRotateCircle::SetListener( albaObserver *Listener )
{
  m_Listener = Listener;
}

void albaGizmoRotateCircle::SetIsActive( bool highlight )
{
  // Yellow (highlighted) gizmo is the active one 
  m_IsActive = highlight;
}

bool albaGizmoRotateCircle::GetIsActive()
{
  return m_IsActive;
}

albaVME * albaGizmoRotateCircle::GetInput()
{
  return this->m_InputVme;
}

void albaGizmoRotateCircle::SetMediator(albaObserver *mediator)
{
	// superclass call
	albaGizmoInterface::SetMediator(mediator);	
	m_GizmoCircle->SetMediator(mediator);
}