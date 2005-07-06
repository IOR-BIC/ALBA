/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoTranslatePlane.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-06 13:50:27 $
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


#include "mafGizmoTranslatePlane.h"
#include "mafDecl.h"

// isa stuff
#include "mmiCompositorMouse.h"
#include "mmiGenericMouse.h"

// vme stuff
#include "mmaMaterial.h"
#include "mafVME.h"
#include "mafVMEGizmo.h"
#include "mafVMEOutput.h"

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
mafGizmoTranslatePlane::mafGizmoTranslatePlane(mafVME *input, mafObserver *listener)
//----------------------------------------------------------------------------
{
  this->SetIsActive(false);
  
  IsaComp[0] = IsaComp[1] =  NULL;

  m_Listener = listener;
  InputVme = input;
  
  // default plane is YZ
  ActivePlane = YZ;
  
  //-----------------
  // pivot stuff
  //-----------------

  // pivotTransform is useless for this operation but required by isa generic
  PivotTransform = vtkTransform::New();

  ////-----------------
  // create pipeline stuff
  CreatePipeline();

  // create isa stuff
  CreateISA();

  //-----------------
  // create vme gizmos stuff
  //-----------------
  mafString vmeName;
  for (int i = 0; i < 3; i++)
  {
    // the ith gizmo
    Gizmo[i] = mafVMEGizmo::New();
  
    vmeName = "part";
    vmeName << i;
    Gizmo[i]->SetName(vmeName.GetCStr());
    Gizmo[i]->ReparentTo(mafVME::SafeDownCast(InputVme->GetRoot()));
    Gizmo[i]->SetData(RotatePDF[i]->GetOutput());
  
    // gizmo data
    //GizmoData[i] = new mafVmeData(Gizmo[i], this);
    //Gizmo[i]->SetClientData(GizmoData[i]);
  }

  // assign isa to S1 and S2;
  Gizmo[S1]->SetBehavior(IsaComp[S1]);
  Gizmo[S2]->SetBehavior(IsaComp[S2]);

  SetAbsPose(InputVme->GetOutput()->GetAbsMatrix());
  SetConstrainRefSys(InputVme->GetOutput()->GetAbsMatrix());

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

  // hide gizmos at creation
  this->Show(false);
  
  //-----------------
  
  // add the gizmo to the tree, this should increase reference count 
  for (i = 0; i < 3; i++)
  {
    mafEventMacro(mafEvent(this, VME_ADD, Gizmo[i]));
  }
  
}
//----------------------------------------------------------------------------
mafGizmoTranslatePlane::~mafGizmoTranslatePlane() 
//----------------------------------------------------------------------------
{
  Gizmo[S1]->SetBehavior(NULL);
  Gizmo[S2]->SetBehavior(NULL);
  Gizmo[SQ]->SetBehavior(NULL);
  
  vtkDEL(Line[S1]);
  vtkDEL(Line[S2]);
  vtkDEL(Plane);
  vtkDEL(RotationTr);

  // clean up
  for (int i = 0; i < SQ; i++)
  {
    vtkDEL(LineTF[i]);
	//----------------------
	// No leaks so somebody is performing this...
	// wxDEL(GizmoData[i]);
	//----------------------
    vtkDEL(IsaComp[i]); 
  }

  PivotTransform->Delete();

  for (i = 0; i < 3; i++)
  {
    vtkDEL(RotatePDF[i]);
    mafEventMacro(mafEvent(this, VME_REMOVE, Gizmo[i]));
    Gizmo[i]->Delete();
  }
}

