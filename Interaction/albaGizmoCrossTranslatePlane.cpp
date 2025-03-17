/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoCrossTranslatePlane
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaGizmoCrossTranslatePlane.h"
#include "albaDecl.h"

// isa stuff
#include "albaInteractorCompositorMouse.h"
#include "albaInteractorGenericMouse.h"
#include "albaRefSys.h"

// vme stuff
#include "mmaMaterial.h"
#include "albaVME.h"
#include "albaVMEGizmo.h"
#include "albaVMEOutput.h"

// vtk stuff
#include "vtkLineSource.h"
#include "vtkPlaneSource.h"
#include "vtkTubeFilter.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkProperty.h"
#include "albaGizmoPath.h"

#include "albaTagArray.h"
//----------------------------------------------------------------------------
albaGizmoCrossTranslatePlane::albaGizmoCrossTranslatePlane(albaVME *input, albaObserver *listener)
//----------------------------------------------------------------------------
{
	m_LastColor[0][S0] = -1;
	m_LastColor[1][S0] = -1;
	m_LastColor[2][S0] = -1;

	m_LastColor[0][S1] = -1;
	m_LastColor[1][S1] = -1;
	m_LastColor[2][S1] = -1;

	m_TranslationFeedbackGizmo = NULL;

	m_FeedbackConeSource = NULL;

	// feedback cone transform stuff
	m_LeftFeedbackConeTransform = NULL;
	m_RightFeedbackConeTransform = NULL;
	m_UpFeedbackConeTransform = NULL;
	m_DownFeedbackConeTransform = NULL;

	// feedback cone transform PDF
	m_LeftFeedbackConeTransformPDF = NULL;
	m_RightFeedbackConeTransformPDF = NULL;
	m_UpFeedbackConeTransformPDF = NULL;
	m_DownFeedbackConeTransformPDF = NULL;

	m_FeedbackCylinderSource = NULL;

	m_HorizontalFeedbackCylinderTransform = NULL;
	m_VerticalFeedbackCylinderTransform = NULL;

	m_VerticalFeedbackCylinderTransformPDF = NULL;
	m_HorizontalFeedbackCylinderTransformPDF = NULL;

	m_FeedbackStuffAppendPolydata = NULL;

	this->SetIsActive(false);

	m_IsaComp[0]  = m_IsaComp[1] =  NULL;
	m_Listener  = listener;
	m_InputVme    = input;
	m_Length = 1;

	// default plane is YZ
	m_ActivePlane = X_NORMAL;

	// TODO REFACTOR THIS: Isa Generic API cleanup 
	// pivot transform stuff in isa generic probably could be deleted with a minor refactor
	// 

	//-----------------
	// pivot stuff
	//-----------------
	// pivotTransform is useless for this operation but required by isa generic
	m_PivotTransform = vtkTransform::New();

	// create pipeline stuff
	CreatePipeline();

	// create isa stuff
	CreateISA();

	//-----------------
	// create vme gizmos stuff
	//-----------------
	albaString vmeName;
	int i;
	for (i = 0; i < NUM_GIZMO_PARTS; i++)
	{
		// the ith gizmo
		m_Gizmo[i] = albaVMEGizmo::New();
		vmeName = "part";
		vmeName << i;
		m_Gizmo[i]->SetName(vmeName.GetCStr());
		m_Gizmo[i]->SetDataConnection(m_RotatePDF[i]->GetOutputPort());
		m_Gizmo[i]->SetMediator(m_Listener);
	}
	// assign isa to S1 and S2;
	m_Gizmo[S0]->SetBehavior(m_IsaComp[S0]);
	m_Gizmo[S1]->SetBehavior(m_IsaComp[S1]);

	albaMatrix *absInputMatrix = m_InputVme->GetOutput()->GetAbsMatrix();
	SetAbsPose(absInputMatrix);
	SetConstrainRefSys(absInputMatrix);

	// add the gizmo to the tree, this should increase reference count 
	for (i = 0; i < NUM_GIZMO_PARTS; i++)
	{
		m_Gizmo[i]->ReparentTo(m_InputVme->GetRoot());
	}

	CreateFeedbackGizmoPipeline();
}
//----------------------------------------------------------------------------
albaGizmoCrossTranslatePlane::~albaGizmoCrossTranslatePlane() 
//----------------------------------------------------------------------------
{
	// set gizmo behavior to NULL
	m_Gizmo[S0]->SetBehavior(NULL);
	m_Gizmo[S1]->SetBehavior(NULL);

	vtkDEL(m_Line[S0]);
	vtkDEL(m_Line[S1]);
	vtkDEL(m_RotationTr);

	// clean up
	int i;
	for (i = 0; i < NUM_GIZMO_PARTS; i++)
	{
		vtkDEL(m_LineTF[i]);
		vtkDEL(m_IsaComp[i]); 
	}

	m_PivotTransform->Delete();

	for (i = 0; i < NUM_GIZMO_PARTS; i++)
	{
		vtkDEL(m_RotatePDF[i]);
		m_Gizmo[i]->ReparentTo(NULL);
	}

	vtkDEL(m_TranslationFeedbackGizmo);

	vtkDEL(m_FeedbackConeSource);


	vtkDEL(m_LeftFeedbackConeTransform);
	vtkDEL(m_RightFeedbackConeTransform);
	vtkDEL(m_UpFeedbackConeTransform);
	vtkDEL(m_DownFeedbackConeTransform);

	vtkDEL(m_LeftFeedbackConeTransformPDF);
	vtkDEL(m_RightFeedbackConeTransformPDF);
	vtkDEL(m_UpFeedbackConeTransformPDF);
	vtkDEL(m_DownFeedbackConeTransformPDF);

	vtkDEL(m_FeedbackCylinderSource);

	vtkDEL(m_HorizontalFeedbackCylinderTransform);
	vtkDEL(m_VerticalFeedbackCylinderTransform);

	vtkDEL(m_VerticalFeedbackCylinderTransformPDF);
	vtkDEL(m_HorizontalFeedbackCylinderTransformPDF);

	vtkDEL(m_FeedbackStuffAppendPolydata);

}

