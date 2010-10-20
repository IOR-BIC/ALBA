/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGizmoCrossTranslateAxis.cpp,v $
Language:  C++
Date:      $Date: 2010-10-20 15:28:03 $
Version:   $Revision: 1.1.2.2 $
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
	m_CylGizmo->SetData(m_RotatePDF->GetOutput());
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

	vtkDEL(m_Cylinder);

	// clean up
	vtkDEL(m_TranslateTr);
	vtkDEL(m_TranslatePDF);
	vtkDEL(m_RotationTr);
	vtkDEL(m_RotatePDF);
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

	// create the cylinder
	m_Cylinder = vtkCylinderSource::New();
	m_Cylinder->SetRadius(d / 200);

	//-----------------
	// rotate the cylinder on the X axis (default axis is Z)
	//-----------------
	vtkTransform *cylInitTr = vtkTransform::New();
	cylInitTr->RotateZ(-90);	

	vtkTransformPolyDataFilter *cylInitTrPDF = vtkTransformPolyDataFilter::New();
	cylInitTrPDF->SetInput(m_Cylinder->GetOutput());
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

	// create the translation transform
	m_TranslateTr = vtkTransform::New();

	// create cylinder translation transform
	m_TranslatePDF = vtkTransformPolyDataFilter::New();
	m_TranslatePDF->SetInput(cylInitTrPDF->GetOutput());

	// place the cylinder before the cone; default cylinder length is 1/4 of vme bb diagonal
	this->SetCylinderLength(d / 4);

	//-----------------
	// translate transform setting
	m_TranslatePDF->SetTransform(m_TranslateTr);


	// create rotation transform and rotation TPDF 
	m_RotatePDF = vtkTransformPolyDataFilter::New();
	m_RotationTr = vtkTransform::New();
	m_RotationTr->Identity(); 

	m_RotatePDF->SetTransform(m_RotationTr);

	m_RotatePDF->SetInput(m_TranslatePDF->GetOutput());

	m_RotatePDF->Update();
	m_RotatePDF->Update();

	//clean up
	cylInitTr->Delete();
	cylInitTrPDF->Delete();
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
		m_RotationTr->Identity();

		// set cyl and cone color to red
		this->SetColor(m_LastColor);

		// change the axis constrain
		m_IsaGen->GetTranslationConstraint()->SetConstraintModality(mafInteractorConstraint::LOCK, mafInteractorConstraint::FREE, mafInteractorConstraint::LOCK);
	}
	else if (axis == Y)
	{
		// set rotation to move con and cyl on Y 
		m_RotationTr->Identity();
		m_RotationTr->RotateZ(90);

		// set cyl and cone color to green
		this->SetColor(m_LastColor);

		// change the axis constrain
		m_IsaGen->GetTranslationConstraint()->SetConstraintModality(mafInteractorConstraint::FREE, mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK);
	}  
	else if (axis == Z)
	{
		// set rotation to move con and cyl on Z
		m_RotationTr->Identity();
		m_RotationTr->RotateY(-90);

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
	m_Cylinder->SetHeight(4 * length);
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
