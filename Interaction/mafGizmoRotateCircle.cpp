/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoRotateCircle.cpp,v $
  Language:  C++
  Date:      $Date: 2008-11-04 18:03:33 $
  Version:   $Revision: 1.8.4.2 $
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


#include "mafGizmoRotateCircle.h"
// wxwin stuff
#include "wx/string.h"

#include "mafDecl.h"

// isa stuff
#include "mmiCompositorMouse.h"
#include "mmiGenericMouse.h"

// vme stuff
#include "mmaMaterial.h"
#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafVMEGizmo.h"
#include "mafMatrix.h"

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
mafGizmoRotateCircle::mafGizmoRotateCircle(mafVME *input, mafObserver *listener)
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
  CreatePipeline();

  // create isa stuff
  CreateISA();

  //-----------------
  // create vme gizmo stuff
  //-----------------
  
  // the circle gizmo
  m_Gizmo = mafVMEGizmo::New();
  m_Gizmo->SetName("circle");
  m_Gizmo->SetData(m_RotatePDF->GetOutput());

  // assign isa to S1 and S2;
  m_Gizmo->SetBehavior(m_IsaComp);
  
  // set the axis to X axis
  this->SetAxis(m_ActiveAxis);

  m_AbsInputMatrix = m_InputVme->GetOutput()->GetAbsMatrix();
  m_InputVme->GetOutput()->Update();
  SetAbsPose(m_AbsInputMatrix);
  SetRefSysMatrix(m_AbsInputMatrix);

  // add the gizmo to the tree, this should increase reference count  
  m_Gizmo->ReparentTo(mafVME::SafeDownCast(m_InputVme->GetRoot()));

}
//----------------------------------------------------------------------------
mafGizmoRotateCircle::~mafGizmoRotateCircle() 
//----------------------------------------------------------------------------
{
  m_Gizmo->SetBehavior(NULL);
  
  vtkDEL(m_Circle);
  vtkDEL(m_CleanCircle);
  vtkDEL(m_CircleTF);
  vtkDEL(m_RotationTr); 
  vtkDEL(m_RotatePDF); 
	//----------------------
	// No leaks so somebody is performing this...
	//----------------------
  vtkDEL(m_IsaComp); 
  
	m_Gizmo->ReparentTo(NULL);
}
//----------------------------------------------------------------------------
void mafGizmoRotateCircle::CreatePipeline() 
//----------------------------------------------------------------------------
{
  // calculate diagonal of InputVme space bounds 
  double b[6],p1[3],p2[3],d;
	if(m_InputVme->IsA("mafVMEGizmo"))
		m_InputVme->GetOutput()->GetVTKData()->GetBounds(b);
	else
		m_InputVme->GetOutput()->GetBounds(b);
  p1[0] = b[0];
  p1[1] = b[2];
  p1[2] = b[4];
  p2[0] = b[1];
  p2[1] = b[3];
  p2[2] = b[5];
  d = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));
    
  // create circle
  m_Circle = vtkDiskSource::New();
	m_Circle->SetCircumferentialResolution(50);
	m_Circle->SetInnerRadius(d / 2);
	m_Circle->SetOuterRadius(d / 2);

  m_Radius = d / 2;

  // clean the circle polydata
  m_CleanCircle = vtkCleanPolyData::New();
	m_CleanCircle->SetInput(m_Circle->GetOutput());

  // tube filter the circle 
  m_CircleTF = vtkTubeFilter::New();
  m_CircleTF->SetInput(m_CleanCircle->GetOutput());
  m_CircleTF->SetRadius(d / 200);
  m_CircleTF->SetNumberOfSides(20);
  
  // create rotation transform and rotation TPDF
  m_RotationTr = vtkTransform::New();
  m_RotationTr->Identity();

  m_RotatePDF = vtkTransformPolyDataFilter::New();
  m_RotatePDF->SetTransform(m_RotationTr);
  m_RotatePDF->SetInput(m_CircleTF->GetOutput());

}
//----------------------------------------------------------------------------
void mafGizmoRotateCircle::CreateISA()
//----------------------------------------------------------------------------
{
  // Create isa compositor and assign behaviors to IsaGen ivar.
  // Default isa constrain rotation around X axis.
  m_IsaComp = mmiCompositorMouse::New();

  // default behavior is activated by mouse left and is constrained to X axis,
  // default ref sys is input vme abs matrix
  m_IsaGen = m_IsaComp->CreateBehavior(MOUSE_LEFT);
  m_IsaGen->SetVME(m_InputVme);
  m_IsaGen->GetRotationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::LOCK, mmiConstraint::LOCK); 
  m_IsaGen->GetRotationConstraint()->GetRefSys()->SetTypeToCustom(m_AbsInputMatrix);
  m_IsaGen->GetPivotRefSys()->SetTypeToCustom(m_AbsInputMatrix);
  m_IsaGen->EnableRotation(true);

  //isa will send events to this
  m_IsaGen->SetListener(this);
}
//----------------------------------------------------------------------------
void mafGizmoRotateCircle::SetAxis(int axis) 
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
    m_IsaGen->GetRotationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::LOCK, mmiConstraint::LOCK);
  }
  else if (m_ActiveAxis == Y)
  {
    // set rotation to move gizmo normal to Y 
    m_RotationTr->Identity();
    m_RotationTr->RotateX(90);
 
    // set the color to green
    this->SetColor(0, 1, 0);

    // change the Gizmo constrain
    m_IsaGen->GetRotationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::FREE, mmiConstraint::LOCK);
  }
  else if (m_ActiveAxis == Z)
  {
    // reset circle orientation to move gizmo normal to Z
    m_RotationTr->Identity();
  
    // set the color to blue
    this->SetColor(0, 0, 1);
   
    // change the Gizmo constrain
    m_IsaGen->GetRotationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::LOCK, mmiConstraint::FREE);
  }    
}
//----------------------------------------------------------------------------
void mafGizmoRotateCircle::Highlight(bool highlight)
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
void  mafGizmoRotateCircle::SetRadius(double radius)
//----------------------------------------------------------------------------
{
  m_Radius = radius;
  m_Circle->SetInnerRadius(radius);
	m_Circle->SetOuterRadius(radius);
}
//----------------------------------------------------------------------------
void mafGizmoRotateCircle::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // mouse down change gizmo selection status
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    if (e->GetId() == ID_TRANSFORM)
    {
      if (e->GetArg() == mmiGenericMouse::MOUSE_DOWN)
      {
        this->SetIsActive(true);
      }
      // forward events to the listener
      e->SetSender(this);
      mafEventMacro(*e);
    }
    else
    {
      mafEventMacro(*e);
    }
  }
}
/** Gizmo color */
//----------------------------------------------------------------------------
void mafGizmoRotateCircle::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_Gizmo->GetMaterial()->m_Prop->SetColor(col);
	m_Gizmo->GetMaterial()->m_Prop->SetAmbient(0);
	m_Gizmo->GetMaterial()->m_Prop->SetDiffuse(1);
	m_Gizmo->GetMaterial()->m_Prop->SetSpecular(0);
}
//----------------------------------------------------------------------------
void mafGizmoRotateCircle::SetColor(double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(col);
}
//----------------------------------------------------------------------------
void mafGizmoRotateCircle::Show(bool show)
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,VME_SHOW,m_Gizmo,show));
}
//----------------------------------------------------------------------------
void mafGizmoRotateCircle::SetAbsPose(mafMatrix *absPose)
//----------------------------------------------------------------------------
{
  m_Gizmo->SetAbsMatrix(*absPose);
  SetRefSysMatrix(absPose);
}
//----------------------------------------------------------------------------
void mafGizmoRotateCircle::SetRefSysMatrix(mafMatrix *matrix)
//----------------------------------------------------------------------------
{  
  m_IsaGen->GetRotationConstraint()->GetRefSys()->SetTypeToCustom(matrix);
  m_IsaGen->GetPivotRefSys()->SetTypeToCustom(matrix);
}
//----------------------------------------------------------------------------
mafMatrix *mafGizmoRotateCircle::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_Gizmo->GetOutput()->GetAbsMatrix();
}
//----------------------------------------------------------------------------
void mafGizmoRotateCircle::SetInput(mafVME *vme)
//----------------------------------------------------------------------------
{
  this->m_InputVme = vme; 
  SetAbsPose(vme->GetOutput()->GetAbsMatrix()); 
}
//---------------------------------------------------------------------------
mmiGenericInterface *mafGizmoRotateCircle::GetInteractor()
//----------------------------------------------------------------------------
{
  return m_IsaGen;
}

int mafGizmoRotateCircle::GetAxis()
{
 return m_ActiveAxis;
}

double mafGizmoRotateCircle::GetRadius()
{
  return m_Radius;
}

void mafGizmoRotateCircle::SetListener( mafObserver *Listener )
{
  m_Listener = Listener;
}

void mafGizmoRotateCircle::SetIsActive( bool highlight )
{
  m_IsActive = highlight;
}

bool mafGizmoRotateCircle::GetIsActive()
{
  return m_IsActive;
}

mafVME * mafGizmoRotateCircle::GetInput()
{
  return this->m_InputVme;
}