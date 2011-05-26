/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGizmoCrossTranslateAxis.cpp,v $
Language:  C++
Date:      $Date: 2011-05-26 07:55:17 $
Version:   $Revision: 1.1.2.12 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "medGizmoCrossTranslateAxis.h"

#include "mafDecl.h"

// maf stuff
#include "mmaMaterial.h"
#include "mafMatrix.h"
#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafVMEGizmo.h"
#include "mafTagArray.h"
#include "mafInteractorCompositorMouse.h"
#include "mafInteractorGenericMouse.h"
#include "mafRefSys.h"


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


	// translation feedback stuff
	m_TranslationFeedbackGizmo = NULL;
	m_FeedbackConeSource = NULL;
	m_FeedbackCylinderSource = NULL;

	m_LeftUpFeedbackConeTransform = NULL;
	m_LeftDownFeedbackConeTransform = NULL;
	m_RightDownFeedbackConeTransform = NULL;
	m_RightUpFeedbackConeTransform = NULL;

	m_LeftUpFeedbackConeTransformPDF = NULL;
	m_LeftDownFeedbackConeTransformPDF = NULL;
	m_RightUpFeedbackConeTransformPDF = NULL;
	m_RightDownFeedbackConeTransformPDF = NULL;

	m_LeftFeedbackCylinderTransform = NULL;
	m_RightFeedbackCylinderTransform = NULL;

	m_LeftFeedbackCylinderTransformPDF = NULL;
	m_RightFeedbackCylinderTransformPDF = NULL;

	m_FeedbackStuffAppendPolydata = NULL;

	// default last color is red
	m_LastColor[0] = -1;
	m_LastColor[1] = -1;
	m_LastColor[2] = -1;

	m_CylinderLength = 0.5;

	m_IsaComp = NULL;

	m_Listener = listener;
	m_InputVme = input;

	// default axis is X
	m_Axis = X;

	// create VTK pipeline stuff
	CreateTranslationGizmoPipeline();

	// create interactor stuff
	CreateISA();

	//-----------------
	// create vme gizmos stuff
	//-----------------
	// cylinder gizmo
	m_TranslationCylinderGizmo = mafVMEGizmo::New();
	//  m_TranslationCylinderGizmo->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 1));
	m_TranslationCylinderGizmo->SetName("AxisTranslationGizmo");
	m_TranslationCylinderGizmo->SetData(m_Append->GetOutput());
	m_TranslationCylinderGizmo->SetMediator(m_Listener);
	// cone gizmo

	// assign isa to cylinder and cone
	m_TranslationCylinderGizmo->SetBehavior(m_IsaComp);

	// default gizmo abs pose is the input vme one
	SetAbsPose(m_InputVme->GetOutput()->GetAbsMatrix());

	// ReparentTo will add the gizmos to the tree
	// and increse reference count
	m_TranslationCylinderGizmo->ReparentTo(mafVME::SafeDownCast(m_InputVme->GetRoot()));

	// build translation feedback gizmo stuff
	CreateFeedbackGizmoPipeline();

	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
	//======================== WORK IN PROGRESS !!!!! ======================== 
}
//----------------------------------------------------------------------------
medGizmoCrossTranslateAxis::~medGizmoCrossTranslateAxis() 
//----------------------------------------------------------------------------
{
	m_TranslationCylinderGizmo->SetBehavior(NULL);

	// clean up
	vtkDEL(m_RightCylinder);
	vtkDEL(m_RightTranslateTr);
	vtkDEL(m_RightTranslatePDF);
	vtkDEL(m_RightCylinderRotationTr);
	vtkDEL(m_RightCylinderRotatePDF);
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

	m_TranslationCylinderGizmo->ReparentTo(NULL);

	// clean up translation feedback stuff
	vtkDEL(m_TranslationFeedbackGizmo);
	vtkDEL(m_FeedbackConeSource);
	vtkDEL(m_LeftUpFeedbackConeTransform);
	vtkDEL(m_LeftDownFeedbackConeTransform);
	vtkDEL(m_RightUpFeedbackConeTransform);
	vtkDEL(m_RightDownFeedbackConeTransform);
	vtkDEL(m_LeftUpFeedbackConeTransformPDF);
	vtkDEL(m_LeftDownFeedbackConeTransformPDF);
	vtkDEL(m_RightUpFeedbackConeTransformPDF);
	vtkDEL(m_RightDownFeedbackConeTransformPDF);
	vtkDEL(m_FeedbackCylinderSource);
	vtkDEL(m_LeftFeedbackCylinderTransform);
	vtkDEL(m_LeftFeedbackCylinderTransformPDF);
	vtkDEL(m_RightFeedbackCylinderTransform);
	vtkDEL(m_RightFeedbackCylinderTransformPDF);
	vtkDEL(m_FeedbackStuffAppendPolydata);
}