//----------------------------------------------------------------------------
void albaGizmoCrossTranslatePlane::CreatePipeline() 
//----------------------------------------------------------------------------
{
	// calculate diagonal of InputVme space bounds 
	double b[6],p1[3],p2[3],boundingBoxDiagonal;
	if(m_InputVme->IsA("albaVMEGizmo"))
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

	/*
	z
	^  S2
	(0,0,1) |----- (0,1,1)
	|     |         
	| SQ  |S1          
	|     |         
	x--------> y    

	(0,1,0)

	*/

	// create pipeline for cone-cylinder gizmo along global X axis

	// create S1
	m_Line[S0] = vtkLineSource::New();  
	m_Line[S0]->SetPoint1(0, 1, -1);
	m_Line[S0]->SetPoint2(0, 1, 1);

	// create S2
	m_Line[S1] = vtkLineSource::New();
	m_Line[S1]->SetPoint1(0, -1, 1);
	m_Line[S1]->SetPoint2(0, 1, 1);

	// create tube filter for the segments
	int i;
	for (i = 0; i < NUM_GIZMO_PARTS; i++)
	{
		m_LineTF[i] = vtkTubeFilter::New();
		m_LineTF[i]->SetInputConnection(m_Line[i]->GetOutputPort());

		double tubeRadius = boundingBoxDiagonal/250;
		m_LineTF[i]->SetRadius(tubeRadius);
		m_LineTF[i]->SetNumberOfSides(20);
	}

	//-----------------
	// update segments and square dimension based on vme bb diagonal
	//-----------------
	this->SetSizeLength(boundingBoxDiagonal / 4);

	//-----------------
	m_RotationTr = vtkTransform::New();
	m_RotationTr->Identity();

	// create rotation transform and rotation TPDF 
	for (i = 0; i < NUM_GIZMO_PARTS; i++)
	{
		m_RotatePDF[i] = vtkTransformPolyDataFilter::New();
		m_RotatePDF[i]->SetTransform(m_RotationTr);
		m_RotatePDF[i]->SetInputConnection(m_LineTF[i]->GetOutputPort());
	}
}

//----------------------------------------------------------------------------
void albaGizmoCrossTranslatePlane::CreateISA()
//----------------------------------------------------------------------------
{
	// Create isa compositor and assign behaviors to IsaGen ivar.
	// Default isa is constrained to plane XZ.
	for (int i = 0; i < NUM_GIZMO_PARTS; i++)
	{
		m_IsaComp[i] = albaInteractorCompositorMouse::New();

		// default behavior is activated by mouse left and is constrained to X axis,
		// default ref sys is input vme abs matrix
		m_IsaGen[i] = m_IsaComp[i]->CreateBehavior(MOUSE_LEFT);
		m_IsaGen[i]->SetVME(m_InputVme);

		// default movement is constrained to plane XZ 
		m_IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE);     

		//isa will send events to this
		m_IsaGen[i]->SetListener(this);
	}
}

