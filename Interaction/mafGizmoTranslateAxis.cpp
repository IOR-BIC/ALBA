/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoTranslateAxis.cpp,v $
  Language:  C++
  Date:      $Date: 2007-05-14 09:22:17 $
  Version:   $Revision: 1.9 $
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


#include "mafGizmoTranslateAxis.h"
#include "mafDecl.h"

// isa stuff
#include "mmiCompositorMouse.h"
#include "mmiGenericMouse.h"

// vme stuff
#include "mmaMaterial.h"
#include "mafMatrix.h"
#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafVMEGizmo.h"

// vtk stuff
#include "vtkConeSource.h"
#include "vtkCylinderSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkProperty.h"

//----------------------------------------------------------------------------
mafGizmoTranslateAxis::mafGizmoTranslateAxis(mafVME *input, mafObserver *listener)
//----------------------------------------------------------------------------
{
  IsaComp[0] = IsaComp[1] = NULL;

  m_Listener = listener;
  InputVme = input;
  
  // default axis is X
  Axis = X;
  
  // create pipeline stuff
  CreatePipeline();

  // create isa stuff
  CreateISA();

  //-----------------
  // create vme gizmos stuff
  //-----------------
  // cylinder gizmo
  CylGizmo = mafVMEGizmo::New();
  CylGizmo->SetName("CylGizmo");
  CylGizmo->SetData(RotatePDF[CYLINDER]->GetOutput());
  
  // cone gizmo
  ConeGizmo = mafVMEGizmo::New();  
  ConeGizmo->SetName("ConeGizmo");
  ConeGizmo->SetData(RotatePDF[CONE]->GetOutput());

  // assign isa to cylinder and cone
  CylGizmo->SetBehavior(IsaComp[CYLINDER]);
  ConeGizmo->SetBehavior(IsaComp[CONE]);

  SetAbsPose(InputVme->GetOutput()->GetAbsMatrix());
  
  // set come gizmo material property and initial color to red
  this->SetColor(1, 0, 0, 1, 0, 0);

  //-----------------
  // ReparentTo will add also the gizmos to the tree!!
  // add the gizmo to the tree, this should increase reference count 
  CylGizmo->ReparentTo(mafVME::SafeDownCast(InputVme->GetRoot()));
  ConeGizmo->ReparentTo(mafVME::SafeDownCast(InputVme->GetRoot()));
}
//----------------------------------------------------------------------------
mafGizmoTranslateAxis::~mafGizmoTranslateAxis() 
//----------------------------------------------------------------------------
{
  CylGizmo->SetBehavior(NULL);
  ConeGizmo->SetBehavior(NULL);
   
  vtkDEL(Cone);
  vtkDEL(Cylinder);
  
  // clean up
  for (int i = 0; i < 2; i++)
  {
    vtkDEL(TranslateTr[i]);
    vtkDEL(TranslatePDF[i]);
    vtkDEL(RotationTr);
    vtkDEL(RotatePDF[i]);
	//----------------------
	// No leaks so somebody is performing this...
	// wxDEL(GizmoData[i]);
	//----------------------
    vtkDEL(IsaComp[i]); 
  }
	CylGizmo->ReparentTo(NULL);
	ConeGizmo->ReparentTo(NULL);
}

