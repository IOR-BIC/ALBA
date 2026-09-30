/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoTranslatePlane
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


#include "albaGizmoTranslatePlane.h"
#include "albaDecl.h"
#include "albaRefSys.h"

// isa stuff
#include "albaInteractorCompositorMouse.h"
#include "albaInteractorGenericMouse.h"

// vme stuff
#include "mmaMaterial.h"
#include "albaVME.h"
#include "albaVMEGizmo.h"
#include "albaVMEOutput.h"

// vtk stuff
#include "vtkLineSource.h"
#include "vtkPlaneSource.h"
#include "vtkTubeFilter.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkProperty.h"

//----------------------------------------------------------------------------
albaGizmoTranslatePlane::albaGizmoTranslatePlane(albaVME *input, albaObserver *listener)
//----------------------------------------------------------------------------
{
  this->SetIsActive(false);
  
  m_IsaComp[0]  = m_IsaComp[1] =  NULL;
  m_Listener  = listener;
  m_InputVme    = input;
  m_Length = 1;
  
  // default plane is YZ
  m_ActivePlane = YZ;
  
  //-----------------
  // pivot stuff
  //-----------------
  // pivotTransform is useless for this operation but required by isa generic
  m_PivotTransform = vtkTransform::New();

  // create pipeline stuff
  CreatePipeline();

  // create isa stuff
  CreateISA();

  //-----------------
  // create vme gizmos stuff
  //-----------------
  albaString vmeName;
  int i;
  for (i = 0; i < 3; i++)
  {
    // the ith gizmo
    m_Gizmo[i] = albaVMEGizmo::New();
    vmeName = "part";
    vmeName << i;
    m_Gizmo[i]->SetName(vmeName.GetCStr());
    m_Gizmo[i]->SetDataConnection(m_RotatePDF[i]->GetOutputPort());
		m_Gizmo[i]->SetMediator(m_Listener);
  }

  // assign isa to S1 and S2;
  m_Gizmo[S1]->SetBehavior(m_IsaComp[S1]);
  m_Gizmo[S2]->SetBehavior(m_IsaComp[S2]);

  albaMatrix *absInputMatrix = m_InputVme->GetOutput()->GetAbsMatrix();
  SetAbsPose(absInputMatrix);
  SetConstrainRefSys(absInputMatrix);

  /*
      z
      ^  S2: blue
      |-----
      |     |         
      | SQ  |S1: green         
      |     |         
       --------> y   
      SQ: yellow
  */

  // set come gizmo material property and initial color 
  this->SetColor(S1, 0, 1, 0);
  this->SetColor(S2, 0, 0, 1);
  this->SetColor(SQ, 1, 1, 0);
  
  // add the gizmo to the tree, this should increase reference count 
  for (i = 0; i < 3; i++)
  {
    m_Gizmo[i]->ReparentTo(m_InputVme->GetRoot());
  }
}
//----------------------------------------------------------------------------
albaGizmoTranslatePlane::~albaGizmoTranslatePlane() 
//----------------------------------------------------------------------------
{
  m_Gizmo[S1]->SetBehavior(NULL);
  m_Gizmo[S2]->SetBehavior(NULL);
  m_Gizmo[SQ]->SetBehavior(NULL);
  
  vtkDEL(m_Line[S1]);
  vtkDEL(m_Line[S2]);
  vtkDEL(m_Plane);
  vtkDEL(m_RotationTr);

  // clean up
  int i;
  for (i = 0; i < SQ; i++)
  {
    vtkDEL(m_LineTF[i]);
    vtkDEL(m_IsaComp[i]); 
  }

  m_PivotTransform->Delete();

  for (i = 0; i < 3; i++)
  {
    vtkDEL(m_RotatePDF[i]);
		m_Gizmo[i]->ReparentTo(NULL);
  }
}