//----------------------------------------------------------------------------
void albaGizmoCrossTranslatePlane::SetPlane(int plane) 
//----------------------------------------------------------------------------
{
	// this should be called when the translation gizmo
	// is created; gizmos are not highlighted

	// register the plane
	m_ActivePlane = plane;

	// rotate the gizmo components to match the specified plane
	if (m_ActivePlane == X_NORMAL)
	{
		// reset cyl and cone rotation
		m_RotationTr->Identity();

		// change the axis constrain
		for (int i = 0; i < NUM_GIZMO_PARTS; i++)
		{
			// move on X_NORMAL plane
			m_IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE, albaInteractorConstraint::FREE);
		}
	}
	else if (m_ActivePlane == Y_NORMAL)
	{
		// set rotation to move con and cyl on Y 
		m_RotationTr->Identity();
		m_RotationTr->RotateZ(-90);

		// change the axis constrain
		for (int i = 0; i < NUM_GIZMO_PARTS; i++)
		{
			// move on Y_NORMAL plane
			m_IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE);
		}
	}  
	else if (m_ActivePlane == Z_NORMAL)
	{
		// set rotation to move con and cyl on Z
		m_RotationTr->Identity();
		m_RotationTr->RotateY(90);

		// change the axis constrain
		for (int i = 0; i < NUM_GIZMO_PARTS; i++)
		{
			// move on Z_NORMAL plane
			m_IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
		}
	}  
}

//----------------------------------------------------------------------------
void albaGizmoCrossTranslatePlane::Highlight(bool highlight)
//----------------------------------------------------------------------------
{
	if (highlight == true)
	{

		// store last color

		// Highlight the S1 and S2  by setting its color to yellow 
		SetColor(S0, 1, 1, 0);
		SetColor(S1, 1, 1, 0);
	} 
	else
	{
		// TODO:restore previous color
		SetColor(S0, m_LastColor[0][S0],m_LastColor[1][S0], m_LastColor[2][S0]);
		SetColor(S1, m_LastColor[0][S1],m_LastColor[1][S1], m_LastColor[2][S1]);
	}
}

//----------------------------------------------------------------------------
void  albaGizmoCrossTranslatePlane::SetSizeLength(double length)
//----------------------------------------------------------------------------
{

	/*
	z
	^  S2
	(0,0,L) |----- (0,L,L)
	|     |         
	|     |S1          
	|     |         
	x--------> y    

	(0,L,0)
	*/

	// register the gizmo length
	m_Length = length;
	double L = length;
	// update S1
	m_Line[S0]->SetPoint1(0, 0, -L);
	m_Line[S0]->SetPoint2(0, 0, L);

	// update S2
	m_Line[S1]->SetPoint1(0, -L, 0);
	m_Line[S1]->SetPoint2(0, L, 0);

}
//----------------------------------------------------------------------------
void albaGizmoCrossTranslatePlane::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		// mouse down change gizmo selection status
		if (e->GetId() == ID_TRANSFORM)
		{
			if (e->GetArg() == albaInteractorGenericMouse::MOUSE_DOWN)
			{
				this->SetIsActive(true);
			}
		}
		// forward events to the listener
		e->SetSender(this);
		albaEventMacro(*e);
	}
}
//----------------------------------------------------------------------------
void albaGizmoCrossTranslatePlane::SetColor(int part, double col[3])
//----------------------------------------------------------------------------
{
	// set the color for a gizmo component
	// ie for S0 or S1 segment

	m_Color[0][part] = col[0];
	m_Color[1][part] = col[1];
	m_Color[2][part] = col[2];

	if (m_LastColor[0][part] == -1)
	{
		m_LastColor[0][part] = m_Color[0][part];
		m_LastColor[1][part] = m_Color[1][part];
		m_LastColor[2][part] = m_Color[2][part];

	}

	// TODO: add api to set single translation gizmos axis colors    
	m_Gizmo[part]->GetMaterial()->m_Prop->SetColor(col);
	m_Gizmo[part]->GetMaterial()->m_Prop->SetAmbient(0);
	m_Gizmo[part]->GetMaterial()->m_Prop->SetDiffuse(1);
	m_Gizmo[part]->GetMaterial()->m_Prop->SetSpecular(0);
}
//----------------------------------------------------------------------------
void albaGizmoCrossTranslatePlane::SetColor(int part, double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
	double col[3] = {colR, colG, colB};
	this->SetColor(part, col);
}
//----------------------------------------------------------------------------
void albaGizmoCrossTranslatePlane::Show(bool show)
//----------------------------------------------------------------------------
{
	// show gizmo components by issuing events
	for (int i = 0; i < NUM_GIZMO_PARTS; i++)
		GetLogicManager()->VmeShow(m_Gizmo[i], show);
}