//----------------------------------------------------------------------------
void mafGizmoTranslateAxis::CreatePipeline() 
//----------------------------------------------------------------------------
{
  // create pipeline for cone-cylinder gizmo along global X axis
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
  
  // create the cylinder
  Cylinder = vtkCylinderSource::New();
  Cylinder->SetRadius(d / 200);
  
  //-----------------
  // rotate the cylinder on the X axis (default axis is Z)
  //-----------------
  vtkTransform *cylInitTr = vtkTransform::New();
  cylInitTr->RotateZ(-90);	
  
  vtkTransformPolyDataFilter *cylInitTrPDF = vtkTransformPolyDataFilter::New();
  cylInitTrPDF->SetInput(Cylinder->GetOutput());
  cylInitTrPDF->SetTransform(cylInitTr);

  /*
   vtk coord
      y
      ^
      |
      __
     || |
     |z-|-------> x
     |  |
      --
   */
  
  //-----------------
  // create the cone
  Cone = vtkConeSource::New();
  Cone->SetRadius(d / 40);
  Cone->SetResolution(20);

  // create the translation transform
  TranslateTr[CONE] = vtkTransform::New();
  TranslateTr[CYLINDER] = vtkTransform::New();

  // create cone translation transform pdf
  TranslatePDF[CONE] = vtkTransformPolyDataFilter::New();
  TranslatePDF[CONE]->SetInput(Cone->GetOutput());
  
  // create cylinder translation transform
  TranslatePDF[CYLINDER] = vtkTransformPolyDataFilter::New();
  TranslatePDF[CYLINDER]->SetInput(cylInitTrPDF->GetOutput());

  //-----------------
  // update translate transform
  //-----------------
  // place the cone; default cone length is 1/4 of vme bb diagonal
  this->SetConeLength(d / 4);

  // place the cylinder before the cone; default cylinder length is 1/4 of vme bb diagonal
  this->SetCylinderLength(d / 4);

  //-----------------
  // translate transform setting
  TranslatePDF[CONE]->SetTransform(TranslateTr[CONE]);
  TranslatePDF[CYLINDER]->SetTransform(TranslateTr[CYLINDER]);

  /*
  ^          * 
  |          |***
  |----------|   ***
  |          |      *** ------>X
  |----------|   ***
  |          |***
  O          *
  
  <---------><-------->
     cylLen    conLen
  */    

  // create rotation transform and rotation TPDF 
  RotatePDF[CYLINDER] = vtkTransformPolyDataFilter::New();
  RotatePDF[CONE] = vtkTransformPolyDataFilter::New();
  RotationTr = vtkTransform::New();
  RotationTr->Identity(); 

  RotatePDF[CYLINDER]->SetTransform(RotationTr);
  RotatePDF[CONE]->SetTransform(RotationTr);

  RotatePDF[CYLINDER]->SetInput(TranslatePDF[CYLINDER]->GetOutput());
  RotatePDF[CONE]->SetInput(TranslatePDF[CONE]->GetOutput());

  RotatePDF[CYLINDER]->Update();
  RotatePDF[CYLINDER]->Update();

  //clean up
  cylInitTr->Delete();
  cylInitTrPDF->Delete();
}
//----------------------------------------------------------------------------
void mafGizmoTranslateAxis::CreateISA()
//----------------------------------------------------------------------------
{
  // create isa compositor and assign behaviors to IsaGen ivar
  for (int i = 0; i < 2; i++)
  {
    IsaComp[i] = mmiCompositorMouse::New();

    // default behavior is activated by mouse left and is constrained to X axis,
    // default ref sys is input vme abs matrix
    IsaGen[i] = IsaComp[i]->CreateBehavior(MOUSE_LEFT);

    IsaGen[i]->SetVME(InputVme);
    IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::LOCK, mmiConstraint::LOCK);
  	IsaGen[i]->GetTranslationConstraint()->GetRefSys()->SetTypeToLocal();
      
    //isa will send events to this
    IsaGen[i]->SetListener(this);
  }
}
//----------------------------------------------------------------------------
void mafGizmoTranslateAxis::SetAxis(int axis) 
//----------------------------------------------------------------------------
{
  // this should be called when the translation gizmo
  // is created; gizmos are not highlighted
  // register the axis
  Axis = axis;
  
  // rotate the cylinder and the cone to match given axis
  if (Axis == X)
  {
    // reset cyl and cone rotation
    RotationTr->Identity();
  
    // set cyl and cone color to red
    this->SetColor(1, 0, 0, 1, 0, 0);

    // change the axis constrain
    for (int i = 0; i < 2; i++)
    {
      IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::LOCK, mmiConstraint::LOCK);
    }
  }
  else if (axis == Y)
  {
    // set rotation to move con and cyl on Y 
    RotationTr->Identity();
    RotationTr->RotateZ(90);
   
    // set cyl and cone color to green
    this->SetColor(0, 1, 0, 0, 1, 0);

    // change the axis constrain
    for (int i = 0; i < 2; i++)
    {
      IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::FREE, mmiConstraint::LOCK);
    }
  }  
  else if (axis == Z)
  {
    // set rotation to move con and cyl on Z
    RotationTr->Identity();
    RotationTr->RotateY(-90);
    
    // set cyl and cone color to blue
     this->SetColor(0, 0, 1, 0, 0, 1);

     // change the axis constrain
    for (int i = 0; i < 2; i++)
    {
      IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::LOCK, mmiConstraint::FREE);
    }
  }  
}
//----------------------------------------------------------------------------
void mafGizmoTranslateAxis::Highlight(bool highlight)
//----------------------------------------------------------------------------
{
  if (highlight == true)
  {
   // Highlight the cylinder by setting its color to yellow 
   this->SetColor(CYLINDER, 1, 1, 0);
  } 
  else
  {
   // deactivate the cylinder 
   if (Axis == X)
   {
     // set cyl col to red
     this->SetColor(CYLINDER, 1, 0, 0);
   } 
   else if (Axis == Y)
   {
     // set cyl col to green
     this->SetColor(CYLINDER, 0, 1, 0);
   }
   else if (Axis == Z)
   {
     // set cyl col to blue
     this->SetColor(CYLINDER, 0, 0, 1);
   } 
  }
}
//----------------------------------------------------------------------------
void  mafGizmoTranslateAxis::SetConeLength(double length)
//----------------------------------------------------------------------------
{
  /*
  ^          * 
  |          |***
  |----------|   ***
  |          |      *** ------>X
  |----------|   ***
  |          |***
  O          *
  
  <---------><-------->
     cylLen    conLen
  */  
  // set the cone length
  Cone->SetHeight(length);

  // translate the cone in (cylLen + conLen/2)
  TranslateTr[CONE]->Identity();
  TranslateTr[CONE]->Translate(Cylinder->GetHeight() + length / 2, 0, 0);
}
//----------------------------------------------------------------------------
void mafGizmoTranslateAxis::SetCylinderLength(double length)
//----------------------------------------------------------------------------
{
  /*
  ^          * 
  |          |***
  |----------|   ***
  |          |      *** ------>X
  |----------|   ***
  |          |***
  O          *
  
  <---------><-------->
     cylLen    conLen
  */  

  // set cylLen to length
  Cylinder->SetHeight(length);

  // translate the cyl in (cylLen / 2)
  TranslateTr[CYLINDER]->Identity();
  TranslateTr[CYLINDER]->Translate(length / 2, 0, 0);

  // translate the cone in (cylLen + (conLen / 2)) 
  TranslateTr[CONE]->Identity();
  TranslateTr[CONE]->Translate(length + Cone->GetHeight() / 2, 0, 0);

}
//----------------------------------------------------------------------------
void mafGizmoTranslateAxis::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // forward events to the listener
	maf_event->SetSender(this);
  mafEventMacro(*maf_event);
}
/** Gizmo color */
//----------------------------------------------------------------------------
void mafGizmoTranslateAxis::SetColor(int part, double col[3])
//----------------------------------------------------------------------------
{
  if (part == CONE)
  {
    ConeGizmo->GetMaterial()->m_Prop->SetColor(col);
    ConeGizmo->GetMaterial()->m_Prop->SetAmbient(0);
    ConeGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
    ConeGizmo->GetMaterial()->m_Prop->SetSpecular(0);
  }
  else
  {
    CylGizmo->GetMaterial()->m_Prop->SetColor(col);
    CylGizmo->GetMaterial()->m_Prop->SetAmbient(0);
    CylGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
    CylGizmo->GetMaterial()->m_Prop->SetSpecular(0);
  }
}
//----------------------------------------------------------------------------
void mafGizmoTranslateAxis::SetColor(int part, double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(part, col);
}
//----------------------------------------------------------------------------
void mafGizmoTranslateAxis::SetColor(double cylCol[3], double coneCol[3])
//----------------------------------------------------------------------------
{
  this->SetColor(CYLINDER, cylCol);
  this->SetColor(CONE, coneCol);
}
//----------------------------------------------------------------------------
void mafGizmoTranslateAxis::SetColor(double cylR, double cylG, double cylB, double coneR, double coneG, double coneB)
//----------------------------------------------------------------------------
{
  this->SetColor(CYLINDER, cylR, cylG, cylB);
  this->SetColor(CONE, coneR, coneG, coneB);
}
//----------------------------------------------------------------------------
void mafGizmoTranslateAxis::Show(bool show)
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,VME_SHOW,CylGizmo,show));
	mafEventMacro(mafEvent(this,VME_SHOW,ConeGizmo,show));
}
//----------------------------------------------------------------------------
void mafGizmoTranslateAxis::SetAbsPose(mafMatrix *absPose)
//----------------------------------------------------------------------------
{
  ConeGizmo->SetAbsMatrix(*absPose); 
  CylGizmo->SetAbsMatrix(*absPose); 
  SetRefSysMatrix(absPose);
}
//----------------------------------------------------------------------------
void mafGizmoTranslateAxis::SetRefSysMatrix(mafMatrix *matrix)
//----------------------------------------------------------------------------
{  
  for (int i = 0; i < 2; i++)
  {
    IsaGen[i]->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(matrix);
    IsaGen[i]->GetPivotRefSys()->SetTypeToCustom(matrix);
  } 
}
//----------------------------------------------------------------------------
mafMatrix *mafGizmoTranslateAxis::GetAbsPose()
//----------------------------------------------------------------------------
{
  return CylGizmo->GetOutput()->GetAbsMatrix();
}
//----------------------------------------------------------------------------
void mafGizmoTranslateAxis::SetInput(mafVME *vme)
//----------------------------------------------------------------------------
{
  this->InputVme = vme; 
  SetAbsPose(vme->GetOutput()->GetAbsMatrix()); 
  SetRefSysMatrix(vme->GetOutput()->GetAbsMatrix());
}
//----------------------------------------------------------------------------
void mafGizmoTranslateAxis::SetConstraintModality(int axis, int constrainModality)
//----------------------------------------------------------------------------
{
  for (int i = 0; i < 2; i++)
  {
    IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(axis,constrainModality);
  } 
}
//----------------------------------------------------------------------------
void mafGizmoTranslateAxis::SetStep(int axis, double step)
//----------------------------------------------------------------------------
{
  for (int i = 0; i < 2; i++)
  {
    IsaGen[i]->GetTranslationConstraint()->SetStep(axis,step);
  } 
}
