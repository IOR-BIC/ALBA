/*=========================================================================
Program:   AssemblerPro
Module:    albaGUIDialogProsthesisSelection.h
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

#ifndef __albaGUIDialogProsthesisSelection_H__
#define __albaGUIDialogProsthesisSelection_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUIDialog.h"
//#include "appOpCreateProsthesis.h"
#include "albaProsthesesDBManager.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaGUIButton;
class albaGUILab;
class albaGUIPicButton;
class albaGUIHyperLink;
class albaProsthesesDBManager;

class ALBA_EXPORT albaGUIDialogProsthesisSelection : public albaGUIDialog
{
public:

	albaGUIDialogProsthesisSelection(const wxString& title, long style = albaCLOSEWINDOW);
	virtual ~albaGUIDialogProsthesisSelection();

	void OnEvent(albaEventBase *alba_event);

	void SetProducer(albaProDBProducer *producer);
	void SetProducer(albaString producerName);

	void SetProsthesis(albaProDBProsthesis *prosthesis);
	albaProDBProsthesis * GetProsthesis() {	return m_CurrentProsthesis; };

	bool OkClosed() { return m_IsChanged; };

	void Show();

protected:

	void CreateDialog();
	void UpdateDialog();
	
	bool m_IsChanged;

	albaProsthesesDBManager *m_DBManager;

	albaProDBProducer  *m_CurrentProducer;
	albaProDBProsthesis *m_CurrentProsthesis;

	int m_SelectedProducer;
	int m_SelectedProsthesis;

	wxString m_ProducerName;
	wxString m_ProducerWebSite;
	wxString m_ProducerImageName;
	wxString m_ProducerImageFullName;

	wxString m_ProsthesisName;
	wxString m_ProsthesisImageName;
	wxString m_ProsthesisImageFullName;

	albaGUI *m_Gui; ///< Gui variable used to plug custom widgets
	
	wxBoxSizer *m_MainBoxSizer;
	wxBoxSizer *m_ProducerImageSizer;
	albaGUIPicButton *m_ProducerImageButton;

	wxComboBox *m_ProducerComboBox;
	wxTextCtrl *m_ProducerName_textCtrl;
	albaGUIHyperLink *m_ProducerName_Link;

	wxStaticBoxSizer *m_ProsthesisBoxSizer;
	wxComboBox *m_ProsthesisComboBox;

	albaGUIButton *m_OkBtn;
};
#endif
