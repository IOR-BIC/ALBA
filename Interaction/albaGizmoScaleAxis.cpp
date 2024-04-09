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

// isa stuff
#include "albaInteractorCompositorMouse.h"
#include "albaInteractorGenericMouse.h"

// vme stuff
#include "mmaMaterial.h"
#include "albaMatrix.h"
#include "albaVME.h"
#include "albaVMEGizmo.h"
#include "albaVMEOutput.h"

// vtk stuff
#include "vtkCubeSource.h"
#include "vtkCylinderSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkProperty.h"

//----------------------------------------------------------------------------
albaGizmoScaleAxis::albaGizmoScaleAxis(albaVME *input, albaObserver *listener)
//----------------------------------------------------------------------------
{
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
  m_CylGizmo->SetName("CylGizmo");
  m_CylGizmo->SetData(m_RotatePDF[CYLINDER]->GetOutput());
  m_CylGizmo->SetMediator(m_Listener);

  // cube gizmo
  m_CubeGizmo = albaVMEGizmo::New();  
  m_CubeGizmo->SetName("CubeGizmo");
  m_CubeGizmo->SetData(m_RotatePDF[CUBE]->GetOutput());
  m_CubeGizmo->SetMediator(m_Listener);

  // assign isa to cylinder and cube
  m_CylGizmo->SetBehavior(m_IsaComp[0]);
  m_CubeGizmo->SetBehavior(m_IsaComp[1]);

  m_InputVme->Update();
  SetAbsPose(m_InputVme->GetOutput()->GetAbsMatrix());
  
  // set come gizmo material property and initial color to red
  this->SetColor(1, 0, 0, 1, 0, 0);

  m_CylGizmo->ReparentTo(m_InputVme->GetRoot());
  m_CubeGizmo->ReparentTo(m_InputVme->GetRoot());

  m_Highlight = false;
  m_Show = false;
}
//----------------------------------------------------------------------------
albaGizmoScaleAxis::~albaGizmoScaleAxis() 
//----------------------------------------------------------------------------
{
  m_CylGizmo->SetBehavior(NULL);
  m_CubeGizmo->SetBehavior(NULL);
   
  vtkDEL(m_Cube);
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
	m_CubeGizmo->ReparentTo(NULL);
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
  //Cube->SetRadius(m_InputVme->GetCurrentData()->GetLength() / 30);

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

  // create rotation transform and rotation TPDF 
  m_RotatePDF[CYLINDER] = vtkTransformPolyDataFilter::New();
  m_RotatePDF[CUBE] = vtkTransformPolyDataFilter::New();
  m_RotationTr = vtkTransform::New();
  m_RotationTr->Identity(); 

  m_RotatePDF[CYLINDER]->SetTransform(m_RotationTr);
  m_RotatePDF[CUBE]->SetTransform(m_RotationTr);

  m_RotatePDF[CYLINDER]->SetInputConnection(m_TranslatePDF[CYLINDER]->GetOutputPort());
  m_RotatePDF[CUBE]->SetInputConnection(m_TranslatePDF[CUBE]->GetOutputPort());

  m_RotatePDF[CYLINDER]->Update();
  m_RotatePDF[CYLINDER]->Update();

  //clean up
  cylInitTr->Delete();
  cylInitTrPDF->Delete();
}

//----------------------------------------------------------------------------
void albaGizmoScaleAxis::CreateISA()
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
void albaGizmoScaleAxis::SetAxis(int axis) 
//----------------------------------------------------------------------------
{
  // this should be called when the translation gizmo
  // is created; gizmos are not highlighted
  
  // register the axis
  m_Axis = axis;
  
  // rotate the cylinder and the cube to match given axis
  if (m_Axis == X)
  {
    // reset cyl and cube rotation
    m_RotationTr->Identity();
  
    // set cyl and cube color to red
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
   
    // set cyl and cube color to green
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
    
    // set cyl and cube color to blue
     this->SetColor(0, 0, 1, 0, 0, 1);

     // change the axis constrain
    for (int i = 0; i < 2; i++)
    {
      m_IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE);
    }
  }  
}

//----------------------------------------------------------------------------
void albaGizmoScaleAxis::Highlight(bool highlight)
//----------------------------------------------------------------------------
{
  m_Highlight = highlight;

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

/** Gizmo color */
//----------------------------------------------------------------------------
void albaGizmoScaleAxis::SetColor(int part, double col[3])
//----------------------------------------------------------------------------
{
  if (part == CYLINDER)
  {
    m_CylGizmo->GetMaterial()->m_Prop->SetColor(col);
    m_CylGizmo->GetMaterial()->m_Prop->SetAmbient(0);
    m_CylGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
    m_CylGizmo->GetMaterial()->m_Prop->SetSpecular(0);
  }
  else
  {
    m_CubeGizmo->GetMaterial()->m_Prop->SetColor(col);
    m_CubeGizmo->GetMaterial()->m_Prop->SetAmbient(0);
    m_CubeGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
    m_CubeGizmo->GetMaterial()->m_Prop->SetSpecular(0);
  }
}

//----------------------------------------------------------------------------
void albaGizmoScaleAxis::SetColor(int part, double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(part, col);
}

//----------------------------------------------------------------------------
void albaGizmoScaleAxis::SetColor(double cylCol[3], double coneCol[3])
//----------------------------------------------------------------------------
{
  this->SetColor(CYLINDER, cylCol);
  this->SetColor(CUBE, coneCol);
}

//----------------------------------------------------------------------------
void albaGizmoScaleAxis::SetColor(double cylR, double cylG, double cylB, double coneR, double coneG, double coneB)
//----------------------------------------------------------------------------
{
  this->SetColor(CYLINDER, cylR, cylG, cylB);
  this->SetColor(CUBE, coneR, coneG, coneB);
}

//----------------------------------------------------------------------------
void albaGizmoScaleAxis::Show(bool show)
//----------------------------------------------------------------------------
{
  m_Show = show;
  GetLogicManager()->VmeShow(m_CylGizmo, show);
  GetLogicManager()->VmeShow(m_CubeGizmo, show);
}

//----------------------------------------------------------------------------
void albaGizmoScaleAxis::SetAbsPose(albaMatrix *absPose)
//----------------------------------------------------------------------------
{
  m_CubeGizmo->SetAbsMatrix(*absPose); 
  m_CylGizmo->SetAbsMatrix(*absPose); 
  SetRefSysMatrix(absPose);
}

//----------------------------------------------------------------------------
void albaGizmoScaleAxis::SetRefSysMatrix(albaMatrix *matrix)
//----------------------------------------------------------------------------
{  
  for (int i = 0; i < 2; i++)
  {
    m_IsaGen[i]->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(matrix);
    m_IsaGen[i]->GetPivotRefSys()->SetTypeToCustom(matrix);
  } 
}

//----------------------------------------------------------------------------
albaMatrix *albaGizmoScaleAxis::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_CylGizmo->GetOutput()->GetAbsMatrix();
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