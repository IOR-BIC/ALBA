/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoScaleIsotropic.cpp,v $
  Language:  C++
  Date:      $Date: 2009-03-26 16:52:57 $
  Version:   $Revision: 1.7.4.1 $
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


#include "mafGizmoScaleIsotropic.h"
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
#include "mafTransform.h"

// vtk stuff
#include "vtkCubeSource.h"
#include "vtkCylinderSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkProperty.h"

//----------------------------------------------------------------------------
mafGizmoScaleIsotropic::mafGizmoScaleIsotropic(mafVME *input, mafObserver *listener)
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
  m_CubeGizmo = mafVMEGizmo::New();  
  m_CubeGizmo->SetName("CubeGizmo");
  m_CubeGizmo->SetData(m_Cube->GetOutput());
  
  // assign isa to cube
  m_CubeGizmo->SetBehavior(m_IsaComp); 
  
  SetAbsPose(m_InputVme->GetOutput()->GetAbsMatrix());
  
  // set cube gizmo material property and initial color to light blue
  this->SetColor(0, 1, 1);

  //-----------------
  m_CubeGizmo->ReparentTo(mafVME::SafeDownCast(m_InputVme->GetRoot()));
}
//----------------------------------------------------------------------------
mafGizmoScaleIsotropic::~mafGizmoScaleIsotropic() 
//----------------------------------------------------------------------------
{
  m_CubeGizmo->SetBehavior(NULL);
  vtkDEL(m_Cube);
  vtkDEL(m_IsaComp); 

	m_CubeGizmo->ReparentTo(NULL);
}

//----------------------------------------------------------------------------
void mafGizmoScaleIsotropic::CreatePipeline() 
//----------------------------------------------------------------------------
{
  // create pipeline for cube-cylinder gizmo along global X axis
  m_InputVme->Update();
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
  
  // create the cube
  m_Cube = vtkCubeSource::New();

  // place the cube; default cube length is 1/16 of vme bb diagonal
  this->SetCubeLength(d / 16);
}

//----------------------------------------------------------------------------
void mafGizmoScaleIsotropic::CreateISA()
//----------------------------------------------------------------------------
{
  
  // create isa compositor and assign behaviors to m_IsaGen ivar
  m_IsaComp = mmiCompositorMouse::New();

  // default behavior is activated by mouse left and is constrained to X axis,
  // default ref sys is input vme abs matrix
  m_IsaGen = m_IsaComp->CreateBehavior(MOUSE_LEFT);

  m_IsaGen->SetVME(m_InputVme);
  m_IsaGen->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::LOCK, mmiConstraint::LOCK);
  m_IsaGen->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
    
  //isa will send events to this
  m_IsaGen->SetListener(this);
}

//----------------------------------------------------------------------------
void mafGizmoScaleIsotropic::Highlight(bool highlight)
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
void  mafGizmoScaleIsotropic::SetCubeLength(double cubeLength)
//----------------------------------------------------------------------------
{
  // set the cube length
  m_Cube->SetXLength(cubeLength);
  m_Cube->SetYLength(cubeLength);
  m_Cube->SetZLength(cubeLength);
}

//----------------------------------------------------------------------------
double mafGizmoScaleIsotropic::GetCubeLength() const
//----------------------------------------------------------------------------
{
  return m_Cube->GetXLength();
}

//----------------------------------------------------------------------------
void mafGizmoScaleIsotropic::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // forward events to the listener
	maf_event->SetSender(this);
  mafEventMacro(*maf_event);
}

/** Gizmo color */
//----------------------------------------------------------------------------
void mafGizmoScaleIsotropic::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_CubeGizmo->GetMaterial()->m_Prop->SetColor(col);
  m_CubeGizmo->GetMaterial()->m_Prop->SetAmbient(0);
	m_CubeGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
	m_CubeGizmo->GetMaterial()->m_Prop->SetSpecular(0);
}

//----------------------------------------------------------------------------
void mafGizmoScaleIsotropic::SetColor(double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(col);
}


//----------------------------------------------------------------------------
void mafGizmoScaleIsotropic::Show(bool show)
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,VME_SHOW,m_CubeGizmo,show));
}

//----------------------------------------------------------------------------
void mafGizmoScaleIsotropic::SetAbsPose(mafMatrix *absPose)
//----------------------------------------------------------------------------
{
  m_CubeGizmo->SetAbsMatrix(*absPose); 
  SetRefSysMatrix(absPose);
}

//----------------------------------------------------------------------------
void mafGizmoScaleIsotropic::SetRefSysMatrix(mafMatrix *matrix)
//----------------------------------------------------------------------------
{  
  m_IsaGen->GetTranslationConstraint()->GetRefSys()->SetMatrix(matrix);
  m_IsaGen->GetPivotRefSys()->SetTypeToCustom(matrix); 
}

//----------------------------------------------------------------------------
mafMatrix *mafGizmoScaleIsotropic::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_CubeGizmo->GetOutput()->GetAbsMatrix();
}

//----------------------------------------------------------------------------
void mafGizmoScaleIsotropic::SetInput(mafVME *vme)
//----------------------------------------------------------------------------
{
  this->m_InputVme = vme; 
  SetAbsPose(vme->GetOutput()->GetAbsMatrix()); 
  SetRefSysMatrix(vme->GetOutput()->GetAbsMatrix());
}
