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

#ifndef __albaOpShowHistory_H__
#define __albaOpShowHistory_H__

#include "albaOp.h"
#include "albaVME.h"
#include "albaSmartPointer.h"
#include <wx/listctrl.h>
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMEIterator;
class albaAttributesMap;
class albaHistoryGUIDialog;

//----------------------------------------------------------------------------
// albaOpShowHistory :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpShowHistory: public albaOp
{
public:
  albaOpShowHistory(const wxString &label = "Show History");
  ~albaOpShowHistory(); 

  albaTypeMacro(albaOpShowHistory, albaOp);

  albaOp* Copy();

  void OpRun();

	void AddToList(albaVME * vme, bool recursive=false);

	void OpDo();

	virtual void OnEvent(albaEventBase *alba_event);

    	
protected: 

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	void InsertItem(const wxString& date, const wxString& vme, const wxString& type, const wxString& op, const wxString& app);

	// Sorting logic
	static int wxCALLBACK SortCallback(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData);

	void OnColClick(wxListEvent& event);

	void OnResize(wxSizeEvent& event);

	wxString ReformatDateTime(const wxString& input);
	
	int m_SortColumn;
	bool m_SortAscending;
	int m_Recursive;

	typedef std::map<albaString, albaAutoPointer<albaAttribute> > albaAttributesMap;

	albaHistoryGUIDialog *m_Dialog;
	wxListCtrl *m_ListCtrl;
	bool m_DialogIsOpened;
};
#endif
