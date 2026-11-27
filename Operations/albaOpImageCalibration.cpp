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

#include "albaPositionalEventRouter.h"
#include "albaOpImageCalibration.h"

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaTool2DMeasure.h"
#include "albaTool2DMeasureDistance.h"
#include "albaTool2DMeasureCenterPoint.h"
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

	m_DistanceTool = NULL;
	m_CenterPointTool = NULL;
	m_ActiveTool = NULL;

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

	albaNEW(m_DistanceTool);
	albaNEW(m_CenterPointTool);

	m_DistanceTool->SetMaxMeasures(1);
	m_CenterPointTool->SetMaxMeasures(1);


	m_ActiveTool = m_DistanceTool;
	GetPositionalEventRouter()->PushTool(m_ActiveTool);
	m_ActiveTool->SetListener(this);

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

	if (m_ActiveTool->GetMeasureCount() > 0)
	{
		double iterDist = 0;
		double guiDist = 0;
		if (isCircle)
		{
			iterDist = m_CenterPointTool->GetCircleRadius(0);
			guiDist = m_Radius;
		}
		else
		{
			iterDist = m_DistanceTool->GetDistance(0);
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
void albaOpImageCalibration::SetActiveTool()
{
	//on change interactor remove current measure
	m_ActiveTool->RemoveAllMeasures();


	//set constraint for distance interactor
	//constraint mode and gui order is the same so we can use m_SelectedIterator to set constraint 
	if (m_SelectedIterator != CIRCLE)
		m_DistanceTool->SetConstraint(m_SelectedIterator);
	
	albaTool2DMeasure *nextInteractor = (m_SelectedIterator == CIRCLE) ? (albaTool2DMeasure *)m_CenterPointTool : m_DistanceTool;
	if (nextInteractor == m_ActiveTool)
		return;

	m_ActiveTool->Show(false);

	GetPositionalEventRouter()->PopTool(m_ActiveTool);
	m_ActiveTool = nextInteractor;
	GetPositionalEventRouter()->PushTool(m_ActiveTool);
	m_ActiveTool->SetListener(this);
	m_ActiveTool->Show(true);

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
			SetActiveTool();
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
	else if (event->GetSender() == m_ActiveTool && event->GetId() == albaTool2DMeasure::ID_MEASURE_CHANGED)
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
			
				measuredDistance = m_CenterPointTool->GetCircleRadius(0);
				guiDistance = m_Radius;
		}
		else
		{
			measuredDistance = m_DistanceTool->GetDistance(0);
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

	if (m_ActiveTool != NULL)
		GetPositionalEventRouter()->PopTool(m_ActiveTool);

	albaDEL(m_DistanceTool);
	albaDEL(m_CenterPointTool);

	m_ActiveTool = NULL;

	NotifyOpEnd(result);
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
	GetViewManager()->CameraUpdate();
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
	GetViewManager()->CameraUpdate();
}