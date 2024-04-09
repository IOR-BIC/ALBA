/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoTranslateAxis
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


#include "albaGizmoTranslateAxis.h"
#include "albaDecl.h"
#include "albaRefSys.h"

// isa stuff
#include "albaInteractorCompositorMouse.h"
#include "albaInteractorGenericMouse.h"

// vme stuff
#include "mmaMaterial.h"
#include "albaMatrix.h"
#include "albaVME.h"
#include "albaVMEOutput.h"
#include "albaVMEGizmo.h"

// vtk stuff
#include "vtkConeSource.h"
#include "vtkCylinderSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkProperty.h"

//----------------------------------------------------------------------------
albaGizmoTranslateAxis::albaGizmoTranslateAxis(albaVME *input, albaObserver *listener, albaString name)
//----------------------------------------------------------------------------
{
  m_ConeLength = 0.5;
  m_CylinderLength = 0.5;

  m_IsaComp[0] = m_IsaComp[1] = NULL;

  m_Listener = listener;
  m_InputVme = input;
  
  // default axis is X
  m_Axis = X;
  
  // create pipeline stuff
  CreatePipeline();

  // create isa stuff
  CreateISA();

  //-----------------
  // create vme gizmos stuff
  //-----------------
  // cylinder gizmo
  m_CylGizmo = albaVMEGizmo::New();
  if(name.Equals("")) {
      m_CylGizmo->SetName("CylGizmo");
  } else {
	  m_CylGizmo->SetName(name);
  }
  m_CylGizmo->SetData(m_RotatePDF[CYLINDER]->GetOutput());
  m_CylGizmo->SetMediator(m_Listener);

  // cone gizmo
  m_ConeGizmo = albaVMEGizmo::New();
  if(name.Equals("")) {
      m_ConeGizmo->SetName("ConeGizmo");
  } else {
     m_ConeGizmo->SetName(name);
  }
  m_ConeGizmo->SetData(m_RotatePDF[CONE]->GetOutput());
  m_ConeGizmo->SetMediator(m_Listener);

  // assign isa to cylinder and cone
  m_CylGizmo->SetBehavior(m_IsaComp[CYLINDER]);
  m_ConeGizmo->SetBehavior(m_IsaComp[CONE]);

  SetAbsPose(m_InputVme->GetOutput()->GetAbsMatrix());
  
  // set come gizmo material property and initial color to red
  this->SetColor(1, 0, 0, 1, 0, 0);

  //-----------------
  // ReparentTo will add also the gizmos to the tree!!
  // add the gizmo to the tree, this should increase reference count 
  m_CylGizmo->ReparentTo(m_InputVme->GetRoot());
  m_ConeGizmo->ReparentTo(m_InputVme->GetRoot());
}
//----------------------------------------------------------------------------
albaGizmoTranslateAxis::~albaGizmoTranslateAxis() 
//----------------------------------------------------------------------------
{
  m_CylGizmo->SetBehavior(NULL);
  m_ConeGizmo->SetBehavior(NULL);
   
  vtkDEL(m_Cone);
  vtkDEL(m_Cylinder);
  
  // clean up
  for (int i = 0; i < 2; i++)
  {
    vtkDEL(m_TranslateTr[i]);
    vtkDEL(m_TranslatePDF[i]);
    vtkDEL(m_RotationTr);
    vtkDEL(m_RotatePDF[i]);
	//----------------------
	// No leaks so somebody is performing this...
	// wxDEL(GizmoData[i]);
	//----------------------
    vtkDEL(m_IsaComp[i]); 
  }
	m_CylGizmo->ReparentTo(NULL);
	m_ConeGizmo->ReparentTo(NULL);
}

