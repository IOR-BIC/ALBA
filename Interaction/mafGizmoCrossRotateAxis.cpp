/*=========================================================================

 Program: MAF2
 Module: mafGizmoCrossRotateAxis
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafGizmoCrossRotateAxis.h"
// wxwin stuff
#include "wx/string.h"

#include "mafDecl.h"

// isa stuff
#include "mafInteractorCompositorMouse.h"
#include "mafInteractorGenericMouse.h"
#include "mafRefSys.h"

// vme stuff
#include "mmaMaterial.h"
#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafVMEGizmo.h"
#include "mafMatrix.h"
#include "mafTagArray.h"

// vtk stuff
#include "vtkDiskSource.h"
#include "vtkCleanPolyData.h"
#include "vtkTubeFilter.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkProperty.h"
#include "vtkLineSource.h"
#include "vtkAppendPolydata.h"
#include "mafGizmoCrossRotateFan.h"

//----------------------------------------------------------------------------
mafGizmoCrossRotateAxis::mafGizmoCrossRotateAxis(mafVME *input, mafObserver *listener)
//----------------------------------------------------------------------------
{
	m_GizmoDirection =NS;
	m_FGCircle = NULL;
	m_FGCleanCircle = NULL;
	m_FGCircleTubeFilter = NULL;
	m_FGRotationTransform = NULL;
	m_FGRotatePDF = NULL;
	m_FGCircleTF = NULL;
	m_FGRotationTr = NULL;

	m_FeedbackConeSource = NULL;

	m_LeftUpFeedbackConeTransform = NULL;
	m_LeftDownFeedbackConeTransform = NULL;
	m_RightDownFeedbackConeTransform = NULL;
	m_RightUpFeedbackConeTransform = NULL;

	m_LeftUpFeedbackConeTransformPDF = NULL;
	m_LeftDownFeedbackConeTransformPDF = NULL;
	m_RightUpFeedbackConeTransformPDF = NULL;
	m_RightDownFeedbackConeTransformPDF = NULL;

	m_LineSourceEast = NULL;
	m_LineSourceWest = NULL;
	m_LineSourceNorth = NULL;
	m_LineSourceSouth = NULL;

	m_LinesAppendPolyData = NULL;

	m_LinesCleanCircle = NULL;
	m_LinesRotationTransform = NULL; 
	m_LinesRotatePDF = NULL; 

	this->SetIsActive(false);

	// default axis is X
	m_RotationAxis = X;
	m_IsaComp = NULL;
	m_Radius = -1;

	m_Listener = listener;
	m_InputVme = input;

	//-----------------
	// create pipeline stuff
	CreatePipeline();

	// create isa stuff
	CreateISA();

	//-----------------
	// create vme gizmo stuff
	//-----------------

	// the circle gizmo
	m_GizmoCross = mafVMEGizmo::New();
	m_GizmoCross->SetName("rotate cross");
	m_GizmoCross->SetData(m_LinesRotatePDF->GetOutput());

	mafGizmoCrossRotateFan *rotateFan = NULL;
	rotateFan = dynamic_cast<mafGizmoCrossRotateFan *>(m_Listener);

	m_GizmoCross->SetMediator(rotateFan->GetMediator());

	// m_GizmoCross->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 1));

	// assign isa to S1 and S2;
	m_GizmoCross->SetBehavior(m_IsaComp);

	// set the axis to X axis
	this->SetRotationAxis(m_RotationAxis);

	m_AbsInputMatrix = m_InputVme->GetOutput()->GetAbsMatrix();
	m_InputVme->GetOutput()->Update();
	SetAbsPose(m_AbsInputMatrix);
	SetRefSysMatrix(m_AbsInputMatrix);

	// add the gizmo to the tree, this should increase reference count  
	m_GizmoCross->ReparentTo(mafVME::SafeDownCast(m_InputVme->GetRoot()));

	CreateFeedbackGizmoPipeline();
}
//----------------------------------------------------------------------------
mafGizmoCrossRotateAxis::~mafGizmoCrossRotateAxis() 
//----------------------------------------------------------------------------
{
	m_GizmoCross->SetBehavior(NULL);

	vtkDEL(m_LineSourceEast);
	vtkDEL(m_LineSourceWest);
	vtkDEL(m_LineSourceNorth);
	vtkDEL(m_LineSourceSouth);

	vtkDEL(m_LinesAppendPolyData);
	vtkDEL(m_LinesCleanCircle);
	vtkDEL(m_LinesRotationTransform); 
	vtkDEL(m_LinesRotatePDF); 
	//----------------------
	// No leaks so somebody is performing this...
	//----------------------
	vtkDEL(m_IsaComp); 

	m_GizmoCross->ReparentTo(NULL);

	vtkDEL(m_FGCircle);
	vtkDEL(m_FGCleanCircle);
	vtkDEL(m_FGCircleTubeFilter);
	vtkDEL(m_FGRotationTransform);
	vtkDEL(m_FGRotatePDF);
	vtkDEL(m_FGCircleTF);
	vtkDEL(m_FGRotationTr);

	vtkDEL(m_FeedbackConeSource);

	vtkDEL(m_LeftUpFeedbackConeTransform);
	vtkDEL(m_LeftDownFeedbackConeTransform);
	vtkDEL(m_RightDownFeedbackConeTransform);
	vtkDEL(m_RightUpFeedbackConeTransform);

	vtkDEL(m_LeftUpFeedbackConeTransformPDF);
	vtkDEL(m_LeftDownFeedbackConeTransformPDF);
	vtkDEL(m_RightUpFeedbackConeTransformPDF);
	vtkDEL(m_RightDownFeedbackConeTransformPDF);
}
//----------------------------------------------------------------------------
void mafGizmoCrossRotateAxis::CreatePipeline() 
//----------------------------------------------------------------------------
{
	// calculate diagonal of InputVme space bounds 
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

	double min = boundingBoxDiagonal/2;
	double max = boundingBoxDiagonal * 10;
	// create line

	m_LineSourceEast = vtkLineSource::New();
	m_LineSourceWest = vtkLineSource::New();

	m_LineSourceEast->SetPoint1(min,0,0);
	m_LineSourceEast->SetPoint2(max,0,0);

	m_LineSourceWest->SetPoint1(-min,0,0);
	m_LineSourceWest->SetPoint2(-max,0,0);

	m_LineSourceNorth = vtkLineSource::New();
	m_LineSourceSouth = vtkLineSource::New();

	m_LineSourceNorth->SetPoint1(0,min,0);
	m_LineSourceNorth->SetPoint2(0,max,0);

	m_LineSourceSouth->SetPoint1(0,-min,0);
	m_LineSourceSouth->SetPoint2(0,-max,0);

	m_LinesAppendPolyData = vtkAppendPolyData::New();

	if (m_GizmoDirection = EW)
	{
		m_LinesAppendPolyData->AddInput(m_LineSourceEast->GetOutput());
		m_LinesAppendPolyData->AddInput(m_LineSourceWest->GetOutput());
	}
	else if (m_GizmoDirection = NS)
	{
		m_LinesAppendPolyData->AddInput(m_LineSourceNorth->GetOutput());
		m_LinesAppendPolyData->AddInput(m_LineSourceSouth->GetOutput());
	}

	m_Radius = boundingBoxDiagonal / 2;

	// clean the circle polydata
	m_LinesCleanCircle = vtkCleanPolyData::New();
	m_LinesCleanCircle->SetInput(m_LinesAppendPolyData->GetOutput());

	// tube filter the circle 
	m_LinesTubeFilter = vtkTubeFilter::New();
	m_LinesTubeFilter->SetInput(m_LinesCleanCircle->GetOutput());

	double tubeRadius = boundingBoxDiagonal / 250;

	m_LinesTubeFilter->SetRadius(tubeRadius);
	m_LinesTubeFilter->SetNumberOfSides(20);

	// create rotation transform and rotation TPDF
	m_LinesRotationTransform = vtkTransform::New();
	m_LinesRotationTransform->Identity();

	m_LinesRotatePDF = vtkTransformPolyDataFilter::New();
	m_LinesRotatePDF->SetTransform(m_LinesRotationTransform);
	m_LinesRotatePDF->SetInput(m_LinesTubeFilter->GetOutput());

}
//----------------------------------------------------------------------------
void mafGizmoCrossRotateAxis::CreateISA()
//----------------------------------------------------------------------------
{
	// Create isa compositor and assign behaviors to IsaGen ivar.
	// Default isa constrain rotation around X axis.
	m_IsaComp = mafInteractorCompositorMouse::New();

	// default behavior is activated by mouse left and is constrained to X axis,
	// default ref sys is input vme abs matrix
	m_IsaGen = m_IsaComp->CreateBehavior(MOUSE_LEFT);
	m_IsaGen->SetVME(m_InputVme);
	m_IsaGen->GetRotationConstraint()->SetConstraintModality(mafInteractorConstraint::FREE, mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK); 
	m_IsaGen->GetRotationConstraint()->GetRefSys()->SetTypeToCustom(m_AbsInputMatrix);
	m_IsaGen->GetPivotRefSys()->SetTypeToCustom(m_AbsInputMatrix);
	m_IsaGen->EnableRotation(true);

	//isa will send events to this
	m_IsaGen->SetListener(this);
}
//----------------------------------------------------------------------------
void mafGizmoCrossRotateAxis::SetRotationAxis(int axis) 
//----------------------------------------------------------------------------
{
	// this should be called when the gizmo
	// is created; gizmos are not highlighted

	// register the axis
	m_RotationAxis = axis;

	// rotate the gizmo components to match the specified axis
	if (m_RotationAxis == X)
	{
		// set rotation to move gizmo normal to X
		m_LinesRotationTransform->Identity();
		m_LinesRotationTransform->RotateY(90);

		// change the axis constrain  
		m_IsaGen->GetRotationConstraint()->SetConstraintModality(mafInteractorConstraint::FREE, mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK);
	}
	else if (m_RotationAxis == Y)
	{
		// set rotation to move gizmo normal to Y 
		m_LinesRotationTransform->Identity();
		m_LinesRotationTransform->RotateX(90);

		// change the Gizmo constrain
		m_IsaGen->GetRotationConstraint()->SetConstraintModality(mafInteractorConstraint::LOCK, mafInteractorConstraint::FREE, mafInteractorConstraint::LOCK);
	}
	else if (m_RotationAxis == Z)
	{
		// reset circle orientation to move gizmo normal to Z
		m_LinesRotationTransform->Identity();

		// change the Gizmo constrain
		m_IsaGen->GetRotationConstraint()->SetConstraintModality(mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK, mafInteractorConstraint::FREE);
	}    
}
//----------------------------------------------------------------------------
void mafGizmoCrossRotateAxis::Highlight(bool highlight)
//----------------------------------------------------------------------------
{
	if (highlight == true)
	{
		// Highlight the circle by setting its color to yellow 
		
		m_LastColor[0] = m_Color[0];
        m_LastColor[1] = m_Color[1];
        m_LastColor[2] = m_Color[2];

		this->SetColor(1, 1, 0);
		ShowTranslationFeedbackArrows(true);
	} 
	else
	{

		// TODO: restore original colors
		ShowTranslationFeedbackArrows(false);
		this->SetColor(m_LastColor);
	}
}

//----------------------------------------------------------------------------
void mafGizmoCrossRotateAxis::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	// mouse down change gizmo selection status
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		if (e->GetId() == ID_TRANSFORM)
		{
			if (e->GetArg() == mafInteractorGenericMouse::MOUSE_DOWN)
			{
				this->SetIsActive(true);
			}
			// forward events to the listener
			e->SetSender(this);
			mafEventMacro(*e);
		}
		else
		{
			mafEventMacro(*e);
		}
	}
}
/** Gizmo color */
//----------------------------------------------------------------------------
void mafGizmoCrossRotateAxis::SetColor(double col[3])
//----------------------------------------------------------------------------
{
	m_Color[0] = col[0];
	m_Color[1] = col[1];
	m_Color[2] = col[2];

	m_GizmoCross->GetMaterial()->m_Prop->SetColor(col);
	m_GizmoCross->GetMaterial()->m_Prop->SetAmbient(0);
	m_GizmoCross->GetMaterial()->m_Prop->SetDiffuse(1);
	m_GizmoCross->GetMaterial()->m_Prop->SetSpecular(0);
}
//----------------------------------------------------------------------------
void mafGizmoCrossRotateAxis::SetColor(double colR, double colG, double colB)
//----------------------------------------------------------------------------
{  
	double col[3] = {colR, colG, colB};
	this->SetColor(col);
}
//----------------------------------------------------------------------------
void mafGizmoCrossRotateAxis::Show(bool show)
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,VME_SHOW,m_GizmoCross,show));
}
//----------------------------------------------------------------------------
void mafGizmoCrossRotateAxis::SetAbsPose(mafMatrix *absPose)
//----------------------------------------------------------------------------
{
	m_GizmoCross->SetAbsMatrix(*absPose);
	SetRefSysMatrix(absPose);
}
//----------------------------------------------------------------------------
void mafGizmoCrossRotateAxis::SetRefSysMatrix(mafMatrix *matrix)
//----------------------------------------------------------------------------
{  
	m_IsaGen->GetRotationConstraint()->GetRefSys()->SetTypeToCustom(matrix);
	m_IsaGen->GetPivotRefSys()->SetTypeToCustom(matrix);
}
//----------------------------------------------------------------------------
mafMatrix *mafGizmoCrossRotateAxis::GetAbsPose()
//----------------------------------------------------------------------------
{
	return m_GizmoCross->GetOutput()->GetAbsMatrix();
}
//----------------------------------------------------------------------------
void mafGizmoCrossRotateAxis::SetInput(mafVME *vme)
//----------------------------------------------------------------------------
{
	this->m_InputVme = vme; 
	SetAbsPose(vme->GetOutput()->GetAbsMatrix()); 
}
//---------------------------------------------------------------------------
mafInteractorGenericInterface *mafGizmoCrossRotateAxis::GetInteractor()
//----------------------------------------------------------------------------
{
	return m_IsaGen;
}

