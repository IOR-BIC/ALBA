/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUIWizard.h,v $
Language:  C++
Date:      $Date: 2007-09-25 14:20:27 $
Version:   $Revision: 1.1 $
Authors:   Matteo Giacomoni
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
#include "wx/wizard.h"
#include "mafEvent.h"
#include "mafObserver.h"
#include "mmgGui.h"
//----------------------------------------------------------------------------
// Const:
//----------------------------------------------------------------------------
enum DIALOG_STYLES
{
	mafRESIZABLE    =  1,  // make the Dialog Resizable  
	mafCLOSEWINDOW  =  2,  // to enable the close button on the window frame
	mafOK           =  4,  // to create an ok button
	mafCANCEL       =  8,  // to create a cancel button 
	mafCLOSE        = 16, // to create a close button
};
//----------------------------------------------------------------------------
// medGUIWizard :
//----------------------------------------------------------------------------
/**
*/
class medGUIWizard : public wxWizard, public mafObserver  
{
public:
	medGUIWizard (const wxString& title);
	virtual ~medGUIWizard (); 
	void SetListener(mafObserver *Listener) {m_Listener = Listener;};
	void OnEvent(mafEventBase *maf_event);

	bool Run();

	void SetFirstPage(wxWizardPageSimple *firstPage);

	mafObserver *m_Listener;

private:

	wxWizardPageSimple *m_FirstPage;

	void nvOnCloseWindow(wxCloseEvent &event) {OnCloseWindow(event);};
	void nvOnClose(wxCommandEvent &event)     {wxDialog::Close();}; //calls nvOnCloseWindow
	void OnWizardPageChanging(wxWizardEvent& event);

	//DECLARE_EVENT_TABLE()
};
#endif
