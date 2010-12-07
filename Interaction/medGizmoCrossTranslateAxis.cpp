/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGizmoCrossTranslateAxis.cpp,v $
Language:  C++
Date:      $Date: 2010-12-07 10:45:06 $
Version:   $Revision: 1.1.2.5 $
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


#include "medGizmoCrossTranslateAxis.h"
#include "mafDecl.h"

// isa stuff
#include "mafInteractorCompositorMouse.h"
#include "mafInteractorGenericMouse.h"

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
#include "vtkAppendPolyData.h"

//----------------------------------------------------------------------------
medGizmoCrossTranslateAxis::medGizmoCrossTranslateAxis(mafVME *input, mafObserver *listener)
//----------------------------------------------------------------------------
{
	m_LastColor[0] = 0;
	m_LastColor[1] = 0.7;
	m_LastColor[2] = 1;

	m_CylinderLength = 0.5;

	m_IsaComp = NULL;

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
	m_CylGizmo = mafVMEGizmo::New();
	m_CylGizmo->SetName("CylGizmo");
	m_CylGizmo->SetData(m_Append->GetOutput());
	m_CylGizmo->SetMediator(m_Listener);
	// cone gizmo

	// assign isa to cylinder and cone
	m_CylGizmo->SetBehavior(m_IsaComp);

	SetAbsPose(m_InputVme->GetOutput()->GetAbsMatrix());

	// set come gizmo material property and initial color to red
	this->SetColor(m_LastColor);

	//-----------------
	// ReparentTo will add also the gizmos to the tree!!
	// add the gizmo to the tree, this should increase reference count 
	m_CylGizmo->ReparentTo(mafVME::SafeDownCast(m_InputVme->GetRoot()));
}
//----------------------------------------------------------------------------
medGizmoCrossTranslateAxis::~medGizmoCrossTranslateAxis() 
//----------------------------------------------------------------------------
{
	m_CylGizmo->SetBehavior(NULL);

	vtkDEL(m_RightCylinder);

	// clean up
	vtkDEL(m_RightTranslateTr);
	vtkDEL(m_RightTranslatePDF);
	vtkDEL(m_RightCylinderRotationTr);
	vtkDEL(m_RightCylinderRotatePDF);

  // clean up
  vtkDEL(m_LeftTranslateTr);
  vtkDEL(m_LeftTranslatePDF);
  vtkDEL(m_LeftCylinderRotationTr);
  vtkDEL(m_LeftCylinderRotatePDF);

  vtkDEL(m_Append);

  //----------------------
	// No leaks so somebody is performing this...
	// wxDEL(GizmoData);
	//----------------------
	vtkDEL(m_IsaComp); 

	m_CylGizmo->ReparentTo(NULL);
}