//----------------------------------------------------------------------------
void mafGizmoTranslatePlane::CreatePipeline() 
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
  Line[S1] = vtkLineSource::New();  
  Line[S1]->SetPoint1(0, 1, 0);
  Line[S1]->SetPoint2(0, 1, 1);

  // create S2
  Line[S2] = vtkLineSource::New();
  Line[S2]->SetPoint1(0, 0, 1);
  Line[S2]->SetPoint2(0, 1, 1);

  // create SQ
  Plane = vtkPlaneSource::New();
  Plane->SetOrigin(0, 0, 0);
  Plane->SetPoint1(0, 1, 0);
  Plane->SetPoint2(0, 0, 1);

  // create tube filter for the segments
  for (int i = 0; i < SQ; i++)
  {
    LineTF[i] = vtkTubeFilter::New();
    LineTF[i]->SetInput(Line[i]->GetOutput());
    LineTF[i]->SetRadius(d / 200);
    LineTF[i]->SetNumberOfSides(20);
  }

  //-----------------
  // update segments and square dimension based on vme bb diagonal
  //-----------------

  this->SetSizeLength(d / 8);

  //-----------------

  RotationTr = vtkTransform::New();
  RotationTr->Identity();

  // create rotation transform and rotation TPDF 
  for (i = 0; i < SQ; i++)
  {
    RotatePDF[i] = vtkTransformPolyDataFilter::New();
    RotatePDF[i]->SetTransform(RotationTr);
    RotatePDF[i]->SetInput(LineTF[i]->GetOutput());
  }

  RotatePDF[SQ] = vtkTransformPolyDataFilter::New();
  RotatePDF[SQ]->SetTransform(RotationTr);
  RotatePDF[SQ]->SetInput(Plane->GetOutput());
}

//----------------------------------------------------------------------------
void mafGizmoTranslatePlane::CreateISA()
//----------------------------------------------------------------------------
{
  
  // Create isa compositor and assign behaviors to IsaGen ivar.
  // Default isa is constrained to plane XZ.

  for (int i = 0; i < SQ; i++)
  {
    IsaComp[i] = mmiCompositorMouse::New();

    // default behavior is activated by mouse left and is constrained to X axis,
    // default ref sys is input vme abs matrix
    IsaGen[i] = IsaComp[i]->CreateBehavior(MOUSE_LEFT);
    IsaGen[i]->SetVME(InputVme);
    IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::LOCK, mmiConstraint::FREE);     
    
    //isa will send events to this
    IsaGen[i]->SetListener(this);
  }
}

//----------------------------------------------------------------------------
void mafGizmoTranslatePlane::SetPlane(int plane) 
//----------------------------------------------------------------------------
{
  // this should be called when the translation gizmo
  // is created; gizmos are not highlighted
  
  // register the plane
  ActivePlane = plane;
  
  // rotate the gizmo components to match the specified plane
  
  if (ActivePlane == YZ)
  {
    // reset cyl and cone rotation
    RotationTr->Identity();
  
    // set S1 and S2 color
    this->SetColor(S1, 0, 1, 0);
    this->SetColor(S2, 0, 0, 1);

    // change the axis constrain
    for (int i = 0; i < 2; i++)
    {
      IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::FREE, mmiConstraint::FREE);
    }
  }
  else if (ActivePlane == XZ)
  {
    // set rotation to move con and cyl on Y 
    RotationTr->Identity();
    RotationTr->RotateZ(-90);
   
    // set S1 and S2 color
    this->SetColor(S1, 1, 0, 0);
    this->SetColor(S2, 0, 0, 1);

    // change the axis constrain
    for (int i = 0; i < 2; i++)
    {
      IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::LOCK, mmiConstraint::FREE);
    }
  }  
  else if (ActivePlane == XY)
  {
    // set rotation to move con and cyl on Z
    RotationTr->Identity();
    RotationTr->RotateY(90);
    
    // set S1 and S2 color
    this->SetColor(S1, 0, 1, 0);
    this->SetColor(S2, 1, 0, 0);

     // change the axis constrain
    for (int i = 0; i < 2; i++)
    {
      IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
    }
  }  
}

