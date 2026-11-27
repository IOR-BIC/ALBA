/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoScaleAxis
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


#include "albaGizmoScaleAxis.h"
#include "albaDecl.h"
#include "albaRefSys.h"

// behaviors
#include "albaBehaviorTransform.h"

// vme stuff
#include "mmaMaterial.h"
#include "albaMatrix.h"
#include "albaVME.h"
#include "albaVMEGizmo.h"
#include "albaVMEOutput.h"

// vtk stuff
#include "vtkCubeSource.h"
#include "vtkCylinderSource.h"
#include "vtkAppendPolyData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkProperty.h"

//----------------------------------------------------------------------------
albaGizmoScaleAxis::albaGizmoScaleAxis(albaVME *input, albaObserver *listener)
//----------------------------------------------------------------------------
{
  m_Behavior = NULL;

  m_Listener = listener;
  m_InputVme = input;

  // default axis is X
  m_Axis = X;

  // create pipeline stuff
  CreatePipeline();

  // create the behavior
  CreateBehavior();

  // the gizmo vme shows the cylinder and the cube as a single figure
  m_Gizmo = albaVMEGizmo::New();
  m_Gizmo->SetName("ScaleAxisGizmo");
  m_Gizmo->SetDataConnection(m_RotatePDF->GetOutputPort());
  m_Gizmo->SetMediator(m_Listener);
  m_Gizmo->SetBehavior(m_Behavior);

  m_InputVme->Update();
  SetAbsPose(m_InputVme->GetOutput()->GetAbsMatrix());

  // set the gizmo material property and initial color to red
  this->SetColor(1, 0, 0);

  m_Gizmo->ReparentTo(m_InputVme->GetRoot());

  m_Highlight = false;
  m_Show = false;
}
//----------------------------------------------------------------------------
albaGizmoScaleAxis::~albaGizmoScaleAxis()
//----------------------------------------------------------------------------
{
  m_Gizmo->SetBehavior(NULL);

  vtkDEL(m_Cube);
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
void albaGizmoScaleAxis::CreatePipeline()
//----------------------------------------------------------------------------
{
  // create pipeline for cube-cylinder gizmo along global X axis
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

  // create the cube
  m_Cube = vtkCubeSource::New();

  // create the translation transform
  m_TranslateTr[CUBE] = vtkTransform::New();
  m_TranslateTr[CYLINDER] = vtkTransform::New();

  // create cube translation transform pdf
  m_TranslatePDF[CUBE] = vtkTransformPolyDataFilter::New();
  m_TranslatePDF[CUBE]->SetInputConnection(m_Cube->GetOutputPort());

  // create cylinder translation transform
  m_TranslatePDF[CYLINDER] = vtkTransformPolyDataFilter::New();
  m_TranslatePDF[CYLINDER]->SetInputConnection(cylInitTrPDF->GetOutputPort());

  //-----------------
  // update translate transform
  //-----------------

  // place the cube; default cube length is 1/16 of vme bb diagonal
  this->SetCubeLength(d / 16);

  // place the cylinder before the cube; default cylinder length is 7/16 of vme bb diagonal
  this->SetCylinderLength(d * 7 / 16);

  //-----------------

  // translate transform setting
  m_TranslatePDF[CUBE]->SetTransform(m_TranslateTr[CUBE]);
  m_TranslatePDF[CYLINDER]->SetTransform(m_TranslateTr[CYLINDER]);

/*
  ^           ________
  |          |        |
  |----------|        |
  |          |        |------>X
  |----------|        |
  |          |________|
  O

  <---------><-------->
     cylLen    cubeLen
  */

  // join the cylinder and the cube in a single figure
  m_Append = vtkAppendPolyData::New();
  m_Append->AddInputConnection(m_TranslatePDF[CYLINDER]->GetOutputPort());
  m_Append->AddInputConnection(m_TranslatePDF[CUBE]->GetOutputPort());

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
void albaGizmoScaleAxis::CreateBehavior()
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
void albaGizmoScaleAxis::SetAxis(int axis)
//----------------------------------------------------------------------------
{
  // this should be called when the translation gizmo
  // is created; gizmos are not highlighted

  // register the axis
  m_Axis = axis;

  // rotate the figure to match the given axis and change the axis constraint
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
void albaGizmoScaleAxis::Highlight(bool highlight)
//----------------------------------------------------------------------------
{
  m_Highlight = highlight;

  // the highlighted figure is yellow, otherwise it has the axis color: X red, Y green, Z blue
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
void  albaGizmoScaleAxis::SetCubeLength(double cubeLength)
//----------------------------------------------------------------------------
{
  /*
  ^           ________
  |          |        |
  |----------|        |
  |          |        |------>X
  |----------|        |
  |          |________|
  O

  <---------><-------->
     cylLen    cubeLen
  */

  // set the cube length
  m_Cube->SetXLength(cubeLength);
  m_Cube->SetYLength(cubeLength);
  m_Cube->SetZLength(cubeLength);

  // translate the cube in (cylLen + cubeLen/2)
  m_TranslateTr[CUBE]->Identity();
  m_TranslateTr[CUBE]->Translate(m_Cylinder->GetHeight() + cubeLength / 2, 0, 0);
}

//----------------------------------------------------------------------------
double albaGizmoScaleAxis::GetCubeLength() const
//----------------------------------------------------------------------------
{
  return m_Cube->GetXLength();
}

//----------------------------------------------------------------------------
void albaGizmoScaleAxis::SetCylinderLength(double cylLength)
//----------------------------------------------------------------------------
{
  /*
  ^           ________
  |          |        |
  |----------|        |
  |          |        |------>X
  |----------|        |
  |          |________|
  O

  <---------><-------->
     cylLen    cubeLen
  */

  // set cylLen to cylLength
  m_Cylinder->SetHeight(cylLength);

  // translate the cyl in (cylLen / 2)
  m_TranslateTr[CYLINDER]->Identity();
  m_TranslateTr[CYLINDER]->Translate(cylLength / 2, 0, 0);

  // translate the cube in (cylLen + (cubeLen / 2))
  m_TranslateTr[CUBE]->Identity();
  m_TranslateTr[CUBE]->Translate(cylLength + m_Cube->GetXLength() / 2, 0, 0);
}

//----------------------------------------------------------------------------
double albaGizmoScaleAxis::GetCylinderLength() const
//----------------------------------------------------------------------------
{
  return m_Cylinder->GetHeight();
}

//----------------------------------------------------------------------------
void albaGizmoScaleAxis::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  // forward events to the listener
	alba_event->SetSender(this);
  albaEventMacro(*alba_event);
}

//----------------------------------------------------------------------------
void albaGizmoScaleAxis::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_Gizmo->GetMaterial()->m_Prop->SetColor(col);
  m_Gizmo->GetMaterial()->m_Prop->SetAmbient(0);
  m_Gizmo->GetMaterial()->m_Prop->SetDiffuse(1);
  m_Gizmo->GetMaterial()->m_Prop->SetSpecular(0);
}

//----------------------------------------------------------------------------
void albaGizmoScaleAxis::SetColor(double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(col);
}

//----------------------------------------------------------------------------
void albaGizmoScaleAxis::Show(bool show)
//----------------------------------------------------------------------------
{
  m_Show = show;
  GetLogicManager()->VmeShow(m_Gizmo, show);
}

//----------------------------------------------------------------------------
void albaGizmoScaleAxis::SetAbsPose(albaMatrix *absPose)
//----------------------------------------------------------------------------
{
  m_Gizmo->SetAbsMatrix(*absPose);
  SetRefSysMatrix(absPose);
}

//----------------------------------------------------------------------------
void albaGizmoScaleAxis::SetRefSysMatrix(albaMatrix *matrix)
//----------------------------------------------------------------------------
{
  m_Behavior->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(matrix);
  m_Behavior->GetPivotRefSys()->SetTypeToCustom(matrix);
}

//----------------------------------------------------------------------------
albaMatrix *albaGizmoScaleAxis::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_Gizmo->GetOutput()->GetAbsMatrix();
}

//----------------------------------------------------------------------------
void albaGizmoScaleAxis::SetInput(albaVME *vme)
//----------------------------------------------------------------------------
{
  this->m_InputVme = vme;
  SetAbsPose(vme->GetOutput()->GetAbsMatrix());
  SetRefSysMatrix(vme->GetOutput()->GetAbsMatrix());
}

int albaGizmoScaleAxis::GetAxis() const
{
  return m_Axis;
}
