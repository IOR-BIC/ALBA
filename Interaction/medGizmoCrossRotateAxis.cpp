/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGizmoCrossRotateAxis.cpp,v $
  Language:  C++
  Date:      $Date: 2010-12-06 17:23:19 $
  Version:   $Revision: 1.1.2.4 $
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


#include "medGizmoCrossRotateAxis.h"
// wxwin stuff
#include "wx/string.h"

#include "mafDecl.h"

// isa stuff
#include "mafInteractorCompositorMouse.h"
#include "mafInteractorGenericMouse.h"

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
#include "vtkLineSource.h"
#include "vtkAppendPolydata.h"
#include "medGizmoCrossRotateFan.h"

//----------------------------------------------------------------------------
medGizmoCrossRotateAxis::medGizmoCrossRotateAxis(mafVME *input, mafObserver *listener)
//----------------------------------------------------------------------------
{
	m_LineSourceEast = NULL;
	m_LineSourceWest = NULL;
	m_LineSourceNorth = NULL;
	m_LineSourceSouth = NULL;

	m_AppendPolyData = NULL;

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
  m_GizmoCross = mafVMEGizmo::New();
  m_GizmoCross->SetName("rotate cross");
  m_GizmoCross->SetData(m_RotatePDF->GetOutput());
  
  medGizmoCrossRotateFan *rotateFan = NULL;
  rotateFan = dynamic_cast<medGizmoCrossRotateFan *>(m_Listener);

  m_GizmoCross->SetMediator(rotateFan->GetMediator());

  // assign isa to S1 and S2;
  m_GizmoCross->SetBehavior(m_IsaComp);
  
  // set the axis to X axis
  this->SetAxis(m_ActiveAxis);

  m_AbsInputMatrix = m_InputVme->GetOutput()->GetAbsMatrix();
  m_InputVme->GetOutput()->Update();
  SetAbsPose(m_AbsInputMatrix);
  SetRefSysMatrix(m_AbsInputMatrix);

  // add the gizmo to the tree, this should increase reference count  
  m_GizmoCross->ReparentTo(mafVME::SafeDownCast(m_InputVme->GetRoot()));

}
//----------------------------------------------------------------------------
medGizmoCrossRotateAxis::~medGizmoCrossRotateAxis() 
//----------------------------------------------------------------------------
{
  m_GizmoCross->SetBehavior(NULL);

  vtkDEL(m_LineSourceEast);
  vtkDEL(m_LineSourceWest);
  vtkDEL(m_LineSourceNorth);
  vtkDEL(m_LineSourceSouth);

  vtkDEL(m_AppendPolyData);
  vtkDEL(m_CleanCircle);
  vtkDEL(m_CircleTF);
  vtkDEL(m_RotationTr); 
  vtkDEL(m_RotatePDF); 
	//----------------------
	// No leaks so somebody is performing this...
	//----------------------
  vtkDEL(m_IsaComp); 
  
	m_GizmoCross->ReparentTo(NULL);
}
//----------------------------------------------------------------------------
void medGizmoCrossRotateAxis::CreatePipeline() 
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
    
  // create line
  m_LineSourceEast = vtkLineSource::New();
  m_LineSourceEast->SetPoint1(d/2,0,0);
  m_LineSourceEast->SetPoint2(d/2 * 3,0,0);

  m_LineSourceWest = vtkLineSource::New();
  m_LineSourceWest->SetPoint1(-d/2,0,0);
  m_LineSourceWest->SetPoint2(-d/2 * 3,0,0);

  m_LineSourceNorth = vtkLineSource::New();
  m_LineSourceNorth->SetPoint1(0,d/2,0);
  m_LineSourceNorth->SetPoint2(0,d/2 * 3,0);
  
  m_LineSourceSouth = vtkLineSource::New();
  m_LineSourceSouth->SetPoint1(0,-d/2,0);
  m_LineSourceSouth->SetPoint2(0,-d/2 * 3,0);

  m_Radius = d / 2;

  m_AppendPolyData = vtkAppendPolyData::New();
  m_AppendPolyData->AddInput(m_LineSourceEast->GetOutput());
  m_AppendPolyData->AddInput(m_LineSourceWest->GetOutput());
  m_AppendPolyData->AddInput(m_LineSourceNorth->GetOutput());
  m_AppendPolyData->AddInput(m_LineSourceSouth->GetOutput());

  // clean the circle polydata
  m_CleanCircle = vtkCleanPolyData::New();
  m_CleanCircle->SetInput(m_AppendPolyData->GetOutput());

  // tube filter the circle 
  m_CircleTF = vtkTubeFilter::New();
  m_CircleTF->SetInput(m_CleanCircle->GetOutput());
  m_CircleTF->SetRadius(d / 400);
  m_CircleTF->SetNumberOfSides(20);
  
  // create rotation transform and rotation TPDF
  m_RotationTr = vtkTransform::New();
  m_RotationTr->Identity();

  m_RotatePDF = vtkTransformPolyDataFilter::New();
  m_RotatePDF->SetTransform(m_RotationTr);
  m_RotatePDF->SetInput(m_CircleTF->GetOutput());

}
//----------------------------------------------------------------------------
void medGizmoCrossRotateAxis::CreateISA()
//----------------------------------------------------------------------------
{
  // Create isa compositor and assign behaviors to IsaGen ivar.
  // Default isa constrain rotation around X axis.
  m_IsaComp = mafInteractorCompositorMouse::New();

  // default behavior is activated by mouse left and is constrained to X axis,
  // default ref sys is input vme abs matrix
  m_IsaGen = m_IsaComp->CreateBehavior(MOUSE_LEFT);
  m_IsaGen->SetVME(m_InputVme);
  m_IsaGen->GetRotationConstraint()->SetConstraintModality(mafInteractorConstraint::FREE, mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK); 
  m_IsaGen->GetRotationConstraint()->GetRefSys()->SetTypeToCustom(m_AbsInputMatrix);
  m_IsaGen->GetPivotRefSys()->SetTypeToCustom(m_AbsInputMatrix);
  m_IsaGen->EnableRotation(true);

  //isa will send events to this
  m_IsaGen->SetListener(this);
}
//----------------------------------------------------------------------------
void medGizmoCrossRotateAxis::SetAxis(int axis) 
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
    m_IsaGen->GetRotationConstraint()->SetConstraintModality(mafInteractorConstraint::FREE, mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK);
  }
  else if (m_ActiveAxis == Y)
  {
    // set rotation to move gizmo normal to Y 
    m_RotationTr->Identity();
    m_RotationTr->RotateX(90);
 
    // set the color to green
    this->SetColor(0, 1, 0);

    // change the Gizmo constrain
    m_IsaGen->GetRotationConstraint()->SetConstraintModality(mafInteractorConstraint::LOCK, mafInteractorConstraint::FREE, mafInteractorConstraint::LOCK);
  }
  else if (m_ActiveAxis == Z)
  {
    // reset circle orientation to move gizmo normal to Z
    m_RotationTr->Identity();
  
    // set the color to blue
    this->SetColor(0, 0, 1);
   
    // change the Gizmo constrain
    m_IsaGen->GetRotationConstraint()->SetConstraintModality(mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK, mafInteractorConstraint::FREE);
  }    
}
//----------------------------------------------------------------------------
void medGizmoCrossRotateAxis::Highlight(bool highlight)
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
void medGizmoCrossRotateAxis::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // mouse down change gizmo selection status
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    if (e->GetId() == ID_TRANSFORM)
    {
      if (e->GetArg() == mafInteractorGenericMouse::MOUSE_DOWN)
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
void medGizmoCrossRotateAxis::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_GizmoCross->GetMaterial()->m_Prop->SetColor(col);
	m_GizmoCross->GetMaterial()->m_Prop->SetAmbient(0);
	m_GizmoCross->GetMaterial()->m_Prop->SetDiffuse(1);
	m_GizmoCross->GetMaterial()->m_Prop->SetSpecular(0);
}
//----------------------------------------------------------------------------
void medGizmoCrossRotateAxis::SetColor(double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(col);
}
//----------------------------------------------------------------------------
void medGizmoCrossRotateAxis::Show(bool show)
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,VME_SHOW,m_GizmoCross,show));
}
//----------------------------------------------------------------------------
void medGizmoCrossRotateAxis::SetAbsPose(mafMatrix *absPose)
//----------------------------------------------------------------------------
{
  m_GizmoCross->SetAbsMatrix(*absPose);
  SetRefSysMatrix(absPose);
}
//----------------------------------------------------------------------------
void medGizmoCrossRotateAxis::SetRefSysMatrix(mafMatrix *matrix)
//----------------------------------------------------------------------------
{  
  m_IsaGen->GetRotationConstraint()->GetRefSys()->SetTypeToCustom(matrix);
  m_IsaGen->GetPivotRefSys()->SetTypeToCustom(matrix);
}
//----------------------------------------------------------------------------
mafMatrix *medGizmoCrossRotateAxis::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_GizmoCross->GetOutput()->GetAbsMatrix();
}
//----------------------------------------------------------------------------
void medGizmoCrossRotateAxis::SetInput(mafVME *vme)
//----------------------------------------------------------------------------
{
  this->m_InputVme = vme; 
  SetAbsPose(vme->GetOutput()->GetAbsMatrix()); 
}
//---------------------------------------------------------------------------
mafInteractorGenericInterface *medGizmoCrossRotateAxis::GetInteractor()
//----------------------------------------------------------------------------
{
  return m_IsaGen;
}

int medGizmoCrossRotateAxis::GetAxis()
{
 return m_ActiveAxis;
}

double medGizmoCrossRotateAxis::GetRadius()
{
  return m_Radius;
}

void medGizmoCrossRotateAxis::SetListener( mafObserver *Listener )
{
  m_Listener = Listener;
}

void medGizmoCrossRotateAxis::SetIsActive( bool isActive )
{
  m_IsActive = isActive;
}

bool medGizmoCrossRotateAxis::GetIsActive()
{
  return m_IsActive;
}

mafVME * medGizmoCrossRotateAxis::GetInput()
{
  return this->m_InputVme;
}