//----------------------------------------------------------------------------
void mafGizmoTranslatePlane::Highlight(bool highlight)
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
   if (ActivePlane == YZ)
   {
    // set S1 and S2 color
    this->SetColor(S1, 0, 1, 0);
    this->SetColor(S2, 0, 0, 1);
   } 
   else if (ActivePlane == XZ)
   {
     // set S1 and S2 color
    this->SetColor(S1, 1, 0, 0);
    this->SetColor(S2, 0, 0, 1);
   }
   else if (ActivePlane == XY)
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
void  mafGizmoTranslatePlane::SetSizeLength(double length)
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

  double L = length;
 
  // update S1
  Line[S1]->SetPoint1(0, L, 0);
  Line[S1]->SetPoint2(0, L, L);

  // update S2
  Line[S2]->SetPoint1(0, 0, L);
  Line[S2]->SetPoint2(0, L, L);

  // update SQ
  Plane->SetOrigin(0, 0, 0);
  Plane->SetPoint1(0, L, 0);
  Plane->SetPoint2(0, 0, L);

}

//----------------------------------------------------------------------------
void mafGizmoTranslatePlane::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    // mouse down change gizmo selection status
    if (e->GetId() == ID_TRANSFORM)
    {
      if (e->GetArg() == mmiGenericMouse::MOUSE_DOWN)
      {
        this->SetIsActive(true);
      }
    }
    // forward events to the listener
    e->SetSender(this);
    mafEventMacro(*e);
  }
}


/** Gizmo color */
//----------------------------------------------------------------------------
void mafGizmoTranslatePlane::SetColor(int part, double col[3])
//----------------------------------------------------------------------------
{
  if (part == S1 || part == S2 || part == SQ)
  {
    Gizmo[part]->GetMaterial()->m_Prop->SetColor(col);
	  Gizmo[part]->GetMaterial()->m_Prop->SetAmbient(0);
	  Gizmo[part]->GetMaterial()->m_Prop->SetDiffuse(1);
	  Gizmo[part]->GetMaterial()->m_Prop->SetSpecular(0);
  }
}

//----------------------------------------------------------------------------
void mafGizmoTranslatePlane::SetColor(int part, double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(part, col);
}

//----------------------------------------------------------------------------
void mafGizmoTranslatePlane::Show(bool show)
//----------------------------------------------------------------------------
{
  
  if (show == TRUE)
  {  
    Gizmo[S1]->GetMaterial()->m_Prop->SetOpacity(1);
    Gizmo[S2]->GetMaterial()->m_Prop->SetOpacity(1);
    
    if (IsActive == TRUE)
    {
      Gizmo[SQ]->GetMaterial()->m_Prop->SetOpacity(0.5);  
    }
    else
    {
      Gizmo[SQ]->GetMaterial()->m_Prop->SetOpacity(0);  
    }
  }
  else
  {
    for (int i = 0; i < 3; i++)
    {
      Gizmo[i]->GetMaterial()->m_Prop->SetOpacity(0);
    }
  }
}

//----------------------------------------------------------------------------
void mafGizmoTranslatePlane::ShowSquare(bool show)
//----------------------------------------------------------------------------
{
  double opacity = ((show == TRUE) ? 0.5 : 0);
  Gizmo[SQ]->GetMaterial()->m_Prop->SetOpacity(opacity);
}

//----------------------------------------------------------------------------
void mafGizmoTranslatePlane::SetAbsPose(mafMatrix *absPose)
//----------------------------------------------------------------------------
{
  for (int i = 0; i < 3; i++)
  {  
    Gizmo[i]->SetAbsMatrix(*absPose);
  }
  
  SetConstrainRefSys(absPose);
}

//----------------------------------------------------------------------------
void mafGizmoTranslatePlane::SetConstrainRefSys(mafMatrix *constrain)
//----------------------------------------------------------------------------
{  
  for (int i = 0; i < SQ; i++)
  {
    //IsaGen[i]->SetConstrainRefSys(constrain);
    IsaGen[i]->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(constrain);
  }
}

//----------------------------------------------------------------------------
mafMatrix *mafGizmoTranslatePlane::GetAbsPose()
//----------------------------------------------------------------------------
{
  return Gizmo[S1]->GetOutput()->GetAbsMatrix();
}

//----------------------------------------------------------------------------
void mafGizmoTranslatePlane::SetInput(mafVME *vme)
//----------------------------------------------------------------------------
{
  this->InputVme = vme; 
  SetAbsPose(vme->GetOutput()->GetAbsMatrix()); 
  SetConstrainRefSys(vme->GetOutput()->GetAbsMatrix());
}