//----------------------------------------------------------------------------
void medGizmoCrossTranslateAxis::CreateTranslationGizmoPipeline() 
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

	double tubeRadius = boundingBoxDiagonal / 75;
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

	// rotate the cylinder on the X axis (default axis is Z)
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

	// translate transform setting
	m_LeftTranslatePDF->SetTransform(m_LeftTranslateTr);

	// create rotation transform and rotation TPDF 
	m_LeftCylinderRotatePDF = vtkTransformPolyDataFilter::New();
	m_LeftCylinderRotationTr = vtkTransform::New();
	m_LeftCylinderRotationTr->Identity(); 

	m_LeftCylinderRotatePDF->SetTransform(m_LeftCylinderRotationTr);
	m_LeftCylinderRotatePDF->SetInput(m_LeftTranslatePDF->GetOutput());
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

		m_LeftFeedbackCylinderTransform->RotateZ(90);
		m_RightFeedbackCylinderTransform->RotateZ(90);

		m_LeftDownFeedbackConeTransform->RotateZ(90);
		m_LeftUpFeedbackConeTransform->RotateZ(90);
		m_RightDownFeedbackConeTransform->RotateZ(90);
		m_RightUpFeedbackConeTransform->RotateZ(90);

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

	if (m_LastColor[0] == -1)
	{
		m_LastColor[0] = m_Color[0];
		m_LastColor[1] = m_Color[1];
		m_LastColor[2] = m_Color[2];
	}

	m_TranslationCylinderGizmo->GetMaterial()->m_Prop->SetColor(col);
	m_TranslationCylinderGizmo->GetMaterial()->m_Prop->SetAmbient(0);
	m_TranslationCylinderGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
	m_TranslationCylinderGizmo->GetMaterial()->m_Prop->SetSpecular(0);
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
	mafEventMacro(mafEvent(this,VME_SHOW,m_TranslationCylinderGizmo,show));
}

//----------------------------------------------------------------------------
void medGizmoCrossTranslateAxis::ShowTranslationFeedbackArrows(bool show)
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,VME_SHOW,m_TranslationFeedbackGizmo,show));
}

