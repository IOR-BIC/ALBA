/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpClipSurface
 Authors: Paolo Quadrani
 
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
#include "albaDefines.h"

#include "albaOpImageCalibration.h"

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaInteractor2DMeasure.h"
#include "albaInteractor2DMeasure_Distance.h"
#include "albaInteractor2DMeasure_CenterPoint.h"
#include "albaLogicWithManagers.h"
#include "albaVMEImage.h"
#include "albaView.h"
#include "albaViewManager.h"
#include "vtkImageData.h"
#include "vtkMath.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImageCalibration);

enum DISTANCE_UNITS
{
	CENTIMETERS,
	INCHES
};

//----------------------------------------------------------------------------
albaOpImageCalibration::albaOpImageCalibration(const wxString &label)
	: albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_SelectedIterator = FREE_LINE;
	m_Units = CENTIMETERS;

	m_Distance = 0.0;
	m_Diameter = 0.0;
	m_Radius = 0.0;

	m_DistanceInteractor = NULL;
	m_CenterPointInteractor = NULL;
	m_ActiveInteractor = NULL;

	for (int i = 0; i < 3; i++)
	{
		m_InputSpacing[i] = 0.0;
		m_NewSpacing[i] = 0.0;
	}
}

//----------------------------------------------------------------------------
albaOpImageCalibration::~albaOpImageCalibration()
{
}

//----------------------------------------------------------------------------
albaOp *albaOpImageCalibration::Copy()
{
	return new albaOpImageCalibration(m_Label);
}

//----------------------------------------------------------------------------
bool albaOpImageCalibration::InternalAccept(albaVME *node)
{
	return node != NULL;
}

//----------------------------------------------------------------------------
void albaOpImageCalibration::OpRun()
{
	if (m_TestMode)
	{
		OpStop(OP_RUN_CANCEL);
		return;
	}

	vtkImageData *inputImage = vtkImageData::SafeDownCast(m_Input->GetOutput()->GetVTKData());
	if (inputImage != NULL)
		inputImage->GetSpacing(m_InputSpacing);

	m_DistanceInteractor = albaInteractor2DMeasure_Distance::New();
	m_CenterPointInteractor = albaInteractor2DMeasure_CenterPoint::New();

	m_DistanceInteractor->SetMaxMeasures(1);
	m_CenterPointInteractor->SetMaxMeasures(1);


	m_ActiveInteractor = m_DistanceInteractor;
	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)m_ActiveInteractor));
	m_ActiveInteractor->SetListener(this);

	CreateGui();
	ShowGui();
}

//----------------------------------------------------------------------------
void albaOpImageCalibration::CreateGui()
{
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

	wxString iteratorChoices[4] = {"Free line", "Horizontal line", "Vertical line", "Circle" };

	wxString unitChoices[2] =	{ "Cm", "Inches" };

	m_Gui->Radio(ID_ITERATOR, _("Iterator"), &m_SelectedIterator, 4, iteratorChoices);
	m_Gui->Label("");

	m_Gui->Radio(ID_UNITS, _("Units"), &m_Units, 2, unitChoices);

	m_Gui->Double(ID_DISTANCE, _("Distance"), &m_Distance, 0.0, 1000000.0);
	m_Gui->Double(ID_DIAMETER, _("Diameter"), &m_Diameter, 0.0, 1000000.0);
	m_Gui->Double(ID_RADIUS, _("Radius"), &m_Radius, 0.0, 1000000.0);

	m_Gui->OkCancel();


	UpdateGui();
}

//----------------------------------------------------------------------------
void albaOpImageCalibration::UpdateGui()
{
	const bool isCircle = m_SelectedIterator == CIRCLE;

	m_Gui->Enable(ID_DISTANCE, !isCircle);
	m_Gui->Enable(ID_DIAMETER, isCircle);
	m_Gui->Enable(ID_RADIUS, isCircle);

	if (m_ActiveInteractor->GetMeasureCount() > 0)
	{
		double iterDist = 0;
		double guiDist = 0;
		if (isCircle)
		{
			iterDist = m_CenterPointInteractor->GetCircleRadius(0);
			guiDist = m_Radius;
		}
		else
		{
			iterDist = m_DistanceInteractor->GetDistance(0);
			guiDist = m_Distance;
		}
		m_Gui->Enable(wxOK, iterDist != 0 && guiDist !=0);
	}
	else
	{
		m_Gui->Enable(wxOK, false);
	}

	m_Gui->Update();
}

