/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpShowHistory
 Authors: Nicola Vanella
 
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

#include "albaOpShowHistory.h"
#include "albaDecl.h"
#include "albaAttributeTraceability.h"
#include "albaVMEIterator.h"
#include "albaGUIDialog.h"
#include "albaPics.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpShowHistory);

//----------------------------------------------------------------------------
albaOpShowHistory::albaOpShowHistory(const wxString &label) :
albaOp(label)
{
  m_OpType	= OPTYPE_EDIT;
  m_Canundo = true;

  m_Dialog = NULL;
  m_DialogIsOpened = false;
}
//----------------------------------------------------------------------------
albaOpShowHistory::~albaOpShowHistory()
{
	if (m_Dialog)
	{
		m_Dialog->Hide();

		delete (m_Dialog);
		m_Dialog = NULL;
	}
}
//----------------------------------------------------------------------------
albaOp* albaOpShowHistory::Copy()   
{
	return new albaOpShowHistory(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpShowHistory::InternalAccept(albaVME*node)
{
  return (node && node->IsALBAType(albaVME));
}
//----------------------------------------------------------------------------
void albaOpShowHistory::OpRun()
{
	albaAttributesMap::iterator att_it;

	wxString text = "";

	for (att_it = m_Input->GetAttributes()->begin(); att_it != m_Input->GetAttributes()->end(); att_it++)
	{
		albaAttribute *attr = att_it->second;

		if (attr->IsA("albaAttributeTraceability"))
		{
			albaAttributeTraceability *at = (albaAttributeTraceability*)attr;

			for (int i = 0; i < at->m_TraceabilityVector.size(); i++)
			{
				albaString textDate = at->m_TraceabilityVector[i].m_Date;
				albaString textEvent = at->m_TraceabilityVector[i].m_TrialEvent;

				albaString textOpName = at->m_TraceabilityVector[i].m_OperationName;
				albaString textParam = at->m_TraceabilityVector[i].m_Parameters;

				albaString textApp = at->m_TraceabilityVector[i].m_AppStamp;
				albaString textBuild = at->m_TraceabilityVector[i].m_BuildNum;
// 				albaString textOpID = at->m_TraceabilityVector[i].m_OperatorID;
// 				albaString textNat = at->m_TraceabilityVector[i].m_IsNatural;

				text += textDate + " [" + textEvent + "] " + textOpName;

				if(!textParam.IsEmpty()) 
					text += " (" + textParam + ")";

				text += " {" + textApp + " " + textBuild + "}\n";
			}
		}
	}

	// Create Dialog
	if (m_Dialog == NULL)
	{
		wxString title = wxString::Format("%s History", m_Input->GetName());

		m_Dialog = new albaGUIDialog(title, albaRESIZABLE | albaCLOSEWINDOW);
		
		#include "pic/menu/EDIT_SHOW_HISTORY.xpm"
		m_Dialog->SetIcon(wxIcon(EDIT_SHOW_HISTORY_xpm));

		wxTextCtrl *textCtrl = new wxTextCtrl(m_Dialog, NULL, text, wxDefaultPosition, wxDefaultSize, wxALL | wxEXPAND);
		textCtrl->SetMinSize(wxSize(450, 400));
		textCtrl->SetEditable(false);
		textCtrl->Enable(true);
		
		m_Dialog->Add(textCtrl);
	}

	// Show dialog
	wxSize s = albaGetFrame()->GetSize();
	wxPoint p = albaGetFrame()->GetPosition();
	int posX = p.x + s.GetWidth() * .5 - 450 * .5;
	int posY = p.y + s.GetHeight() * .5 - 400 * .5;
	m_Dialog->SetPosition(wxPoint(posX, posY));
	m_Dialog->ShowModal();

	m_DialogIsOpened = true;

  albaEventMacro(albaEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void albaOpShowHistory::OpDo()
{

}