//----------------------------------------------------------------------------
void medGizmoCrossTranslateAxis::SetAbsPose(mafMatrix *absPose)
//----------------------------------------------------------------------------
{
	m_TranslationCylinderGizmo->SetAbsMatrix(*absPose); 
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
	return m_TranslationCylinderGizmo->GetOutput()->GetAbsMatrix();
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

//----------------------------------------------------------------------------
void medGizmoCrossTranslateAxis::CreateFeedbackGizmoPipeline()
//----------------------------------------------------------------------------
{
	assert(m_InputVme);

	double bbDiagonal = m_InputVme->GetOutput()->GetVTKData()->GetLength();

	// cone origin in height / 2
	double coneHeight = bbDiagonal / 4;
	double coneRadius = bbDiagonal /7;
	double coneResolution = 40;
	double y = coneHeight;
	double x = bbDiagonal / 2;

	m_FeedbackCylinderSource = vtkCylinderSource::New();

	m_LeftFeedbackCylinderTransformPDF = vtkTransformPolyDataFilter::New();
	m_RightFeedbackCylinderTransformPDF = vtkTransformPolyDataFilter::New();

	m_FeedbackConeSource = vtkConeSource::New();

	m_LeftUpFeedbackConeTransformPDF = vtkTransformPolyDataFilter::New();
	m_LeftDownFeedbackConeTransformPDF = vtkTransformPolyDataFilter::New();
	m_RightUpFeedbackConeTransformPDF = vtkTransformPolyDataFilter::New();
	m_RightDownFeedbackConeTransformPDF = vtkTransformPolyDataFilter::New();

	// left stuff transform
	m_LeftFeedbackCylinderTransform = vtkTransform::New();
	m_LeftFeedbackCylinderTransform->PostMultiply();
	m_LeftFeedbackCylinderTransform->Translate(- x, 0,0);

	m_LeftUpFeedbackConeTransform = vtkTransform::New();
	m_LeftUpFeedbackConeTransform->PostMultiply();
	m_LeftUpFeedbackConeTransform->RotateZ(90);
	m_LeftUpFeedbackConeTransform->Translate(0,y, 0);
	m_LeftUpFeedbackConeTransform->Translate(- x, 0,0);

	m_LeftDownFeedbackConeTransform = vtkTransform::New();
	m_LeftDownFeedbackConeTransform->PostMultiply();
	m_LeftDownFeedbackConeTransform->RotateZ(-90);
	m_LeftDownFeedbackConeTransform->Translate(0, - y, 0);
	m_LeftDownFeedbackConeTransform->Translate(- x, 0,0);

	// right stuff transform
	m_RightFeedbackCylinderTransform = vtkTransform::New();
	m_RightFeedbackCylinderTransform->PostMultiply();
	m_RightFeedbackCylinderTransform->Translate(x, 0,0);

	m_RightDownFeedbackConeTransform = vtkTransform::New();
	m_RightDownFeedbackConeTransform->PostMultiply();
	m_RightDownFeedbackConeTransform->RotateZ(-90);
	m_RightDownFeedbackConeTransform->Translate(0, -y, 0);
	m_RightDownFeedbackConeTransform->Translate(x, 0,0);

	m_RightUpFeedbackConeTransform = vtkTransform::New();
	m_RightUpFeedbackConeTransform->PostMultiply();
	m_RightUpFeedbackConeTransform->RotateZ(90);
	m_RightUpFeedbackConeTransform->Translate(0, y, 0);
	m_RightUpFeedbackConeTransform->Translate(x, 0,0);


	m_TranslationFeedbackGizmo = mafVMEGizmo::New();

	m_FeedbackConeSource->SetResolution(coneResolution);
	m_FeedbackConeSource->SetHeight(coneHeight);
	m_FeedbackConeSource->SetRadius(coneRadius);
	m_FeedbackConeSource->Update();

	m_FeedbackCylinderSource->SetResolution(coneResolution);
	m_FeedbackCylinderSource->SetHeight(coneHeight);
	m_FeedbackCylinderSource->SetRadius(coneRadius / 2);
	m_FeedbackCylinderSource->Update();

	m_LeftFeedbackCylinderTransformPDF->SetInput(m_FeedbackCylinderSource->GetOutput());
	m_LeftFeedbackCylinderTransformPDF->SetTransform(m_LeftFeedbackCylinderTransform);

	m_RightFeedbackCylinderTransformPDF->SetInput(m_FeedbackCylinderSource->GetOutput());
	m_RightFeedbackCylinderTransformPDF->SetTransform(m_RightFeedbackCylinderTransform);

	m_LeftUpFeedbackConeTransformPDF->SetInput(m_FeedbackConeSource->GetOutput());
	m_LeftUpFeedbackConeTransformPDF->SetTransform(m_LeftUpFeedbackConeTransform);

	m_LeftDownFeedbackConeTransformPDF->SetInput(m_FeedbackConeSource->GetOutput());
	m_LeftDownFeedbackConeTransformPDF->SetTransform(m_LeftDownFeedbackConeTransform);

	m_RightUpFeedbackConeTransformPDF->SetInput(m_FeedbackConeSource->GetOutput());
	m_RightUpFeedbackConeTransformPDF->SetTransform(m_RightUpFeedbackConeTransform);

	m_RightDownFeedbackConeTransformPDF->SetInput(m_FeedbackConeSource->GetOutput());
	m_RightDownFeedbackConeTransformPDF->SetTransform(m_RightDownFeedbackConeTransform);

	m_FeedbackStuffAppendPolydata = vtkAppendPolyData::New();
	m_FeedbackStuffAppendPolydata->AddInput(m_LeftUpFeedbackConeTransformPDF->GetOutput());
	m_FeedbackStuffAppendPolydata->AddInput(m_LeftDownFeedbackConeTransformPDF->GetOutput());
	m_FeedbackStuffAppendPolydata->AddInput(m_RightUpFeedbackConeTransformPDF->GetOutput());
	m_FeedbackStuffAppendPolydata->AddInput(m_RightDownFeedbackConeTransformPDF->GetOutput());
	m_FeedbackStuffAppendPolydata->AddInput(m_LeftFeedbackCylinderTransformPDF->GetOutput());
	m_FeedbackStuffAppendPolydata->AddInput(m_RightFeedbackCylinderTransformPDF->GetOutput());
	m_FeedbackStuffAppendPolydata->Update();

	m_TranslationFeedbackGizmo->SetName("AxisTranslationFeedbackGizmo");
	m_TranslationFeedbackGizmo->SetMediator(m_Listener);
	m_TranslationFeedbackGizmo->SetData(m_FeedbackStuffAppendPolydata->GetOutput());
	//  m_TranslationFeedbackGizmo->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 1));
	assert(m_InputVme);

	m_TranslationFeedbackGizmo->GetMaterial()->m_Prop->SetColor(1,1,0);
	m_TranslationFeedbackGizmo->GetMaterial()->m_Prop->SetAmbient(0);
	m_TranslationFeedbackGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
	m_TranslationFeedbackGizmo->GetMaterial()->m_Prop->SetSpecular(0);
	m_TranslationFeedbackGizmo->GetMaterial()->m_Prop->SetOpacity(0.1);
	// ReparentTo will add also the gizmos to the tree
	m_TranslationFeedbackGizmo->ReparentTo(m_TranslationCylinderGizmo);
	// m_TranslationFeedbackArrowGizmo->ReparentTo(m_TranslationCylinderGizmo->GetRoot());
}