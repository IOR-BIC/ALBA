/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoScaleIsotropic
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


#include "albaGizmoScaleIsotropic.h"
#include "albaDecl.h"
#include "albaRefSys.h"

// isa stuff
#include "albaInteractorCompositorMouse.h"
#include "albaInteractorGenericMouse.h"

// vme stuff
#include "mmaMaterial.h"
#include "albaVME.h"
#include "albaVMEOutput.h"
#include "albaVMEGizmo.h"
#include "albaMatrix.h"
#include "albaTransform.h"

// vtk stuff
#include "vtkCubeSource.h"
#include "vtkCylinderSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkProperty.h"

//----------------------------------------------------------------------------
albaGizmoScaleIsotropic::albaGizmoScaleIsotropic(albaVME *input, albaObserver *listener)
//----------------------------------------------------------------------------
{
  m_IsaComp = NULL;

  m_Listener = listener;
  m_InputVme = input;
  
  // create pipeline stuff
  CreatePipeline();

  // create isa stuff
  CreateISA();

  //-----------------
  // create vme gizmo stuff
  //-----------------

  // cube gizmo
  m_CubeGizmo = albaVMEGizmo::New();  
  m_CubeGizmo->SetName("CubeGizmo");
  m_CubeGizmo->SetDataConnection(m_Cube->GetOutputPort());
  m_CubeGizmo->SetMediator(m_Listener);

  // assign isa to cube
  m_CubeGizmo->SetBehavior(m_IsaComp); 


  SetAbsPose(m_InputVme->GetOutput()->GetAbsMatrix());
  
  // set cube gizmo material property and initial color to light blue
  this->SetColor(0, 1, 1);

  //-----------------
  m_CubeGizmo->ReparentTo(m_InputVme->GetRoot());
}
//----------------------------------------------------------------------------
albaGizmoScaleIsotropic::~albaGizmoScaleIsotropic() 
//----------------------------------------------------------------------------
{
  m_CubeGizmo->SetBehavior(NULL);
  vtkDEL(m_Cube);
  vtkDEL(m_IsaComp); 

	m_CubeGizmo->ReparentTo(NULL);
}

//----------------------------------------------------------------------------
void albaGizmoScaleIsotropic::CreatePipeline() 
//----------------------------------------------------------------------------
{
  // create pipeline for cube-cylinder gizmo along global X axis
  m_InputVme->Update();
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
  
  // create the cube
  m_Cube = vtkCubeSource::New();

  // place the cube; default cube length is 1/16 of vme bb diagonal
  this->SetCubeLength(d / 16);
}

//----------------------------------------------------------------------------
void albaGizmoScaleIsotropic::CreateISA()
//----------------------------------------------------------------------------
{
  
  // create isa compositor and assign behaviors to m_IsaGen ivar
  m_IsaComp = albaInteractorCompositorMouse::New();

  // default behavior is activated by mouse left and is constrained to X axis,
  // default ref sys is input vme abs matrix
  m_IsaGen = m_IsaComp->CreateBehavior(MOUSE_LEFT);

  m_IsaGen->SetVME(m_InputVme);
  m_IsaGen->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK);
  m_IsaGen->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
    
  //isa will send events to this
  m_IsaGen->SetListener(this);
}

//----------------------------------------------------------------------------
void albaGizmoScaleIsotropic::Highlight(bool highlight)
//----------------------------------------------------------------------------
{
  if (highlight == true)
  {
    // Highlight the cube by setting its color to yellow 
    this->SetColor(1, 1, 0);
  } 
  else
  {
    this->SetColor(0, 1, 1);
  } 
}

//----------------------------------------------------------------------------
void  albaGizmoScaleIsotropic::SetCubeLength(double cubeLength)
//----------------------------------------------------------------------------
{
  // set the cube length
  m_Cube->SetXLength(cubeLength);
  m_Cube->SetYLength(cubeLength);
  m_Cube->SetZLength(cubeLength);
}

//----------------------------------------------------------------------------
double albaGizmoScaleIsotropic::GetCubeLength() const
//----------------------------------------------------------------------------
{
  return m_Cube->GetXLength();
}

//----------------------------------------------------------------------------
void albaGizmoScaleIsotropic::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  // forward events to the listener
	alba_event->SetSender(this);
  albaEventMacro(*alba_event);
}

/** Gizmo color */
//----------------------------------------------------------------------------
void albaGizmoScaleIsotropic::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_CubeGizmo->GetMaterial()->m_Prop->SetColor(col);
  m_CubeGizmo->GetMaterial()->m_Prop->SetAmbient(0);
	m_CubeGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
	m_CubeGizmo->GetMaterial()->m_Prop->SetSpecular(0);
}

//----------------------------------------------------------------------------
void albaGizmoScaleIsotropic::SetColor(double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(col);
}


//----------------------------------------------------------------------------
void albaGizmoScaleIsotropic::Show(bool show)
//----------------------------------------------------------------------------
{
	GetLogicManager()->VmeShow(m_CubeGizmo, show);
}

//----------------------------------------------------------------------------
void albaGizmoScaleIsotropic::SetAbsPose(albaMatrix *absPose)
//----------------------------------------------------------------------------
{
  m_CubeGizmo->SetAbsMatrix(*absPose); 
  SetRefSysMatrix(absPose);
}

//----------------------------------------------------------------------------
void albaGizmoScaleIsotropic::SetRefSysMatrix(albaMatrix *matrix)
//----------------------------------------------------------------------------
{  
  m_IsaGen->GetTranslationConstraint()->GetRefSys()->SetMatrix(matrix);
  m_IsaGen->GetPivotRefSys()->SetTypeToCustom(matrix); 
}

//----------------------------------------------------------------------------
albaMatrix *albaGizmoScaleIsotropic::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_CubeGizmo->GetOutput()->GetAbsMatrix();
}

//----------------------------------------------------------------------------
void albaGizmoScaleIsotropic::SetInput(albaVME *vme)
//----------------------------------------------------------------------------
{
  this->m_InputVme = vme; 
  SetAbsPose(vme->GetOutput()->GetAbsMatrix()); 
  SetRefSysMatrix(vme->GetOutput()->GetAbsMatrix());
}
