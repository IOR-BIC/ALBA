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
#include "wx/msw/listctrl.h"
#include "wx/tokenzr.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpShowHistory);

enum HISTORY_ID
{
	ID_RECURSIVE = MINID,
};

class albaHistoryGUIDialog : public albaGUIDialog
{
public:
	albaHistoryGUIDialog(const wxString& title, long style = albaCLOSEWINDOW | albaRESIZABLE | albaCLOSE, albaObserver *listener = NULL) :albaGUIDialog(title, style, listener)
	{
		m_Listcrtl = NULL;
	};
	virtual ~albaHistoryGUIDialog() {};
	/** Handle on size event */
	virtual void OnSize(wxSizeEvent &event)
	{
		if (m_Listcrtl)
		{
			int vSize = event.GetSize().GetY();
			vSize -= 60;
			m_Listcrtl->SetMinSize(wxSize(-1, vSize));
			m_Listcrtl->SetSize(wxSize(-1, vSize));
		}
		albaGUIDialog::OnSize(event);
	};

	/** Sets Listcrtl */
	void SetListcrtl(wxListCtrl * listcrtl) { m_Listcrtl = listcrtl; }

protected:
	wxListCtrl *m_Listcrtl;
};

//----------------------------------------------------------------------------
albaOpShowHistory::albaOpShowHistory(const wxString &label) :
albaOp(label)
{
  m_OpType	= OPTYPE_EDIT;
  m_Canundo = true;

  m_Dialog = NULL;
  m_DialogIsOpened = false;
	m_SortColumn = 0;
	m_SortAscending = true;
	m_Recursive = false;
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
void albaOpShowHistory::InsertItem(const wxString& date, const wxString& vme, const wxString& type, const wxString& op, const wxString& app)
{
	
	 
	
	long index = m_ListCtrl->InsertItem(m_ListCtrl->GetItemCount() , date);
	m_ListCtrl->SetItem(index, 1, vme);
	m_ListCtrl->SetItem(index, 2, type);
	m_ListCtrl->SetItem(index, 3, op);
	m_ListCtrl->SetItem(index, 4, app);

	m_ListCtrl->SetItemData(index, index);
}

//----------------------------------------------------------------------------
int wxCALLBACK albaOpShowHistory::SortCallback(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{
	auto* frame = reinterpret_cast<albaOpShowHistory*>(sortData);
	wxListCtrl* lc = frame->m_ListCtrl;

	long id1 = lc->FindItem(-1, item1);
	long id2 = lc->FindItem(-1, item2);

	wxString text1 = lc->GetItemText(id1, frame->m_SortColumn);
	wxString text2 = lc->GetItemText(id2, frame->m_SortColumn);
	int cmp = text1.Cmp(text2); // Lexical comparison

	//albaLogMessage("colA:%d, colB:%d, textA:%s, textB:%s, cmp=%d", item1, item2, text1.ToAscii(), text2.ToAscii(), cmp);

	return frame->m_SortAscending ? cmp : -cmp;
}


void  albaOpShowHistory::OnColClick(wxListEvent& event) {
	int col = event.GetColumn();

	// Toggle sort order if same column, otherwise reset to ascending
	if (m_SortColumn == col) {
		m_SortAscending = !m_SortAscending;
	}
	else {
		m_SortColumn = col;
		m_SortAscending = true;
	}

	m_ListCtrl->SortItems(SortCallback, (wxIntPtr)this);

	m_Dialog->Update();
}


//----------------------------------------------------------------------------
void albaOpShowHistory::OnResize(wxSizeEvent& event)
{
	int totalWidth = m_ListCtrl->GetClientSize().GetWidth();
	int height=m_ListCtrl->GetClientSize().GetHeight();

	int typeColW = totalWidth / 10;

	int otherColW = (totalWidth - typeColW) / 4;

	for (int i = 0; i < 5; ++i) {
		m_ListCtrl->SetColumnWidth(i, (i == 2) ? typeColW : otherColW);
	}

	event.Skip();
}

//----------------------------------------------------------------------------
wxString albaOpShowHistory::ReformatDateTime(const wxString& input)
{
	// Split into date and time
	wxArrayString parts = wxStringTokenize(input, " ");
	if (parts.size() != 2)
		return input;

	wxString date = parts[0];
	wxString time = parts[1];

	// Split date into day/month/year
	wxArrayString dateParts = wxStringTokenize(date, "/");
	if (dateParts.size() != 3)
		return input;

	wxString day = dateParts[0];
	wxString month = dateParts[1];
	wxString year = dateParts[2];

	// Return reformatted string
	return wxString::Format("%s/%s/%s %s", year, month, day, time);
}

//----------------------------------------------------------------------------
void albaOpShowHistory::OpRun()
{
	// Create Dialog
	if (m_Dialog == NULL)
	{
		wxString title = wxString::Format("%s History", m_Input->GetName());

		m_Dialog = new albaHistoryGUIDialog(title, albaRESIZABLE | albaCLOSEWINDOW);
		
		#include "pic/menu/SHOW_HISTORY.xpm"
		m_Dialog->SetIcon(wxIcon(SHOW_HISTORY_xpm));

		m_ListCtrl = new wxListCtrl(m_Dialog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);

		m_ListCtrl->SetBackgroundColour(*wxLIGHT_GREY); // Base color

		m_ListCtrl->SetMinSize(wxSize(-1, 275));
		m_ListCtrl->SetSize(wxSize(-1, 800));
		
		m_Dialog->Add(m_ListCtrl);
		
		albaGUI *gui = new albaGUI(this);
		gui->Bool(ID_RECURSIVE, "Show Children history", &m_Recursive, true);
		m_Dialog->Add(gui);
	}


	// Create 5 columns
	m_ListCtrl->InsertColumn(0, "Date:");
	m_ListCtrl->InsertColumn(1, "VME:");
	m_ListCtrl->InsertColumn(2, "Type:");
	m_ListCtrl->InsertColumn(3, "Operation:");
	m_ListCtrl->InsertColumn(4, "Application:");

	AddToList(m_Input);
	
	m_Dialog->SetListcrtl(m_ListCtrl);
	// Bind column click for sorting
	m_ListCtrl->Bind(wxEVT_LIST_COL_CLICK, &albaOpShowHistory::OnColClick, this);

	m_ListCtrl->Bind(wxEVT_SIZE, &albaOpShowHistory::OnResize, this);

	// Show dialog
	wxSize s = albaGetFrame()->GetSize();
	wxPoint p = albaGetFrame()->GetPosition();
	int posX = p.x + s.GetWidth() * .5 - 700 * .5;
	int posY = p.y + s.GetHeight() * .5 - 300 * .5;
	m_Dialog->SetPosition(wxPoint(posX, posY));
	m_Dialog->SetSize(700, 300);
	m_Dialog->ShowModal();

	m_DialogIsOpened = true;

  albaEventMacro(albaEvent(this,OP_RUN_OK));
}

//----------------------------------------------------------------------------
void albaOpShowHistory::AddToList(albaVME * vme, bool recursive)
{
	albaAttributesMap::iterator att_it;

	std::vector<albaVME *> vmeList;

	
	if (recursive)
	{
		albaVMEIterator *iter = vme->NewIterator();
		for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
			vmeList.push_back(node);
	}
	else
	{
		vmeList.push_back(vme);
	}
	
	m_ListCtrl->DeleteAllItems();

	for (int vmeNum = 0; vmeNum < vmeList.size(); vmeNum++)
		for (att_it = vmeList[vmeNum]->GetAttributes()->begin(); att_it != vmeList[vmeNum]->GetAttributes()->end(); att_it++)
		{
			albaAttribute *attr = att_it->second;

			if (attr->IsA("albaAttributeTraceability"))
			{
				albaAttributeTraceability *at = (albaAttributeTraceability*)attr;

				for (int i = 0; i < at->m_TraceabilityVector.size(); i++)
				{
					wxString date = at->m_TraceabilityVector[i].m_Date;
					wxString type = at->m_TraceabilityVector[i].m_TrialEvent;
					wxString vmeName = vmeList[vmeNum]->GetName();
					wxString operation = at->m_TraceabilityVector[i].m_OperationName;
					wxString textParam = at->m_TraceabilityVector[i].m_Parameters;
					if (!textParam.IsEmpty())
						operation += "(" + textParam + ")";

					wxString app = at->m_TraceabilityVector[i].m_AppStamp;
					app += " " + at->m_TraceabilityVector[i].m_BuildNum;

					InsertItem(ReformatDateTime(date), vmeName, type, operation, app);
				}
			}
		}
}

//----------------------------------------------------------------------------
void albaOpShowHistory::OpDo()
{

}

//----------------------------------------------------------------------------
void albaOpShowHistory::OnEvent(albaEventBase *alba_event)
{
	switch (alba_event->GetId())
	{
		case ID_RECURSIVE:
		{
			AddToList(m_Input, m_Recursive);
		}
		break;
		default:
			Superclass::OnEvent(alba_event);
		break;
	}
}