//----------------------------------------------------------------------------
void medGizmoCrossTranslateAxis::CreatePipeline() 
//----------------------------------------------------------------------------
{
	// create pipeline for cone-cylinder gizmo along global X axis
	// calculate diagonal of m_InputVme space bounds 
	double b[6],p1[3],p2[3],boundingBoxDiagonal;
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
	boundingBoxDiagonal = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));

	// create the right cylinder
	m_RightCylinder = vtkCylinderSource::New();

	double tubeRadius = boundingBoxDiagonal / 350;
	m_RightCylinder->SetRadius(tubeRadius);

	//-----------------
	// rotate the cylinder on the X axis (default axis is Z)
	//-----------------
	vtkTransform *rightCylinderInitialTr = vtkTransform::New();
	rightCylinderInitialTr->RotateZ(-90);	

	vtkTransformPolyDataFilter *rightCylinderInitialTrPDF = vtkTransformPolyDataFilter::New();
	rightCylinderInitialTrPDF->SetInput(m_RightCylinder->GetOutput());
	rightCylinderInitialTrPDF->SetTransform(rightCylinderInitialTr);

	// create the translation transform
	m_RightTranslateTr = vtkTransform::New();
  m_RightTranslateTr->Translate(3* boundingBoxDiagonal / 8, 0, 0);

	// create cylinder translation transform
	m_RightTranslatePDF = vtkTransformPolyDataFilter::New();
	m_RightTranslatePDF->SetInput(rightCylinderInitialTrPDF->GetOutput());

	//-----------------
	// translate transform setting
	m_RightTranslatePDF->SetTransform(m_RightTranslateTr);

	// create rotation transform and rotation TPDF 
	m_RightCylinderRotatePDF = vtkTransformPolyDataFilter::New();
	m_RightCylinderRotationTr = vtkTransform::New();
	m_RightCylinderRotationTr->Identity(); 

	m_RightCylinderRotatePDF->SetTransform(m_RightCylinderRotationTr);

	m_RightCylinderRotatePDF->SetInput(m_RightTranslatePDF->GetOutput());

	m_RightCylinderRotatePDF->Update();
	m_RightCylinderRotatePDF->Update();



  // create the left cylinder
  m_LeftCylinder = vtkCylinderSource::New();

  m_LeftCylinder->SetRadius(tubeRadius);

  //-----------------
  // rotate the cylinder on the X axis (default axis is Z)
  //-----------------
  vtkTransform *LeftCylinderInitialTr = vtkTransform::New();
  LeftCylinderInitialTr->RotateZ(-90);	

  vtkTransformPolyDataFilter *LeftCylinderInitialTrPDF = vtkTransformPolyDataFilter::New();
  LeftCylinderInitialTrPDF->SetInput(m_LeftCylinder->GetOutput());
  LeftCylinderInitialTrPDF->SetTransform(LeftCylinderInitialTr);

  // create the translation transform
  m_LeftTranslateTr = vtkTransform::New();
  m_LeftTranslateTr->Translate( - 3* boundingBoxDiagonal / 8, 0, 0);

  // create cylinder translation transform
  m_LeftTranslatePDF = vtkTransformPolyDataFilter::New();
  m_LeftTranslatePDF->SetInput(LeftCylinderInitialTrPDF->GetOutput());

  // place the cylinder before the cone; default cylinder length is 1/4 of vme bb diagonal
  this->SetCylinderLength(boundingBoxDiagonal / 16);

  //-----------------
  // translate transform setting
  m_LeftTranslatePDF->SetTransform(m_LeftTranslateTr);


  // create rotation transform and rotation TPDF 
  m_LeftCylinderRotatePDF = vtkTransformPolyDataFilter::New();
  m_LeftCylinderRotationTr = vtkTransform::New();
  m_LeftCylinderRotationTr->Identity(); 

  m_LeftCylinderRotatePDF->SetTransform(m_LeftCylinderRotationTr);

  m_LeftCylinderRotatePDF->SetInput(m_LeftTranslatePDF->GetOutput());

  m_LeftCylinderRotatePDF->Update();
  m_LeftCylinderRotatePDF->Update();

  // place the cylinder before the cone; default cylinder length is 1/4 of vme bb diagonal
  this->SetCylinderLength(boundingBoxDiagonal / 16);

  m_Append = vtkAppendPolyData::New();
  m_Append->SetInput(m_RightCylinderRotatePDF->GetOutput());
  m_Append->AddInput(m_LeftCylinderRotatePDF->GetOutput());
  m_Append->Update();

	//clean up
	rightCylinderInitialTr->Delete();
	rightCylinderInitialTrPDF->Delete();
}
//----------------------------------------------------------------------------
void medGizmoCrossTranslateAxis::CreateISA()
//----------------------------------------------------------------------------
{
	m_IsaComp = mafInteractorCompositorMouse::New();

	m_IsaGen = m_IsaComp->CreateBehavior(MOUSE_LEFT);

	m_IsaGen->SetVME(m_InputVme);
	m_IsaGen->GetTranslationConstraint()->SetConstraintModality(mafInteractorConstraint::FREE, mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK);
	m_IsaGen->GetTranslationConstraint()->GetRefSys()->SetTypeToLocal();

	m_IsaGen->SetListener(this);
}
//----------------------------------------------------------------------------
void medGizmoCrossTranslateAxis::SetAxis(int axis) 
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
		m_RightCylinderRotationTr->Identity();

		// set cyl and cone color to red
		this->SetColor(m_LastColor);

		// change the axis constrain
		m_IsaGen->GetTranslationConstraint()->SetConstraintModality(mafInteractorConstraint::LOCK, mafInteractorConstraint::FREE, mafInteractorConstraint::LOCK);
	}
	else if (axis == Y)
	{
		// set rotation to move con and cyl on Y 
		m_RightCylinderRotationTr->Identity();
		m_RightCylinderRotationTr->RotateZ(90);

    m_LeftCylinderRotationTr->Identity();
    m_LeftCylinderRotationTr->RotateZ(90);

		// set cyl and cone color to green
		this->SetColor(m_LastColor);

		// change the axis constrain
		m_IsaGen->GetTranslationConstraint()->SetConstraintModality(mafInteractorConstraint::FREE, mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK);
	}  
	else if (axis == Z)
	{
		// set rotation to move con and cyl on Z
		m_RightCylinderRotationTr->Identity();
		m_RightCylinderRotationTr->RotateY(-90);

    m_LeftCylinderRotationTr->Identity();
    m_LeftCylinderRotationTr->RotateY(-90);

		// set cyl and cone color to blue
		this->SetColor(m_LastColor);

		// change the axis constrain
		m_IsaGen->GetTranslationConstraint()->SetConstraintModality(mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK, mafInteractorConstraint::FREE);
	}  
}
//----------------------------------------------------------------------------
void medGizmoCrossTranslateAxis::Highlight(bool highlight)
//----------------------------------------------------------------------------
{
	if (highlight == true)
	{
		m_LastColor[0] = m_Color[0];
		m_LastColor[1] = m_Color[1];
		m_LastColor[2] = m_Color[2];

		// Highlight the cylinder by setting its color to yellow 
		this->SetColor(1, 1, 0);
	} 
	else
	{
		// set cyl col to red
		this->SetColor(m_LastColor); 
	}
}