//----------------------------------------------------------------------------
void albaGizmoCrossTranslatePlane::SetAbsPose(albaMatrix *absPose)
//----------------------------------------------------------------------------
{
	// set the abs pose to al gizmo components
	for (int i = 0; i < NUM_GIZMO_PARTS; i++)
	{  
		m_Gizmo[i]->SetAbsMatrix(*absPose);
	}

	// set the constrain ref sys for all gizmo components
	SetConstrainRefSys(absPose);
}
//----------------------------------------------------------------------------
void albaGizmoCrossTranslatePlane::SetConstrainRefSys(albaMatrix *constrain)
//----------------------------------------------------------------------------
{  
	for (int i = 0; i < NUM_GIZMO_PARTS; i++)
	{
		m_IsaGen[i]->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(constrain);
	}
}
//----------------------------------------------------------------------------
albaMatrix *albaGizmoCrossTranslatePlane::GetAbsPose()
//----------------------------------------------------------------------------
{
	// get the abs pose from a gizmo component
	return m_Gizmo[S0]->GetOutput()->GetAbsMatrix();
}
//----------------------------------------------------------------------------
void albaGizmoCrossTranslatePlane::SetInput(albaVME *vme)
//----------------------------------------------------------------------------
{
	// set gizmo pose and refsys from an input vme
	this->m_InputVme = vme; 
	SetAbsPose(vme->GetOutput()->GetAbsMatrix()); 
	SetConstrainRefSys(vme->GetOutput()->GetAbsMatrix());
}
//----------------------------------------------------------------------------
void albaGizmoCrossTranslatePlane::SetConstraintModality(int axis, int constrainModality)
//----------------------------------------------------------------------------
{

	for (int i = 0; i < NUM_GIZMO_PARTS; i++)
	{
		m_IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(axis,constrainModality);
	}
}
//----------------------------------------------------------------------------
void albaGizmoCrossTranslatePlane::SetStep(int axis, double step)
//----------------------------------------------------------------------------
{
	// set the translation constraint step
	for (int i = 0; i < NUM_GIZMO_PARTS; i++)
	{
		m_IsaGen[i]->GetTranslationConstraint()->SetStep(axis,step);
	}
}

