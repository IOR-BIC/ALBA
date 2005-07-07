/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoScaleIsotropic.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-07 15:14:53 $
  Version:   $Revision: 1.2 $
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
  IsaComp = NULL;

  m_Listener = listener;
  InputVme = input;
  
  // create pipeline stuff
  CreatePipeline();

  // create isa stuff
  CreateISA();

  //-----------------
  // create vme gizmo stuff
  //-----------------

  // cube gizmo
  CubeGizmo = mafVMEGizmo::New();  
  CubeGizmo->SetName("CubeGizmo");
  CubeGizmo->ReparentTo(mafVME::SafeDownCast(InputVme->GetRoot()));
  CubeGizmo->SetData(Cube->GetOutput());
  
  // assign isa to cube
  CubeGizmo->SetBehavior(IsaComp); 
  
  SetAbsPose(InputVme->GetOutput()->GetAbsMatrix());
  
  // set cube gizmo material property and initial color to light blue
  this->SetColor(0, 1, 1);

  // hide gizmos at creation
  this->Show(false);

  //-----------------
  
  // add the gizmo to the tree, this should increase reference count 
  mafEventMacro(mafEvent(this, VME_ADD, CubeGizmo));
}
//----------------------------------------------------------------------------
mafGizmoScaleIsotropic::~mafGizmoScaleIsotropic() 
//----------------------------------------------------------------------------
{
  CubeGizmo->SetBehavior(NULL);
  vtkDEL(Cube);
  
  // clean up
	//----------------------
	// No leaks so somebody is performing this...
	// wxDEL(GizmoData);
	//----------------------
  vtkDEL(IsaComp); 

  mafEventMacro(mafEvent(this, VME_REMOVE, CubeGizmo));  
//  CubeGizmo->Delete();
}

//----------------------------------------------------------------------------
void mafGizmoScaleIsotropic::CreatePipeline() 
//----------------------------------------------------------------------------
{
  // create pipeline for cube-cylinder gizmo along global X axis
  InputVme->Update();
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
  
  // create the cube
  Cube = vtkCubeSource::New();

  // place the cube; default cube length is 1/16 of vme bb diagonal
  this->SetCubeLength(d / 16);
}

//----------------------------------------------------------------------------
void mafGizmoScaleIsotropic::CreateISA()
//----------------------------------------------------------------------------
{
  
  // create isa compositor and assign behaviors to IsaGen ivar
  IsaComp = mmiCompositorMouse::New();

  // default behavior is activated by mouse left and is constrained to X axis,
  // default ref sys is input vme abs matrix
  IsaGen = IsaComp->CreateBehavior(MOUSE_LEFT);

  IsaGen->SetVME(InputVme);
  IsaGen->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::LOCK, mmiConstraint::LOCK);
  IsaGen->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
    
  //isa will send events to this
  IsaGen->SetListener(this);
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
  Cube->SetXLength(cubeLength);
  Cube->SetYLength(cubeLength);
  Cube->SetZLength(cubeLength);
}

//----------------------------------------------------------------------------
double mafGizmoScaleIsotropic::GetCubeLength() const
//----------------------------------------------------------------------------
{
  return Cube->GetXLength();
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
  CubeGizmo->GetMaterial()->m_Prop->SetColor(col);
  CubeGizmo->GetMaterial()->m_Prop->SetAmbient(0);
	CubeGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
	CubeGizmo->GetMaterial()->m_Prop->SetSpecular(0);
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
  double opacity = ((show == TRUE) ? 1 : 0);
  CubeGizmo->GetMaterial()->m_Prop->SetOpacity(opacity);
}

//----------------------------------------------------------------------------
void mafGizmoScaleIsotropic::SetAbsPose(mafMatrix *absPose)
//----------------------------------------------------------------------------
{
  CubeGizmo->SetAbsMatrix(*absPose); 
  SetRefSysMatrix(absPose);
}

//----------------------------------------------------------------------------
void mafGizmoScaleIsotropic::SetRefSysMatrix(mafMatrix *matrix)
//----------------------------------------------------------------------------
{  
  IsaGen->GetTranslationConstraint()->GetRefSys()->SetMatrix(matrix);
  IsaGen->GetPivotRefSys()->SetTypeToCustom(matrix); 
}

//----------------------------------------------------------------------------
mafMatrix *mafGizmoScaleIsotropic::GetAbsPose()
//----------------------------------------------------------------------------
{
  return CubeGizmo->GetOutput()->GetAbsMatrix();
}

//----------------------------------------------------------------------------
void mafGizmoScaleIsotropic::SetInput(mafVME *vme)
//----------------------------------------------------------------------------
{
  this->InputVme = vme; 
  SetAbsPose(vme->GetOutput()->GetAbsMatrix()); 
  SetRefSysMatrix(vme->GetOutput()->GetAbsMatrix());
}