//----------------------------------------------------------------------------
void albaGizmoTranslatePlane::CreatePipeline() 
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

  /*
        z
        ^  S2
(0,0,1) |----- (0,1,1)
        |     |         
        | SQ  |S1          
        |     |         
        x--------> y    

            (0,1,0)
  
  */

  // create pipeline for cone-cylinder gizmo along global X axis
  
  // create S1
  m_Line[S1] = vtkLineSource::New();  
  m_Line[S1]->SetPoint1(0, 1, 0);
  m_Line[S1]->SetPoint2(0, 1, 1);

  // create S2
  m_Line[S2] = vtkLineSource::New();
  m_Line[S2]->SetPoint1(0, 0, 1);
  m_Line[S2]->SetPoint2(0, 1, 1);

  // create SQ
  m_Plane = vtkPlaneSource::New();
  m_Plane->SetOrigin(0, 0, 0);
  m_Plane->SetPoint1(0, 1, 0);
  m_Plane->SetPoint2(0, 0, 1);

  // create tube filter for the segments
  int i;
  for (i = 0; i < SQ; i++)
  {
    m_LineTF[i] = vtkTubeFilter::New();
    m_LineTF[i]->SetInputConnection(m_Line[i]->GetOutputPort());
    m_LineTF[i]->SetRadius(d / 200);
    m_LineTF[i]->SetNumberOfSides(20);
  }

  //-----------------
  // update segments and square dimension based on vme bb diagonal
  //-----------------
  this->SetSizeLength(d / 8);

  //-----------------
  m_RotationTr = vtkTransform::New();
  m_RotationTr->Identity();

  // create rotation transform and rotation TPDF 
  for (i = 0; i < SQ; i++)
  {
    m_RotatePDF[i] = vtkTransformPolyDataFilter::New();
    m_RotatePDF[i]->SetTransform(m_RotationTr);
    m_RotatePDF[i]->SetInputConnection(m_LineTF[i]->GetOutputPort());
  }
  m_RotatePDF[SQ] = vtkTransformPolyDataFilter::New();
  m_RotatePDF[SQ]->SetTransform(m_RotationTr);
  m_RotatePDF[SQ]->SetInputConnection(m_Plane->GetOutputPort());
}

//----------------------------------------------------------------------------
void albaGizmoTranslatePlane::CreateISA()
//----------------------------------------------------------------------------
{
  // Create isa compositor and assign behaviors to IsaGen ivar.
  // Default isa is constrained to plane XZ.
  for (int i = 0; i < SQ; i++)
  {
    m_IsaComp[i] = albaInteractorCompositorMouse::New();

    // default behavior is activated by mouse left and is constrained to X axis,
    // default ref sys is input vme abs matrix
    m_IsaGen[i] = m_IsaComp[i]->CreateBehavior(MOUSE_LEFT);
    m_IsaGen[i]->SetVME(m_InputVme);
    m_IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE);     
    
    //isa will send events to this
    m_IsaGen[i]->SetListener(this);
  }
}

//----------------------------------------------------------------------------
void albaGizmoTranslatePlane::SetPlane(int plane) 
//----------------------------------------------------------------------------
{
  // this should be called when the translation gizmo
  // is created; gizmos are not highlighted
  
  // register the plane
  m_ActivePlane = plane;
  
  // rotate the gizmo components to match the specified plane
  if (m_ActivePlane == YZ)
  {
    // reset cyl and cone rotation
    m_RotationTr->Identity();
  
    // set S1 and S2 color
    this->SetColor(S1, 0, 1, 0);
    this->SetColor(S2, 0, 0, 1);

    // change the axis constrain
    for (int i = 0; i < 2; i++)
    {
      m_IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE, albaInteractorConstraint::FREE);
    }
  }
  else if (m_ActivePlane == XZ)
  {
    // set rotation to move con and cyl on Y 
    m_RotationTr->Identity();
    m_RotationTr->RotateZ(-90);
   
    // set S1 and S2 color
    this->SetColor(S1, 1, 0, 0);
    this->SetColor(S2, 0, 0, 1);

    // change the axis constrain
    for (int i = 0; i < 2; i++)
    {
      m_IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE);
    }
  }  
  else if (m_ActivePlane == XY)
  {
    // set rotation to move con and cyl on Z
    m_RotationTr->Identity();
    m_RotationTr->RotateY(90);
    
    // set S1 and S2 color
    this->SetColor(S1, 0, 1, 0);
    this->SetColor(S2, 1, 0, 0);

     // change the axis constrain
    for (int i = 0; i < 2; i++)
    {
      m_IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
    }
  }  
}

//----------------------------------------------------------------------------
void albaGizmoTranslatePlane::Highlight(bool highlight)
//----------------------------------------------------------------------------
{
  if (highlight == true)
  {
   // Highlight the S1 and S2  by setting its color to yellow 
   this->SetColor(S1, 1, 1, 0);
   this->SetColor(S2, 1, 1, 0);

   // Show the square
   ShowSquare(true);

  } 
  else
  {
   // restore original color 
   if (m_ActivePlane == YZ)
   {
    // set S1 and S2 color
    this->SetColor(S1, 0, 1, 0);
    this->SetColor(S2, 0, 0, 1);
   } 
   else if (m_ActivePlane == XZ)
   {
     // set S1 and S2 color
    this->SetColor(S1, 1, 0, 0);
    this->SetColor(S2, 0, 0, 1);
   }
   else if (m_ActivePlane == XY)
   {     
    // set S1 and S2 color
    this->SetColor(S1, 0, 1, 0);
    this->SetColor(S2, 1, 0, 0);
   } 

   // Hide the square
   ShowSquare(false);
  }
}