//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::CreatePipeline() 
//----------------------------------------------------------------------------
{
  // create pipeline for cone-cylinder gizmo along global X axis
  // calculate diagonal of m_InputVme space bounds 
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
  
  // create the cylinder
  m_Cylinder = vtkCylinderSource::New();
  m_Cylinder->SetRadius(d / 200);
  
  //-----------------
  // rotate the cylinder on the X axis (default axis is Z)
  //-----------------
  vtkTransform *cylInitTr = vtkTransform::New();
  cylInitTr->RotateZ(-90);	
  
  vtkTransformPolyDataFilter *cylInitTrPDF = vtkTransformPolyDataFilter::New();
  cylInitTrPDF->SetInputConnection(m_Cylinder->GetOutputPort());
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
  m_Cone = vtkConeSource::New();
  m_Cone->SetRadius(d / 40);
  m_Cone->SetResolution(20);

  // create the translation transform
  m_TranslateTr[CONE] = vtkTransform::New();
  m_TranslateTr[CYLINDER] = vtkTransform::New();

  // create cone translation transform pdf
  m_TranslatePDF[CONE] = vtkTransformPolyDataFilter::New();
  m_TranslatePDF[CONE]->SetInputConnection(m_Cone->GetOutputPort());
  
  // create cylinder translation transform
  m_TranslatePDF[CYLINDER] = vtkTransformPolyDataFilter::New();
  m_TranslatePDF[CYLINDER]->SetInputConnection(cylInitTrPDF->GetOutputPort());

  //-----------------
  // update translate transform
  //-----------------
  // place the cone; default cone length is 1/4 of vme bb diagonal
  this->SetConeLength(d / 4);

  // place the cylinder before the cone; default cylinder length is 1/4 of vme bb diagonal
  this->SetCylinderLength(d / 4);

  //-----------------
  // translate transform setting
  m_TranslatePDF[CONE]->SetTransform(m_TranslateTr[CONE]);
  m_TranslatePDF[CYLINDER]->SetTransform(m_TranslateTr[CYLINDER]);

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
  m_RotatePDF[CYLINDER] = vtkTransformPolyDataFilter::New();
  m_RotatePDF[CONE] = vtkTransformPolyDataFilter::New();
  m_RotationTr = vtkTransform::New();
  m_RotationTr->Identity(); 

  m_RotatePDF[CYLINDER]->SetTransform(m_RotationTr);
  m_RotatePDF[CONE]->SetTransform(m_RotationTr);

  m_RotatePDF[CYLINDER]->SetInputConnection(m_TranslatePDF[CYLINDER]->GetOutputPort());
  m_RotatePDF[CONE]->SetInputConnection(m_TranslatePDF[CONE]->GetOutputPort());

  m_RotatePDF[CYLINDER]->Update();
  m_RotatePDF[CYLINDER]->Update();

  //clean up
  cylInitTr->Delete();
  cylInitTrPDF->Delete();
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::CreateISA()
//----------------------------------------------------------------------------
{
  // create isa compositor and assign behaviors to IsaGen ivar
  for (int i = 0; i < 2; i++)
  {
    m_IsaComp[i] = albaInteractorCompositorMouse::New();

    // default behavior is activated by mouse left and is constrained to X axis,
    // default ref sys is input vme abs matrix
    m_IsaGen[i] = m_IsaComp[i]->CreateBehavior(MOUSE_LEFT);

    m_IsaGen[i]->SetVME(m_InputVme);
    m_IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK);
  	m_IsaGen[i]->GetTranslationConstraint()->GetRefSys()->SetTypeToLocal();
      
    //isa will send events to this
    m_IsaGen[i]->SetListener(this);
  }
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::SetAxis(int axis) 
//----------------------------------------------------------------------------
{
  // this should be called when the translation gizmo
  // is created; gizmos are not highlighted
  // register the axis
  m_Axis = axis;
  
  // rotate the cylinder and the cone to match given axis
  if (m_Axis == X)
  {
    // reset cyl and cone rotation
    m_RotationTr->Identity();
  
    // set cyl and cone color to red
    this->SetColor(1, 0, 0, 1, 0, 0);

    // change the axis constrain
    for (int i = 0; i < 2; i++)
    {
      m_IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK);
    }
  }
  else if (axis == Y)
  {
    // set rotation to move con and cyl on Y 
    m_RotationTr->Identity();
    m_RotationTr->RotateZ(90);
   
    // set cyl and cone color to green
    this->SetColor(0, 1, 0, 0, 1, 0);

    // change the axis constrain
    for (int i = 0; i < 2; i++)
    {
      m_IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
    }
  }  
  else if (axis == Z)
  {
    // set rotation to move con and cyl on Z
    m_RotationTr->Identity();
    m_RotationTr->RotateY(-90);
    
    // set cyl and cone color to blue
     this->SetColor(0, 0, 1, 0, 0, 1);

     // change the axis constrain
    for (int i = 0; i < 2; i++)
    {
      m_IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE);
    }
  }  
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::Highlight(bool highlight)
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
   if (m_Axis == X)
   {
     // set cyl col to red
     this->SetColor(CYLINDER, 1, 0, 0);
   } 
   else if (m_Axis == Y)
   {
     // set cyl col to green
     this->SetColor(CYLINDER, 0, 1, 0);
   }
   else if (m_Axis == Z)
   {
     // set cyl col to blue
     this->SetColor(CYLINDER, 0, 0, 1);
   } 
  }
}
//----------------------------------------------------------------------------
void  albaGizmoTranslateAxis::SetConeLength(double length)
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
  m_ConeLength = length;
  m_Cone->SetHeight(length);

  // translate the cone in (cylLen + conLen/2)
  m_TranslateTr[CONE]->Identity();
  m_TranslateTr[CONE]->Translate(m_Cylinder->GetHeight() + length / 2, 0, 0);
  
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::SetCylinderLength(double length)
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
  m_CylinderLength = length;
  m_Cylinder->SetHeight(length);

  // translate the cyl in (cylLen / 2)
  m_TranslateTr[CYLINDER]->Identity();
  m_TranslateTr[CYLINDER]->Translate(length / 2, 0, 0);

  // translate the cone in (cylLen + (conLen / 2)) 
  m_TranslateTr[CONE]->Identity();
  m_TranslateTr[CONE]->Translate(length + m_Cone->GetHeight() / 2, 0, 0);

}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  // forward events to the listener
	alba_event->SetSender(this);
  albaEventMacro(*alba_event);
}
/** Gizmo color */
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::SetColor(int part, double col[3])
//----------------------------------------------------------------------------
{
  if (part == CONE)
  {
    m_ConeGizmo->GetMaterial()->m_Prop->SetColor(col);
    m_ConeGizmo->GetMaterial()->m_Prop->SetAmbient(0);
    m_ConeGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
    m_ConeGizmo->GetMaterial()->m_Prop->SetSpecular(0);
  }
  else
  {
    m_CylGizmo->GetMaterial()->m_Prop->SetColor(col);
    m_CylGizmo->GetMaterial()->m_Prop->SetAmbient(0);
    m_CylGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
    m_CylGizmo->GetMaterial()->m_Prop->SetSpecular(0);
  }
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::SetColor(int part, double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(part, col);
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::SetColor(double cylCol[3], double coneCol[3])
//----------------------------------------------------------------------------
{
  this->SetColor(CYLINDER, cylCol);
  this->SetColor(CONE, coneCol);
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::SetColor(double cylR, double cylG, double cylB, double coneR, double coneG, double coneB)
//----------------------------------------------------------------------------
{
  this->SetColor(CYLINDER, cylR, cylG, cylB);
  this->SetColor(CONE, coneR, coneG, coneB);
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::Show(bool show)
//----------------------------------------------------------------------------
{
  GetLogicManager()->VmeShow(m_CylGizmo, show);
	GetLogicManager()->VmeShow(m_ConeGizmo, show);
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::SetAbsPose(albaMatrix *absPose)
//----------------------------------------------------------------------------
{
  m_ConeGizmo->SetAbsMatrix(*absPose); 
  m_CylGizmo->SetAbsMatrix(*absPose); 
  SetRefSysMatrix(absPose);
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::SetRefSysMatrix(albaMatrix *matrix)
//----------------------------------------------------------------------------
{  
  for (int i = 0; i < 2; i++)
  {
    m_IsaGen[i]->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(matrix);
    m_IsaGen[i]->GetPivotRefSys()->SetTypeToCustom(matrix);
  } 
}
//----------------------------------------------------------------------------
albaMatrix *albaGizmoTranslateAxis::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_CylGizmo->GetOutput()->GetAbsMatrix();
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::SetInput(albaVME *vme)
//----------------------------------------------------------------------------
{
  this->m_InputVme = vme; 
  SetAbsPose(vme->GetOutput()->GetAbsMatrix()); 
  SetRefSysMatrix(vme->GetOutput()->GetAbsMatrix());
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::SetConstraintModality(int axis, int constrainModality)
//----------------------------------------------------------------------------
{
  for (int i = 0; i < 2; i++)
  {
    m_IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(axis,constrainModality);
  } 
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::SetStep(int axis, double step)
//----------------------------------------------------------------------------
{
  for (int i = 0; i < 2; i++)
  {
    m_IsaGen[i]->GetTranslationConstraint()->SetStep(axis,step);
  } 
}