//----------------------------------------------------------------------------
void albaGizmoCrossTranslatePlane::CreateFeedbackGizmoPipeline()
//----------------------------------------------------------------------------
{
	// build the vtk pipeline for feedback gizmo
	assert(m_InputVme);

	double bbDiagonal = m_InputVme->GetOutput()->GetVTKData()->GetLength();

	// cone origin in height / 2
	double coneHeight = bbDiagonal / 4;
	double coneRadius = bbDiagonal /7;
	double coneResolution = 40;
	double y = coneHeight;
	double x = bbDiagonal / 2;

	m_FeedbackCylinderSource = vtkCylinderSource::New();

	m_VerticalFeedbackCylinderTransformPDF = vtkTransformPolyDataFilter::New();
	m_HorizontalFeedbackCylinderTransformPDF = vtkTransformPolyDataFilter::New();

	m_FeedbackConeSource = vtkConeSource::New();

	m_LeftFeedbackConeTransformPDF = vtkTransformPolyDataFilter::New();
	m_RightFeedbackConeTransformPDF = vtkTransformPolyDataFilter::New();
	m_UpFeedbackConeTransformPDF = vtkTransformPolyDataFilter::New();
	m_DownFeedbackConeTransformPDF = vtkTransformPolyDataFilter::New();

	m_HorizontalFeedbackCylinderTransform = vtkTransform::New();
	m_HorizontalFeedbackCylinderTransform->PostMultiply();

	m_VerticalFeedbackCylinderTransform = vtkTransform::New();
	m_VerticalFeedbackCylinderTransform->PostMultiply();
	m_VerticalFeedbackCylinderTransform->RotateZ(90);

	m_LeftFeedbackConeTransform = vtkTransform::New();
	m_LeftFeedbackConeTransform->PostMultiply();
	m_LeftFeedbackConeTransform->RotateZ(180);
	m_LeftFeedbackConeTransform->Translate(- x, 0,0);

	m_RightFeedbackConeTransform = vtkTransform::New();
	m_RightFeedbackConeTransform->PostMultiply();
	m_RightFeedbackConeTransform->Translate(x, 0,0);

	m_UpFeedbackConeTransform = vtkTransform::New();
	m_UpFeedbackConeTransform->PostMultiply();
	m_UpFeedbackConeTransform->RotateZ(90);
	m_UpFeedbackConeTransform->Translate(0, x, 0);

	m_DownFeedbackConeTransform = vtkTransform::New();
	m_DownFeedbackConeTransform->PostMultiply();
	m_DownFeedbackConeTransform->RotateZ(-90);
	m_DownFeedbackConeTransform->Translate(0, -x, 0);

	m_TranslationFeedbackGizmo = albaVMEGizmo::New();

	m_FeedbackConeSource->SetResolution(coneResolution);
	m_FeedbackConeSource->SetHeight(coneHeight);
	m_FeedbackConeSource->SetRadius(coneRadius);
	m_FeedbackConeSource->Update();

	m_FeedbackCylinderSource->SetResolution(coneResolution);
	m_FeedbackCylinderSource->SetHeight(coneHeight);
	m_FeedbackCylinderSource->SetRadius(coneRadius / 2);
	m_FeedbackCylinderSource->Update();

	m_VerticalFeedbackCylinderTransformPDF->SetInputConnection(m_FeedbackCylinderSource->GetOutputPort());
	m_VerticalFeedbackCylinderTransformPDF->SetTransform(m_HorizontalFeedbackCylinderTransform);

	m_HorizontalFeedbackCylinderTransformPDF->SetInputConnection(m_FeedbackCylinderSource->GetOutputPort());
	m_HorizontalFeedbackCylinderTransformPDF->SetTransform(m_VerticalFeedbackCylinderTransform);

	m_LeftFeedbackConeTransformPDF->SetInputConnection(m_FeedbackConeSource->GetOutputPort());
	m_LeftFeedbackConeTransformPDF->SetTransform(m_LeftFeedbackConeTransform);

	m_RightFeedbackConeTransformPDF->SetInputConnection(m_FeedbackConeSource->GetOutputPort());
	m_RightFeedbackConeTransformPDF->SetTransform(m_RightFeedbackConeTransform);

	m_UpFeedbackConeTransformPDF->SetInputConnection(m_FeedbackConeSource->GetOutputPort());
	m_UpFeedbackConeTransformPDF->SetTransform(m_DownFeedbackConeTransform);

	m_DownFeedbackConeTransformPDF->SetInputConnection(m_FeedbackConeSource->GetOutputPort());
	m_DownFeedbackConeTransformPDF->SetTransform(m_UpFeedbackConeTransform);

	m_FeedbackStuffAppendPolydata = vtkAppendPolyData::New();
	m_FeedbackStuffAppendPolydata->AddInputConnection(m_LeftFeedbackConeTransformPDF->GetOutputPort());
	m_FeedbackStuffAppendPolydata->AddInputConnection(m_RightFeedbackConeTransformPDF->GetOutputPort());
	m_FeedbackStuffAppendPolydata->AddInputConnection(m_UpFeedbackConeTransformPDF->GetOutputPort());
	m_FeedbackStuffAppendPolydata->AddInputConnection(m_DownFeedbackConeTransformPDF->GetOutputPort());
	m_FeedbackStuffAppendPolydata->Update();

	m_TranslationFeedbackGizmo->SetName("PlaneTranslationFeedbackGizmo");
	m_TranslationFeedbackGizmo->SetMediator(m_Listener);
	m_TranslationFeedbackGizmo->SetDataConnection(m_FeedbackStuffAppendPolydata->GetOutputPort());
	assert(m_InputVme);

	m_TranslationFeedbackGizmo->GetMaterial()->m_Prop->SetColor(1,1,0);
	m_TranslationFeedbackGizmo->GetMaterial()->m_Prop->SetAmbient(0);
	m_TranslationFeedbackGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
	m_TranslationFeedbackGizmo->GetMaterial()->m_Prop->SetSpecular(0);
	m_TranslationFeedbackGizmo->GetMaterial()->m_Prop->SetOpacity(0.1);

	// ReparentTo will add also the gizmos to the tree
	m_TranslationFeedbackGizmo->ReparentTo(m_Gizmo[0]);
}

//----------------------------------------------------------------------------
void albaGizmoCrossTranslatePlane::ShowTranslationFeedbackArrows(bool show)
//----------------------------------------------------------------------------
{
	GetLogicManager()->VmeShow(m_TranslationFeedbackGizmo, show);
}
