/*=========================================================================
Program:   Alba
Module:    albaOpEmpty.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2024 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the alba must include "albaDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "albaOpEmpty.h"
#include "albaDecl.h"
#include "albaGUI.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpEmpty);

//----------------------------------------------------------------------------
albaOpEmpty::albaOpEmpty(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
}

//----------------------------------------------------------------------------
albaOpEmpty::~albaOpEmpty()
{
}

//----------------------------------------------------------------------------
bool albaOpEmpty::InternalAccept(albaVME *node)
{
	//return node->IsA("...");
	return true;
}

//----------------------------------------------------------------------------
char** albaOpEmpty::GetIcon()
{
#include "pic/MENU_OP_ROSENBERG_MEASURE.xpm"
	return MENU_OP_ROSENBERG_MEASURE_xpm;
}

//----------------------------------------------------------------------------
albaOp* albaOpEmpty::Copy()
{
	albaOpEmpty *cp = new albaOpEmpty(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void albaOpEmpty::OpRun()
{
	if (!m_TestMode)
	{
		CreateGui();
	}
	
	//OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void albaOpEmpty::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void albaOpEmpty::OpDo()
{
}

//----------------------------------------------------------------------------
void albaOpEmpty::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		m_Gui->Update();
		//if (e->GetSender() == m_Gui)
		{
			switch (e->GetId())
			{
			case wxOK:
				OpStop(OP_RUN_OK);
				break;

			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
				break;

			default:
				Superclass::OnEvent(alba_event);
				break;
			}
		}
// 		else
// 		{
// 			Superclass::OnEvent(alba_event);
// 		}
	}
}

//----------------------------------------------------------------------------
void albaOpEmpty::CreateGui()
{
	// Interface:
	m_Gui = new albaGUI(this);

	m_Gui->HintBox(NULL, "This operation is empty.", "Hint");

//	((albaGUI*)m_Gui)->HyperLink(NULL, "Link", "https://github.com/IOR-BIC");

// 	((albaGUI*)m_Gui)->Separator(0, wxSize(1, 100));
//	((albaGUI*)m_Gui)->Separator(2, wxSize(250, 1));
	
	// ToDO: add your custom widgets...

	//m_Gui->Label("Press a button");

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	ShowGui();
}