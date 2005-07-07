/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoRotateCircle.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-07 15:13:43 $
  Version:   $Revision: 1.2 $
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
  ActiveAxis = X;

  IsaComp = NULL;

  m_Listener = listener;
  InputVme = input;

  //-----------------

  // create pipeline stuff
  CreatePipeline();

  // create isa stuff
  CreateISA();

  //-----------------
  // create vme gizmo stuff
  //-----------------
  
  // the circle gizmo
  Gizmo = mafVMEGizmo::New();
  Gizmo->SetName("circle");
  Gizmo->ReparentTo(mafVME::SafeDownCast(InputVme->GetRoot()));
  Gizmo->SetData(RotatePDF->GetOutput());

  // gizmo data
  //GizmoData = new mafVmeData(Gizmo, this);
  //Gizmo->SetClientData(GizmoData);
  
  // assign isa to S1 and S2;
  //GizmoData->m_behavior = IsaComp; 
  Gizmo->SetBehavior(IsaComp);
  
  // set the axis to X axis
  this->SetAxis(ActiveAxis);

  InputVme->GetOutput()->Update();
  SetAbsPose(InputVme->GetOutput()->GetAbsMatrix());
  SetRefSysMatrix(InputVme->GetOutput()->GetAbsMatrix());

  // hide gizmos at creation
  this->Show(false);
  
  // add the gizmo to the tree, this should increase reference count  
  mafEventMacro(mafEvent(this, VME_ADD, Gizmo));
}
//----------------------------------------------------------------------------
mafGizmoRotateCircle::~mafGizmoRotateCircle() 
//----------------------------------------------------------------------------
{
  Gizmo->SetBehavior(NULL);
  
  vtkDEL(Circle);
  vtkDEL(CleanCircle);
  vtkDEL(CircleTF);
  vtkDEL(RotationTr); 
  vtkDEL(RotatePDF); 
	//----------------------
	// No leaks so somebody is performing this...
	// wxDEL(GizmoData[i]);
	//----------------------
  vtkDEL(IsaComp); 
  
  mafEventMacro(mafEvent(this, VME_REMOVE, Gizmo));
  //vtkDEL(Gizmo);
}

//----------------------------------------------------------------------------
void mafGizmoRotateCircle::CreatePipeline() 
//----------------------------------------------------------------------------
{
  // calculate diagonal of InputVme space bounds 
  double b[6],p1[3],p2[3],d;
  InputVme->GetOutput()->GetBounds(b);
  p1[0] = b[0];
  p1[1] = b[2];
  p1[2] = b[4];
  p2[0] = b[1];
  p2[1] = b[3];
  p2[2] = b[5];
  d = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));
  
  // create circle
  Circle = vtkDiskSource::New();
	Circle->SetCircumferentialResolution(50);
	Circle->SetInnerRadius(d / 2);
	Circle->SetOuterRadius(d / 2);

  // clean the circle polydata
  CleanCircle = vtkCleanPolyData::New();
	CleanCircle->SetInput(Circle->GetOutput());

  // tube filter the circle 
  CircleTF = vtkTubeFilter::New();
  CircleTF->SetInput(CleanCircle->GetOutput());
  CircleTF->SetRadius(d / 200);
  CircleTF->SetNumberOfSides(20);
  
  // create rotation transform and rotation TPDF
  RotationTr = vtkTransform::New();
  RotationTr->Identity();

  RotatePDF = vtkTransformPolyDataFilter::New();
  RotatePDF->SetTransform(RotationTr);
  RotatePDF->SetInput(CircleTF->GetOutput());
}

//----------------------------------------------------------------------------
void mafGizmoRotateCircle::CreateISA()
//----------------------------------------------------------------------------
{
  
  // Create isa compositor and assign behaviors to IsaGen ivar.
  // Default isa constrain rotation around X axis.
  
  IsaComp = mmiCompositorMouse::New();

  // default behavior is activated by mouse left and is constrained to X axis,
  // default ref sys is input vme abs matrix
  IsaGen = IsaComp->CreateBehavior(MOUSE_LEFT);
  IsaGen->SetVME(InputVme);
  IsaGen->GetRotationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::LOCK, mmiConstraint::LOCK); 
  IsaGen->GetRotationConstraint()->GetRefSys()->SetTypeToCustom(InputVme->GetOutput()->GetAbsMatrix());
  IsaGen->GetPivotRefSys()->SetTypeToCustom(InputVme->GetOutput()->GetAbsMatrix());
  IsaGen->EnableRotation(true);

  //isa will send events to this
  IsaGen->SetListener(this);
}

