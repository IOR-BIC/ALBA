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

// behaviors
#include "albaBehaviorTransform.h"

// vme stuff
#include "mmaMaterial.h"
#include "albaMatrix.h"
#include "albaVME.h"
#include "albaVMEOutput.h"
#include "albaVMEGizmo.h"

// vtk stuff
#include "vtkConeSource.h"
#include "vtkCylinderSource.h"
#include "vtkAppendPolyData.h"
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

  m_Behavior = NULL;

  m_Listener = listener;
  m_InputVme = input;

  // default axis is X
  m_Axis = X;

  // create pipeline stuff
  CreatePipeline();

  // create the behavior
  CreateBehavior();

  // the gizmo vme shows the cylinder and the cone as a single arrow
  m_Gizmo = albaVMEGizmo::New();
  m_Gizmo->SetName(name.Equals("") ? "TranslateAxisGizmo" : name.GetCStr());
  m_Gizmo->SetDataConnection(m_RotatePDF->GetOutputPort());
  m_Gizmo->SetMediator(m_Listener);
  m_Gizmo->SetBehavior(m_Behavior);

  SetAbsPose(m_InputVme->GetOutput()->GetAbsMatrix());

  // set the gizmo material property and initial color to red
  this->SetColor(1, 0, 0);

  // ReparentTo will add also the gizmo to the tree
  m_Gizmo->ReparentTo(m_InputVme->GetRoot());
}
//----------------------------------------------------------------------------
albaGizmoTranslateAxis::~albaGizmoTranslateAxis()
//----------------------------------------------------------------------------
{
  m_Gizmo->SetBehavior(NULL);

  vtkDEL(m_Cone);
  vtkDEL(m_Cylinder);

  // clean up
  for (int i = 0; i < 2; i++)
  {
    vtkDEL(m_TranslateTr[i]);
    vtkDEL(m_TranslatePDF[i]);
  }
  vtkDEL(m_Append);
  vtkDEL(m_RotationTr);
  vtkDEL(m_RotatePDF);

  // the PER can keep the behavior alive: it must not notify the deleted gizmo
  m_Behavior->SetListener(NULL);
  vtkDEL(m_Behavior);

  m_Gizmo->ReparentTo(NULL);
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

  // join the cylinder and the cone in a single arrow
  m_Append = vtkAppendPolyData::New();
  m_Append->AddInputConnection(m_TranslatePDF[CYLINDER]->GetOutputPort());
  m_Append->AddInputConnection(m_TranslatePDF[CONE]->GetOutputPort());

  // create rotation transform and rotation TPDF
  m_RotationTr = vtkTransform::New();
  m_RotationTr->Identity();

  m_RotatePDF = vtkTransformPolyDataFilter::New();
  m_RotatePDF->SetTransform(m_RotationTr);
  m_RotatePDF->SetInputConnection(m_Append->GetOutputPort());
  m_RotatePDF->Update();

  //clean up
  cylInitTr->Delete();
  cylInitTrPDF->Delete();
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::CreateBehavior()
//----------------------------------------------------------------------------
{
  // default behavior is activated by mouse left and is constrained to X axis,
  // default ref sys is input vme abs matrix
  albaNEW(m_Behavior);

  m_Behavior->SetVME(m_InputVme);
  m_Behavior->GetTranslationConstraint()->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::LOCK, albaTransformConstraint::LOCK);
  m_Behavior->GetTranslationConstraint()->GetRefSys()->SetTypeToLocal();

  // the behavior will send events to this
  m_Behavior->SetListener(this);
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::SetAxis(int axis)
//----------------------------------------------------------------------------
{
  // this should be called when the translation gizmo
  // is created; gizmos are not highlighted
  // register the axis
  m_Axis = axis;

  // rotate the arrow to match the given axis and change the axis constraint
  m_RotationTr->Identity();
  if (m_Axis == X)
    m_Behavior->GetTranslationConstraint()->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::LOCK, albaTransformConstraint::LOCK);
  else if (m_Axis == Y)
  {
    m_RotationTr->RotateZ(90);
    m_Behavior->GetTranslationConstraint()->SetConstraintModality(albaTransformConstraint::LOCK, albaTransformConstraint::FREE, albaTransformConstraint::LOCK);
  }
  else if (m_Axis == Z)
  {
    m_RotationTr->RotateY(-90);
    m_Behavior->GetTranslationConstraint()->SetConstraintModality(albaTransformConstraint::LOCK, albaTransformConstraint::LOCK, albaTransformConstraint::FREE);
  }

  // set the axis color
  Highlight(false);
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::Highlight(bool highlight)
//----------------------------------------------------------------------------
{
  // the highlighted arrow is yellow, otherwise it has the axis color: X red, Y green, Z blue
  if (highlight)
    this->SetColor(1, 1, 0);
  else if (m_Axis == X)
    this->SetColor(1, 0, 0);
  else if (m_Axis == Y)
    this->SetColor(0, 1, 0);
  else if (m_Axis == Z)
    this->SetColor(0, 0, 1);
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
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_Gizmo->GetMaterial()->m_Prop->SetColor(col);
  m_Gizmo->GetMaterial()->m_Prop->SetAmbient(0);
  m_Gizmo->GetMaterial()->m_Prop->SetDiffuse(1);
  m_Gizmo->GetMaterial()->m_Prop->SetSpecular(0);
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::SetColor(double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(col);
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::Show(bool show)
//----------------------------------------------------------------------------
{
  GetLogicManager()->VmeShow(m_Gizmo, show);
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::SetAbsPose(albaMatrix *absPose)
//----------------------------------------------------------------------------
{
  m_Gizmo->SetAbsMatrix(*absPose);
  SetRefSysMatrix(absPose);
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::SetRefSysMatrix(albaMatrix *matrix)
//----------------------------------------------------------------------------
{
  m_Behavior->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(matrix);
  m_Behavior->GetPivotRefSys()->SetTypeToCustom(matrix);
}
//----------------------------------------------------------------------------
albaMatrix *albaGizmoTranslateAxis::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_Gizmo->GetOutput()->GetAbsMatrix();
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
  m_Behavior->GetTranslationConstraint()->SetConstraintModality(axis,constrainModality);
}
//----------------------------------------------------------------------------
void albaGizmoTranslateAxis::SetStep(int axis, double step)
//----------------------------------------------------------------------------
{
  m_Behavior->GetTranslationConstraint()->SetStep(axis,step);
}