//----------------------------------------------------------------------------
void medGizmoCrossTranslateAxis::SetCylinderLength(double length)
//----------------------------------------------------------------------------
{
	// set cylLen to length
	m_CylinderLength = length;
	m_RightCylinder->SetHeight(4 * length);
  m_LeftCylinder->SetHeight(4 * length);
}
//----------------------------------------------------------------------------
void medGizmoCrossTranslateAxis::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	// forward events to the listener
	maf_event->SetSender(this);
	mafEventMacro(*maf_event);
}
/** Gizmo color */
//----------------------------------------------------------------------------
void medGizmoCrossTranslateAxis::SetColor(double col[3])
//----------------------------------------------------------------------------
{
	m_Color[0] = col[0];
	m_Color[1] = col[1];
	m_Color[2] = col[2];

	m_CylGizmo->GetMaterial()->m_Prop->SetColor(col);
	m_CylGizmo->GetMaterial()->m_Prop->SetAmbient(0);
	m_CylGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
	m_CylGizmo->GetMaterial()->m_Prop->SetSpecular(0);
}

//----------------------------------------------------------------------------
void medGizmoCrossTranslateAxis::SetColor(double cylR, double cylG, double cylB)
//----------------------------------------------------------------------------
{
	double color[3] = {cylR, cylG, cylB};
	this->SetColor(color);
}
//----------------------------------------------------------------------------
void medGizmoCrossTranslateAxis::Show(bool show)
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,VME_SHOW,m_CylGizmo,show));
}
//----------------------------------------------------------------------------
void medGizmoCrossTranslateAxis::SetAbsPose(mafMatrix *absPose)
//----------------------------------------------------------------------------
{
	m_CylGizmo->SetAbsMatrix(*absPose); 
	SetRefSysMatrix(absPose);
}
//----------------------------------------------------------------------------
void medGizmoCrossTranslateAxis::SetRefSysMatrix(mafMatrix *matrix)
//----------------------------------------------------------------------------
{  
	m_IsaGen->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(matrix);
	m_IsaGen->GetPivotRefSys()->SetTypeToCustom(matrix);
}
//----------------------------------------------------------------------------
mafMatrix *medGizmoCrossTranslateAxis::GetAbsPose()
//----------------------------------------------------------------------------
{
	return m_CylGizmo->GetOutput()->GetAbsMatrix();
}
//----------------------------------------------------------------------------
void medGizmoCrossTranslateAxis::SetInput(mafVME *vme)
//----------------------------------------------------------------------------
{
	this->m_InputVme = vme; 
	SetAbsPose(vme->GetOutput()->GetAbsMatrix()); 
	SetRefSysMatrix(vme->GetOutput()->GetAbsMatrix());
}
//----------------------------------------------------------------------------
void medGizmoCrossTranslateAxis::SetConstraintModality(int axis, int constrainModality)
//----------------------------------------------------------------------------
{
	m_IsaGen->GetTranslationConstraint()->SetConstraintModality(axis,constrainModality); 
}
//----------------------------------------------------------------------------
void medGizmoCrossTranslateAxis::SetStep(int axis, double step)
//----------------------------------------------------------------------------
{
	m_IsaGen->GetTranslationConstraint()->SetStep(axis,step);
}