//----------------------------------------------------------------------------
void  albaGizmoTranslatePlane::SetSizeLength(double length)
//----------------------------------------------------------------------------
{
  /*
          z
          ^  S2
  (0,0,L) |----- (0,L,L)
          |     |         
          | SQ  |S1          
          |     |         
          x--------> y    

                (0,L,0)
  */

  // register the gizmo length
  m_Length = length;
  double L = length;
  // update S1
  m_Line[S1]->SetPoint1(0, L, 0);
  m_Line[S1]->SetPoint2(0, L, L);

  // update S2
  m_Line[S2]->SetPoint1(0, 0, L);
  m_Line[S2]->SetPoint2(0, L, L);

  // update SQ
  m_Plane->SetOrigin(0, 0, 0);
  m_Plane->SetPoint1(0, L, 0);
  m_Plane->SetPoint2(0, 0, L);
}
//----------------------------------------------------------------------------
void albaGizmoTranslatePlane::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    // mouse down change gizmo selection status
    if (e->GetId() == ID_TRANSFORM)
    {
      if (e->GetArg() == albaInteractorGenericMouse::MOUSE_DOWN)
      {
        this->SetIsActive(true);
      }
    }
    // forward events to the listener
    e->SetSender(this);
    albaEventMacro(*e);
  }
}
/** Gizmo color */
//----------------------------------------------------------------------------
void albaGizmoTranslatePlane::SetColor(int part, double col[3])
//----------------------------------------------------------------------------
{
  if (part == S1 || part == S2 || part == SQ)
  {
    m_Gizmo[part]->GetMaterial()->m_Prop->SetColor(col);
	  m_Gizmo[part]->GetMaterial()->m_Prop->SetAmbient(0);
	  m_Gizmo[part]->GetMaterial()->m_Prop->SetDiffuse(1);
	  m_Gizmo[part]->GetMaterial()->m_Prop->SetSpecular(0);
  }
}
//----------------------------------------------------------------------------
void albaGizmoTranslatePlane::SetColor(int part, double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(part, col);
}
//----------------------------------------------------------------------------
void albaGizmoTranslatePlane::Show(bool show)
//----------------------------------------------------------------------------
{
  for (int i = 0; i < 3; i++)
		GetLogicManager()->VmeShow(m_Gizmo[i], show);
}
//----------------------------------------------------------------------------
void albaGizmoTranslatePlane::ShowSquare(bool show)
//----------------------------------------------------------------------------
{
  double opacity = ((show == true) ? 0.5 : 0);
  m_Gizmo[SQ]->GetMaterial()->m_Prop->SetOpacity(opacity);
}
//----------------------------------------------------------------------------
void albaGizmoTranslatePlane::SetAbsPose(albaMatrix *absPose)
//----------------------------------------------------------------------------
{
  for (int i = 0; i < 3; i++)
  {  
    m_Gizmo[i]->SetAbsMatrix(*absPose);
  }
  
  SetConstrainRefSys(absPose);
}
//----------------------------------------------------------------------------
void albaGizmoTranslatePlane::SetConstrainRefSys(albaMatrix *constrain)
//----------------------------------------------------------------------------
{  
  for (int i = 0; i < SQ; i++)
  {
    m_IsaGen[i]->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(constrain);
  }
}
//----------------------------------------------------------------------------
albaMatrix *albaGizmoTranslatePlane::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_Gizmo[S1]->GetOutput()->GetAbsMatrix();
}
//----------------------------------------------------------------------------
void albaGizmoTranslatePlane::SetInput(albaVME *vme)
//----------------------------------------------------------------------------
{
  this->m_InputVme = vme; 
  SetAbsPose(vme->GetOutput()->GetAbsMatrix()); 
  SetConstrainRefSys(vme->GetOutput()->GetAbsMatrix());
}
//----------------------------------------------------------------------------
void albaGizmoTranslatePlane::SetConstraintModality(int axis, int constrainModality)
//----------------------------------------------------------------------------
{
  for (int i = 0; i < SQ; i++)
  {
    m_IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(axis,constrainModality);
  }
}
//----------------------------------------------------------------------------
void albaGizmoTranslatePlane::SetStep(int axis, double step)
//----------------------------------------------------------------------------
{
  for (int i = 0; i < SQ; i++)
  {
    m_IsaGen[i]->GetTranslationConstraint()->SetStep(axis,step);
  }
}
