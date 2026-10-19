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
#include "albaVME.h"
#include "albaView.h"
#include "albaViewManager.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImageCalibration);

//----------------------------------------------------------------------------
albaOpImageCalibration::albaOpImageCalibration(const wxString &label)
	: albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_SelectedIterator = FREE_LINE;
	m_Units = 0;

	m_Distance = 0.0;
	m_Diameter = 0.0;
	m_Radius = 0.0;

	m_DistanceInteractor = NULL;
	m_CenterPointInteractor = NULL;
	m_ActiveInteractor = NULL;
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

	m_DistanceInteractor = albaInteractor2DMeasure_Distance::New();
	m_CenterPointInteractor = albaInteractor2DMeasure_CenterPoint::New();

	m_DistanceInteractor->SetListener(this);
	m_CenterPointInteractor->SetListener(this);

	m_ActiveInteractor = m_DistanceInteractor;
	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)m_ActiveInteractor));

	albaView *view = GetLogicManager()->GetViewManager()->GetSelectedView();
	if (view == NULL)
	{
		view = ((albaLogicWithManagers *)GetLogicManager())
			->GetViewManager()->ViewCreate(VIEW_START + 8);
	}

	if (view != NULL)
	{
		m_DistanceInteractor->SetRendererByView(view);
		m_CenterPointInteractor->SetRendererByView(view);
		GetLogicManager()->VmeShow(m_Input, true);
	}

	CreateGui();
	ShowGui();
}

//----------------------------------------------------------------------------
void albaOpImageCalibration::CreateGui()
{
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

	wxString iteratorChoices[4] = {"Free line", "Horizontal line", "Vertical line", "Circle" };

	wxString unitChoices[2] =	{ "Inches","Cm" };

	m_Gui->Combo(ID_ITERATOR, _("Iterator"), &m_SelectedIterator, 4,
		iteratorChoices);
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
}

//----------------------------------------------------------------------------
void albaOpImageCalibration::SetActiveInteractor()
{
	albaInteractor2DMeasure *nextInteractor = (m_SelectedIterator == CIRCLE) ? (albaInteractor2DMeasure *)m_CenterPointInteractor : m_DistanceInteractor;

	if (nextInteractor == m_ActiveInteractor)
		return;

	albaEventMacro(albaEvent(this, PER_POP));
	m_ActiveInteractor = nextInteractor;
	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)m_ActiveInteractor));
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

		case wxOK:
			OpStop(OP_RUN_OK);
			break;

		case wxCANCEL:
			OpStop(OP_RUN_CANCEL);
			break;
		}
	}
}

//----------------------------------------------------------------------------
void albaOpImageCalibration::OpStop(int result)
{
	if (m_Gui != NULL)
		HideGui();

	if (m_ActiveInteractor != NULL)
		albaEventMacro(albaEvent(this, PER_POP));

	albaDEL(m_DistanceInteractor);
	albaDEL(m_CenterPointInteractor);

	m_ActiveInteractor = NULL;

	albaEventMacro(albaEvent(this, result));
}