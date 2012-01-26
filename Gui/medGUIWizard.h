/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUIWizard.h,v $
Language:  C++
Date:      $Date: 2012-01-26 13:48:05 $
Version:   $Revision: 1.4.2.6 $
Authors:   Matteo Giacomoni, Gianluigi Crimi
==========================================================================
Copyright (c) 2002/2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/
#ifndef __medGUIWizard_H__
#define __medGUIWizard_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medDefines.h"
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
class MED_EXPORT medGUIWizard : public wxWizard, public mafObserver  
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