//----------------------------------------------------------------------------
void mafGizmoRotateCircle::SetAxis(int axis) 
//----------------------------------------------------------------------------
{
  // this should be called when the gizmo
  // is created; gizmos are not highlighted
  
  // register the axis
  ActiveAxis = axis;
  
  // rotate the gizmo components to match the specified axis
  
  if (ActiveAxis == X)
  {
    // set rotation to move gizmo normal to X
    RotationTr->Identity();
    RotationTr->RotateY(90);
    
    // set the color to red
    this->SetColor(1, 0, 0);

     // change the axis constrain  
    IsaGen->GetRotationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::LOCK, mmiConstraint::LOCK);
  }
  else if (ActiveAxis == Y)
  {
    // set rotation to move gizmo normal to Y 
    RotationTr->Identity();
    RotationTr->RotateX(90);
 
    // set the color to green
    this->SetColor(0, 1, 0);

    // change the Gizmo constrain
    IsaGen->GetRotationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::FREE, mmiConstraint::LOCK);
  }
  else if (ActiveAxis == Z)
  {
    // reset circle orientation to move gizmo normal to Z
    RotationTr->Identity();
  
    // set the color to blue
    this->SetColor(0, 0, 1);
   
    // change the Gizmo constrain
    IsaGen->GetRotationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::LOCK, mmiConstraint::FREE);
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
   if (ActiveAxis == X)
   {
    // X circle to red
    this->SetColor(1, 0, 0);
   } 
   else if (ActiveAxis == Y)
   {
     // Y circle to green
    this->SetColor(0, 1, 0);
   }
   else if (ActiveAxis == Z)
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
  Circle->SetInnerRadius(radius);
	Circle->SetOuterRadius(radius);
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
  Gizmo->GetMaterial()->m_Prop->SetColor(col);
	Gizmo->GetMaterial()->m_Prop->SetAmbient(0);
	Gizmo->GetMaterial()->m_Prop->SetDiffuse(1);
	Gizmo->GetMaterial()->m_Prop->SetSpecular(0);
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
  if (show == TRUE)
  {  
    Gizmo->GetMaterial()->m_Prop->SetOpacity(1);
  }
  else
  {
    Gizmo->GetMaterial()->m_Prop->SetOpacity(0);
  }
}

//----------------------------------------------------------------------------
void mafGizmoRotateCircle::SetAbsPose(mafMatrix *absPose)
//----------------------------------------------------------------------------
{
  Gizmo->SetAbsMatrix(*absPose);
  SetRefSysMatrix(absPose);
}

//----------------------------------------------------------------------------
void mafGizmoRotateCircle::SetRefSysMatrix(mafMatrix *matrix)
//----------------------------------------------------------------------------
{  
  IsaGen->GetRotationConstraint()->GetRefSys()->SetTypeToCustom(matrix);
  IsaGen->GetPivotRefSys()->SetTypeToCustom(matrix);
}

//----------------------------------------------------------------------------
mafMatrix *mafGizmoRotateCircle::GetAbsPose()
//----------------------------------------------------------------------------
{
  return Gizmo->GetOutput()->GetAbsMatrix();
}

//----------------------------------------------------------------------------
void mafGizmoRotateCircle::SetInput(mafVME *vme)
//----------------------------------------------------------------------------
{
  this->InputVme = vme; 
  SetAbsPose(vme->GetOutput()->GetAbsMatrix()); 
}

//---------------------------------------------------------------------------
mmiGenericInterface *mafGizmoRotateCircle::GetInteractor()
//----------------------------------------------------------------------------
{
  return IsaGen;
}
