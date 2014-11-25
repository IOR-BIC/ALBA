/*=========================================================================

 Program: MAF2Medical
 Module: medGUIWizard
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __medGUIWizard_H__
#define __medGUIWizard_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "wx/wizard.h"
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafGUI.h"
//----------------------------------------------------------------------------
// Const:
//----------------------------------------------------------------------------

/**
  Class Name: medGUIWizard.
  This class is the container of wizard pages in order to handle typical gui wizard widget.
  It can set first page and enable changing pages.
*/
class MAF_EXPORT medGUIWizard : public wxWizard, public mafObserver  
{
public:
  /** constructor. */
	medGUIWizard (const wxString& title, bool testMode = false);
  /** destructor. */
	virtual ~medGUIWizard (); 
  /** Set the Listener that will receive event-notification. */
	void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  /** Precess events coming from other objects. */
	void OnEvent(mafEventBase *maf_event);

  /** Call wxWizard::RunWizard.  */
	bool Run();

  /** Set the first page of the wizard. */
	void SetFirstPage(wxWizardPageSimple *firstPage);

  /** Return the first page of the wizard. */
  wxWizardPageSimple* GetFirstPage(){return m_FirstPage;};

  /** Enable the changing of the page. */
	void EnableChangePageOn(){m_EnableChangePage=true;};
  /** Disable the changing of the page. */
	void EnableChangePageOff(){m_EnableChangePage=false;};

  /** Return the enable changing of the page. */
  bool GetEnableChangePage(){return m_EnableChangePage;};

  /** Message Ids */
	enum ID_MESSAGES
	{
		MED_WIZARD_CHANGE_PAGE = MINID,
    MED_WIZARD_CHANGED_PAGE,
		ID_LAST,
	};

	mafObserver *m_Listener;

private:

	wxWizardPageSimple *m_FirstPage;

	bool m_EnableChangePage;

  bool m_TestMode;

  /** event handler function for closing the window. */
	void OnCloseWindow(wxCloseEvent &event) {OnCloseWindow(event);};
  /** event handler function for close the dialog that call nvOnCloseWindow. */
	void OnClose(wxCommandEvent &event)     {wxDialog::Close();};
  /** event handler function for changing page in the wizard. */
	void OnWizardPageChanging(wxWizardEvent& event);
  /** event handler function for change page in the wizard. */
  void OnWizardPageChange(wxWizardEvent& event);

  /** event table declaration */
	DECLARE_EVENT_TABLE()
};
#endif