int mafGizmoCrossRotateAxis::GetAxis()
{
	return m_RotationAxis;
}

double mafGizmoCrossRotateAxis::GetRadius()
{
	return m_Radius;
}

void mafGizmoCrossRotateAxis::SetListener( mafObserver *Listener )
{
	m_Listener = Listener;
}

void mafGizmoCrossRotateAxis::SetIsActive( bool isActive )
{
	m_IsActive = isActive;
}

bool mafGizmoCrossRotateAxis::GetIsActive()
{
	return m_IsActive;
}

mafVME * mafGizmoCrossRotateAxis::GetInput()
{
	return this->m_InputVme;
}


void mafGizmoCrossRotateAxis::CreateFeedbackGizmoPipeline()
{
	assert(m_InputVme);

	double bbDiagonal = m_InputVme->GetOutput()->GetVTKData()->GetLength();

	// cone origin in height / 2
	double coneHeight = bbDiagonal / 4;
	double coneRadius = bbDiagonal /7;
	double coneResolution = 40;
	double y = coneHeight;
	double x = bbDiagonal / 2;

	m_FeedbackConeSource = vtkConeSource::New();

	m_LeftUpFeedbackConeTransformPDF = vtkTransformPolyDataFilter::New();
	m_LeftDownFeedbackConeTransformPDF = vtkTransformPolyDataFilter::New();
	m_RightUpFeedbackConeTransformPDF = vtkTransformPolyDataFilter::New();
	m_RightDownFeedbackConeTransformPDF = vtkTransformPolyDataFilter::New();

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

	m_FeedbackConeSource->SetResolution(coneResolution);
	m_FeedbackConeSource->SetHeight(coneHeight);
	m_FeedbackConeSource->SetRadius(coneRadius);
	m_FeedbackConeSource->Update();

	m_LeftUpFeedbackConeTransformPDF->SetInput(m_FeedbackConeSource->GetOutput());
	m_LeftUpFeedbackConeTransformPDF->SetTransform(m_LeftUpFeedbackConeTransform);

	m_LeftDownFeedbackConeTransformPDF->SetInput(m_FeedbackConeSource->GetOutput());
	m_LeftDownFeedbackConeTransformPDF->SetTransform(m_LeftDownFeedbackConeTransform);

	m_RightUpFeedbackConeTransformPDF->SetInput(m_FeedbackConeSource->GetOutput());
	m_RightUpFeedbackConeTransformPDF->SetTransform(m_RightUpFeedbackConeTransform);

	m_RightDownFeedbackConeTransformPDF->SetInput(m_FeedbackConeSource->GetOutput());
	m_RightDownFeedbackConeTransformPDF->SetTransform(m_RightDownFeedbackConeTransform);

	// create circle
	m_FGCircle = vtkDiskSource::New();
	m_FGCircle->SetCircumferentialResolution(200);
	m_FGCircle->SetInnerRadius(x);
	m_FGCircle->SetOuterRadius(x);

	// clean the circle polydata
	m_FGCleanCircle = vtkCleanPolyData::New();
	m_FGCleanCircle->SetInput(m_FGCircle->GetOutput());

	// tube filter the circle 
	m_FGCircleTF = vtkTubeFilter::New();
	m_FGCircleTF->SetInput(m_FGCleanCircle->GetOutput());
	m_FGCircleTF->SetRadius(x / 8);
	m_FGCircleTF->SetNumberOfSides(20);

	// create rotation transform and rotation TPDF
	m_FGRotationTr = vtkTransform::New();
	m_FGRotationTr->Identity();

	m_FGRotatePDF = vtkTransformPolyDataFilter::New();
	m_FGRotatePDF->SetTransform(m_FGRotationTr);
	m_FGRotatePDF->SetInput(m_FGCircleTF->GetOutput());

	m_FeedbackStuffAppendPolydata = vtkAppendPolyData::New();
	//   m_FeedbackStuffAppendPolydata->AddInput(m_LeftUpFeedbackConeTransformPDF->GetOutput());
	//   m_FeedbackStuffAppendPolydata->AddInput(m_LeftDownFeedbackConeTransformPDF->GetOutput());
	//   m_FeedbackStuffAppendPolydata->AddInput(m_RightUpFeedbackConeTransformPDF->GetOutput());
	//   m_FeedbackStuffAppendPolydata->AddInput(m_RightDownFeedbackConeTransformPDF->GetOutput());
	m_FeedbackStuffAppendPolydata->AddInput(m_FGRotatePDF->GetOutput());
	m_FeedbackStuffAppendPolydata->Update();

	m_RotationFeedbackGizmo = mafVMEGizmo::New();
	m_RotationFeedbackGizmo->SetName("AxisRotationFeedbackGizmo");

	mafGizmoCrossRotateFan *rotateFan = NULL;
	rotateFan = dynamic_cast<mafGizmoCrossRotateFan *>(m_Listener);

	m_RotationFeedbackGizmo->SetMediator(rotateFan->GetMediator());
	m_RotationFeedbackGizmo->SetData(m_FeedbackStuffAppendPolydata->GetOutput());
	//  m_RotationFeedbackGizmo->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 1));
	assert(m_InputVme);

	m_RotationFeedbackGizmo->GetMaterial()->m_Prop->SetColor(1,1,0);
	m_RotationFeedbackGizmo->GetMaterial()->m_Prop->SetAmbient(0);
	m_RotationFeedbackGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
	m_RotationFeedbackGizmo->GetMaterial()->m_Prop->SetSpecular(0);
	m_RotationFeedbackGizmo->GetMaterial()->m_Prop->SetOpacity(0.1);

	// ReparentTo will add also the gizmos to the tree
	m_RotationFeedbackGizmo->ReparentTo(m_GizmoCross);

}

void mafGizmoCrossRotateAxis::ShowTranslationFeedbackArrows(bool show)
{
	mafEventMacro(mafEvent(this,VME_SHOW,m_RotationFeedbackGizmo,show));
}

//----------------------------------------------------------------------------
void mafGizmoCrossRotateAxis::SetGizmoDirection(int direction) 
//----------------------------------------------------------------------------
{
	// register the direction
	m_GizmoDirection = direction;

	m_LinesAppendPolyData->RemoveAllInputs();

	if (m_GizmoDirection == EW)
	{
		m_LinesAppendPolyData->AddInput(m_LineSourceEast->GetOutput());
		m_LinesAppendPolyData->AddInput(m_LineSourceWest->GetOutput());
	}
	else if (m_GizmoDirection == NS)
	{
		m_LinesAppendPolyData->AddInput(m_LineSourceNorth->GetOutput());
		m_LinesAppendPolyData->AddInput(m_LineSourceSouth->GetOutput());
	}

	m_LinesAppendPolyData->Update();
}
