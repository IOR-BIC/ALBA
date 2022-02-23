/*=========================================================================
Program:	 ALBA (Agile Library for Biomedical Applications)
Module:    albaOpMeasure2D.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the alba must include "albaDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "albaOpMeasure2D.h"
#include "albaDecl.h"

#include "albaInteractor2DMeasure_Point.h"
#include "albaInteractor2DMeasure_Distance.h"
#include "albaInteractor2DMeasure_MidPoint.h"
#include "albaInteractor2DMeasure_CenterPoint.h"
#include "albaInteractor2DMeasure_Line.h"
#include "albaInteractor2DMeasure_LineDistance.h"
#include "albaInteractor2DMeasure_Angle.h"
#include "albaInteractor2DMeasure_AngleLine.h"

#include "albaEvent.h"
#include "albaGUI.h"
#include "albaGUIValidator.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaLogicWithManagers.h"
#include "albaServiceClient.h"
#include "albaViewManager.h"
#include "albaView.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpMeasure2D);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpMeasure2D::albaOpMeasure2D(const wxString &label) :
albaOp(label)
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;

	m_InteractorVector.clear();
	m_SelectedInteractor = m_CurrentInteractor = INTERACTION_TYPE::POINT;

	m_MeasureListBox = NULL;

	m_SelectedMeasure = -1;
	m_Measure = "";
	m_MeasureLabel = "";
	m_MeasureType = "";

	m_MaxMeasures = -1; // Infinite
}
//----------------------------------------------------------------------------
albaOpMeasure2D::~albaOpMeasure2D()
{
}

//----------------------------------------------------------------------------
albaOp* albaOpMeasure2D::Copy()
{
	return (new albaOpMeasure2D(m_Label));
}

//----------------------------------------------------------------------------
bool albaOpMeasure2D::InternalAccept(albaVME *node)
{
	return node;
}

//----------------------------------------------------------------------------
char ** albaOpMeasure2D::GetIcon()
{
#include "pic/MENU_OP_MEASURE_2D.xpm"
	return MENU_OP_MEASURES_2D_xpm;
}

//----------------------------------------------------------------------------
void albaOpMeasure2D::OpRun()
{
	// Open View if necessary
	albaView *view = OpenImageView(VIEW_START + 8);
	GetLogicManager()->VmeShow(m_Input, true);

	InitInteractors();

	//////////////////////////////////////////////////////////////////////////
	
	if (!m_TestMode)
	{
		CreateGui();
		ShowGui();
		
		if (view)
		{
			for (int i = 0; i < m_InteractorVector.size(); i++)
				m_InteractorVector[i]->SetRendererByView(view);

			Load();
		}
	}
	else
	{
		OpStop(OP_RUN_CANCEL);
	}
}

//----------------------------------------------------------------------------
albaView* albaOpMeasure2D::OpenImageView(int viewId)
{
	albaView *currentView = NULL;
	albaString viewLabel = "Image";

	currentView = ((albaLogicWithManagers*)GetLogicManager())->GetViewManager()->GetFromList(viewLabel);;

	if (!currentView)
	{
		currentView = ((albaLogicWithManagers*)GetLogicManager())->GetViewManager()->ViewCreate(viewId);
		currentView->GetFrame()->SetMaxSize(currentView->GetFrame()->GetSize());
	}

	currentView->GetFrame()->SetFocus();

	return currentView;
}

//----------------------------------------------------------------------------
void albaOpMeasure2D::InitInteractors()
{
	// Create Interactor Point
	albaInteractor2DMeasure_Point *InteractorPoint = albaInteractor2DMeasure_Point::New();
	if (m_CurrentInteractor == POINT) albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)InteractorPoint));
	InteractorPoint->SetListener(this);
	m_InteractorVector.push_back(InteractorPoint);
	
	// Create Interactor Distance
	albaInteractor2DMeasure_Distance *InteractorDistance = albaInteractor2DMeasure_Distance::New();
	if (m_CurrentInteractor == DISTANCE)	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)InteractorDistance));
	InteractorDistance->SetListener(this);
	m_InteractorVector.push_back(InteractorDistance);

	// Create Interactor MidPoint
	albaInteractor2DMeasure_MidPoint *InteractorMidPoint = albaInteractor2DMeasure_MidPoint::New();
	if (m_CurrentInteractor == MID_POINT)	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)InteractorMidPoint));
	InteractorMidPoint->SetListener(this);
	m_InteractorVector.push_back(InteractorMidPoint);

	// Create Interactor CenterPoint
	albaInteractor2DMeasure_CenterPoint *InteractorCenterPoint = albaInteractor2DMeasure_CenterPoint::New();
	if (m_CurrentInteractor == CENTER_POINT)	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)InteractorCenterPoint));
	InteractorCenterPoint->SetListener(this);
	m_InteractorVector.push_back(InteractorCenterPoint);

	// Create Interactor Line
	albaInteractor2DMeasure_Line *InteractorLine = albaInteractor2DMeasure_Line::New();
	if (m_CurrentInteractor == LINE)	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)InteractorLine));
	InteractorLine->SetListener(this);
	m_InteractorVector.push_back(InteractorLine);

	// Create Interactor LineDistance
	albaInteractor2DMeasure_LineDistance *InteractorLineDist = albaInteractor2DMeasure_LineDistance::New();
	if (m_CurrentInteractor == LINE_DISTANCE)	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)InteractorLineDist));
	InteractorLineDist->SetListener(this);
	m_InteractorVector.push_back(InteractorLineDist);

	// Create Interactor Angle
	albaInteractor2DMeasure_Angle *InteractorAngle = albaInteractor2DMeasure_Angle::New();
	if (m_CurrentInteractor == ANGLE)	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)InteractorAngle));
	InteractorAngle->SetListener(this);
	m_InteractorVector.push_back(InteractorAngle);

	// Create Interactor Angle by Line
	albaInteractor2DMeasure_AngleLine *InteractorAngleLine = albaInteractor2DMeasure_AngleLine::New();
	if (m_CurrentInteractor == ANGLE_LINE)	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)InteractorAngleLine));
	InteractorAngleLine->SetListener(this);
	m_InteractorVector.push_back(InteractorAngleLine);

	m_MaxMeasures = m_InteractorVector[m_CurrentInteractor]->GetMaxMeasures();
	m_MeasureType = m_InteractorVector[m_CurrentInteractor]->GetMeasureType();
}

//----------------------------------------------------------------------------
void albaOpMeasure2D::CreateGui()
{
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

	m_Gui->Divider(1);
	int const numInter = 8;
	wxString choises[numInter] = { _("Point"),_("Distance"),_("MidPoint"),_("CenterPoint"),_("Line"),_("Line Distance"),_("Angle"),_("Angle by Line") };
	m_Gui->Combo(ID_SELECT_INTERACTOR, "", &m_SelectedInteractor, numInter, choises, "Select Measure Type");
	
	m_MeasureListBox = m_Gui->ListBox(ID_MEASURE_LIST, "", 200);
	m_Gui->Divider();
	m_Gui->Button(ID_REMOVE_MEASURE, _("Remove"));
	m_Gui->Integer(ID_MEASURE_MAX, _("Max"), &m_MaxMeasures,-1, 1000, "-1 = Infinite");

	m_Gui->Divider(1);
	m_Gui->Label("Measure", true);
	m_Gui->String(ID_MEASURE, "Type", &m_MeasureType);
	m_Gui->Integer(ID_MEASURE, "Selected", &m_SelectedMeasure);
	m_Gui->String(ID_MEASURE, "Measure", &m_Measure);
	m_Gui->String(ID_MEASURE_LAB, "Label", &m_MeasureLabel);

	m_Gui->Divider(1);
	m_Gui->Bool(ID_MEASURE_ENABLE, "Enable", &m_Enable);
	m_Gui->Bool(ID_MEASURE_EDIT, "Edit", &m_Edit);
	m_Gui->Bool(ID_MEASURE_MOVE, "Move", &m_Move);
	m_Gui->Bool(ID_SHOW_TEXT, "Show Text", &m_ShowText);

	m_Gui->Divider(1);
	m_Gui->Label("Storage", true);
	m_Gui->TwoButtons(ID_LOAD_MEASURES, ID_SAVE_MEASURES, "Load", "Save");

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Enable(ID_MEASURE, false);
	m_Gui->Enable(ID_REMOVE_MEASURE, m_MeasureListBox->GetCount() != 0);

	m_Gui->Label("");
	m_Gui->Label("");
	m_Gui->Divider(1);

	m_Gui->OkCancel();
	m_Gui->Label("");
}

//----------------------------------------------------------------------------
void albaOpMeasure2D::OpStop(int result)
{
	if (!m_TestMode)
	{
		m_MeasureListBox->Clear();
		HideGui();
	}
	
	// Remove Interactor
	albaEventMacro(albaEvent(this, PER_POP));

	for (int i = 0; i < m_InteractorVector.size(); i++)
		albaDEL(m_InteractorVector[i]);

	m_InteractorVector.clear();

	albaEventMacro(albaEvent(this, result));
}

//----------------------------------------------------------------------------
void albaOpMeasure2D::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		if (e->GetSender() == m_Gui) // FROM GUI
		{
			switch (e->GetId())
			{
			case ID_SELECT_INTERACTOR:
			{
				SetMeasureInteractor(m_SelectedInteractor);
				m_SelectedMeasure = (m_InteractorVector[m_CurrentInteractor]->GetMeasureCount() > 0) ? 0 : -1;
				m_Gui->Update();
			}
			break;
			case ID_MEASURE_LIST:
			{
				int selection = m_MeasureListBox->GetSelection();
				m_InteractorVector[m_CurrentInteractor]->SelectMeasure(selection);
				m_Gui->Update();
			}
			break;

			case ID_MEASURE_MAX:
			{
				int selection = m_MeasureListBox->GetSelection();
				m_InteractorVector[m_CurrentInteractor]->SetMaxMeasures(m_MaxMeasures);
				m_Gui->Update();
			}
			break;
			
			case ID_MEASURE_LAB:
			{
				int nMeasures = m_InteractorVector[m_CurrentInteractor]->GetMeasureCount();
				if (nMeasures > 0 && m_SelectedMeasure >= 0 && m_SelectedMeasure < nMeasures)
				{
					m_InteractorVector[m_CurrentInteractor]->SetMeasureLabel(m_SelectedMeasure, m_MeasureLabel);
					m_InteractorVector[m_CurrentInteractor]->Update(m_SelectedMeasure);
					UpdateMeasureList();
					m_Gui->Update();
				}
			}
			break;

			case ID_MEASURE_ENABLE:
			case ID_MEASURE_EDIT:
			case ID_MEASURE_MOVE:
			case ID_SHOW_TEXT:
			{
				int selection = m_MeasureListBox->GetSelection();
			
				m_InteractorVector[m_CurrentInteractor]->Enable(m_Enable);
				m_InteractorVector[m_CurrentInteractor]->EnableEditMeasure(m_Edit);
				m_InteractorVector[m_CurrentInteractor]->EnableMoveMeasure(m_Move);
				m_InteractorVector[m_CurrentInteractor]->ShowText(m_ShowText);
				m_InteractorVector[m_CurrentInteractor]->Update();
			}
			break;

			case ID_REMOVE_MEASURE:
			{
				RemoveMeasure();
				m_Gui->Update();
			}
			break;

			case ID_LOAD_MEASURES:
			{
				RemoveMeasure(true);
				Load();
				m_Gui->Update();
			}
			break;
			case ID_SAVE_MEASURES:
			{
				Save();
				m_Gui->Update();
			}
			break;

			case wxOK:
				OpStop(OP_RUN_OK);
				break;

			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
				break;
			}
		}
		else
		{
			switch (e->GetId()) // FROM INTERACTOR
			{
			case albaInteractor2DMeasure::ID_MEASURE_ADDED:
			case albaInteractor2DMeasure::ID_MEASURE_CHANGED:
			{
				// Update Measure Gui Entry
				m_SelectedMeasure = m_InteractorVector[m_CurrentInteractor]->GetSelectedMeasureIndex();
				m_Measure = m_InteractorVector[m_CurrentInteractor]->GetMeasureText(m_SelectedMeasure);
				m_MeasureLabel = m_InteractorVector[m_CurrentInteractor]->GetMeasureLabel(m_SelectedMeasure);

				UpdateMeasureList();
				m_Gui->Update();
			}
			break;
			case albaInteractor2DMeasure::ID_MEASURE_SELECTED:
			{				
				// Update Measure Gui Entry
				m_SelectedMeasure = m_InteractorVector[m_CurrentInteractor]->GetSelectedMeasureIndex();
				m_Measure = m_InteractorVector[m_CurrentInteractor]->GetMeasureText(m_SelectedMeasure);
				m_MeasureLabel = m_InteractorVector[m_CurrentInteractor]->GetMeasureLabel(m_SelectedMeasure);

				m_MeasureListBox->SetSelection(m_SelectedMeasure);
				((albaGUIValidator *)m_MeasureListBox->GetValidator())->TransferFromWindow();
				m_Gui->Update();
			}
			break;
			}
		}
	}
}

//----------------------------------------------------------------------------
void albaOpMeasure2D::SetMeasureInteractor(int index)
{
	m_InteractorVector[m_CurrentInteractor]->Enable(false);

	m_CurrentInteractor = index;

	albaEventMacro(albaEvent(this, PER_POP));
	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)m_InteractorVector[m_CurrentInteractor]));

	m_InteractorVector[m_CurrentInteractor]->SetListener(this);
	m_InteractorVector[m_CurrentInteractor]->Enable();

	m_MaxMeasures = m_InteractorVector[m_CurrentInteractor]->GetMaxMeasures();
	m_MeasureType = m_InteractorVector[m_CurrentInteractor]->GetMeasureType();

	UpdateMeasureList();
}

//----------------------------------------------------------------------------
void albaOpMeasure2D::UpdateMeasureList()
{
	if (m_MeasureListBox)
	{
		m_MeasureListBox->Clear();

		if (m_InteractorVector.size() > 0)
			for (int i = 0; i < m_InteractorVector[m_CurrentInteractor]->GetMeasureCount(); i++)
			{
				wxString measure = m_InteractorVector[m_CurrentInteractor]->GetMeasureText(i);
				m_MeasureListBox->Append(_(measure));
			}

		m_Gui->Enable(ID_REMOVE_MEASURE, m_MeasureListBox->GetCount() != 0);

		GetLogicManager()->CameraUpdate();
	}
}

//----------------------------------------------------------------------------
void albaOpMeasure2D::RemoveMeasure(bool clearAll /*Default = false*/)
{
	if (clearAll)
	{
		m_InteractorVector[m_CurrentInteractor]->RemoveAllMeasures();
		m_MeasureListBox->Clear();
	}
	else
	{
		int sel = m_SelectedMeasure;

		if (sel >= 0 && sel < m_InteractorVector[m_CurrentInteractor]->GetMeasureCount())
		{
			m_InteractorVector[m_CurrentInteractor]->RemoveMeasure(sel);
			m_MeasureListBox->Delete(sel);

			if ((sel - 1) >= 0 && (sel - 1) < m_InteractorVector[m_CurrentInteractor]->GetMeasureCount())
			{
				m_InteractorVector[m_CurrentInteractor]->SelectMeasure(sel - 1);
				m_MeasureListBox->Select(sel - 1);
			}

			((albaGUIValidator *)m_MeasureListBox->GetValidator())->TransferFromWindow();
			m_MeasureListBox->Update();
		}
	}

	m_Gui->Enable(ID_REMOVE_MEASURE, m_MeasureListBox->GetCount() != 0);
}

/// LOAD/SAVE - TODO
//----------------------------------------------------------------------------
void albaOpMeasure2D::Load()
{
	wxString tag = "";

	for (int i = 0; i < m_InteractorVector.size(); i++)
	{
		m_InteractorVector[i]->Load(m_Input, tag);
		m_InteractorVector[i]->Update();
	}

	UpdateMeasureList();
}
//----------------------------------------------------------------------------
void albaOpMeasure2D::Save()
{
	wxString tag = "";

	for (int i = 0; i < m_InteractorVector.size(); i++)
		m_InteractorVector[i]->Save(m_Input, tag);
}