//----------------------------------------------------------------------------
void albaOpImageCalibration::SetActiveInteractor()
{
	//on change interactor remove current measure
	m_ActiveInteractor->RemoveAllMeasures();


	//set constraint for distance interactor
	//constraint mode and gui order is the same so we can use m_SelectedIterator to set constraint 
	if (m_SelectedIterator != CIRCLE)
		m_DistanceInteractor->SetConstraint(m_SelectedIterator);
	
	albaInteractor2DMeasure *nextInteractor = (m_SelectedIterator == CIRCLE) ? (albaInteractor2DMeasure *)m_CenterPointInteractor : m_DistanceInteractor;
	if (nextInteractor == m_ActiveInteractor)
		return;

	m_ActiveInteractor->Show(false);

	albaEventMacro(albaEvent(this, PER_POP));
	m_ActiveInteractor = nextInteractor;
	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)m_ActiveInteractor));
	m_ActiveInteractor->SetListener(this);
	m_ActiveInteractor->Show(true);

	UpdateGui();
}

//----------------------------------------------------------------------------
void albaOpImageCalibration::OnEvent(albaEventBase *alba_event)
{
	albaEvent *event = albaEvent::SafeDownCast(alba_event);
	if (event == NULL)
		return;

	if (event->GetSender() == m_Gui)
	{
		switch (event->GetId())
		{
		case ID_ITERATOR:
			SetActiveInteractor();
			UpdateGui();
			break;
		case ID_DISTANCE:
			UpdateGui();
			break;
		case ID_RADIUS:
			m_Diameter = m_Radius * 2.0;
			UpdateGui();
			break;
		case ID_DIAMETER:
			m_Radius = m_Diameter / 2.0;
			UpdateGui();
			break;
		case wxOK:
			OpStop(OP_RUN_OK);
			break;

		case wxCANCEL:
			OpStop(OP_RUN_CANCEL);
			break;
		}
	}
	else if (event->GetSender() == m_ActiveInteractor && event->GetId() == albaInteractor2DMeasure::ID_MEASURE_CHANGED)
	{
		UpdateGui();
	}
}

//----------------------------------------------------------------------------
void albaOpImageCalibration::OpStop(int result)
{
	if (result == OP_RUN_OK)
	{
		double measuredDistance = 0.0;
		double guiDistance = 0.0;

		if (m_SelectedIterator == CIRCLE)
		{
			
				measuredDistance = m_CenterPointInteractor->GetCircleRadius(0);
				guiDistance = m_Radius;
		}
		else
		{
			measuredDistance = m_DistanceInteractor->GetDistance(0);
			guiDistance = m_Distance;
		}

		guiDistance *= m_Units == CENTIMETERS ? 10.0 : 25.4;

		if (measuredDistance > 0.0 && guiDistance > 0.0)
		{
			double spacingScale = guiDistance / measuredDistance;
			for (int i = 0; i < 3; i++)
				m_NewSpacing[i] = m_InputSpacing[i] * spacingScale;
		}
	}

	if (m_Gui != NULL)
		HideGui();

	if (m_ActiveInteractor != NULL)
		albaEventMacro(albaEvent(this, PER_POP));

	albaDEL(m_DistanceInteractor);
	albaDEL(m_CenterPointInteractor);

	m_ActiveInteractor = NULL;

	albaEventMacro(albaEvent(this, result));
}

//----------------------------------------------------------------------------
void albaOpImageCalibration::OpDo()
{
	vtkImageData *inputImage = vtkImageData::SafeDownCast(m_Input->GetOutput()->GetVTKData());
	if (inputImage == NULL)
		return;

	inputImage->SetSpacing(m_NewSpacing);
	inputImage->Modified();
	albaVMEImage *imageVME = albaVMEImage::SafeDownCast(m_Input);
	imageVME->SetData(inputImage, m_Input->GetTimeStamp());
	m_Input->Update();
	m_Input->GetOutput()->Update();

	GetLogicManager()->VmeVisualModeChanged(m_Input);
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaOpImageCalibration::OpUndo()
{
	vtkImageData *inputImage = vtkImageData::SafeDownCast(m_Input->GetOutput()->GetVTKData());
	if (inputImage == NULL)
		return;

	inputImage->SetSpacing(m_InputSpacing);
	inputImage->Modified();
	albaVMEImage *imageVME = albaVMEImage::SafeDownCast(m_Input);
	imageVME->SetData(inputImage, m_Input->GetTimeStamp());
	m_Input->Update();
	m_Input->GetOutput()->Update();
	
	GetLogicManager()->VmeVisualModeChanged(m_Input);
	GetLogicManager()->CameraUpdate();
}