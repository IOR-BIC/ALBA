/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUIWizardPage.cpp,v $
Language:  C++
Date:      $Date: 2007-10-26 11:28:49 $
Version:   $Revision: 1.2 $
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

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include <wx/statline.h>

#include "mafDecl.h"
#include "medGUIWizardPage.h"
#include "medGUIWizard.h"
#include "mmgGui.h"
#include "mmgValidator.h"

//----------------------------------------------------------------------------
// Event Table:
//----------------------------------------------------------------------------
//BEGIN_EVENT_TABLE(medGUIWizardPage, wxWizardPageSimple)
//EVT_CLOSE(medGUIWizardPage::nvOnCloseWindow)
//END_EVENT_TABLE()

enum ID
{
	ID_SCAN_SLICE_LOAD_PAGE=MINID,
};

//----------------------------------------------------------------------------
medGUIWizardPage::medGUIWizardPage(medGUIWizard *wizardParent,long style,wxString label)
: wxWizardPageSimple(wizardParent)
//----------------------------------------------------------------------------
{
	m_Listener = NULL;

	m_FirstPage = NULL;

	m_PreviewSizer = new wxBoxSizer( wxHORIZONTAL );
	m_RwiSizer = new wxBoxSizer( wxVERTICAL );
	m_Rwi = NULL;
	m_Gui = NULL;

	if(style & medUSERWI)
	{
		m_Rwi = new mafRWI(this);
		m_Rwi->SetSize(0,0,500,500);
		m_Rwi->Show(true);
		m_RwiSizer->Add(m_Rwi->m_RwiBase,1,wxEXPAND);

		/*wxPoint dp = wxDefaultPosition;
		wxStaticText *m_SliceLabelLoadPage = new wxStaticText(this, -1, " slice num. ",dp, wxSize(-1,16));
		wxTextCtrl *m_SliceTextLoadPage = new wxTextCtrl(this, -1, "", dp, wxSize(30,16), wxNO_BORDER);
		wxSlider *m_SliceScannerLoadPage = new wxSlider(this, -1, 0, 0, VTK_INT_MAX, dp, wxSize(200,22));

		wxBoxSizer *m_SliceSizerLoadPage = new wxBoxSizer(wxHORIZONTAL);
		m_SliceSizerLoadPage->Add(m_SliceLabelLoadPage, 0, wxALIGN_CENTER|wxRIGHT, 5);
		m_SliceSizerLoadPage->Add(m_SliceTextLoadPage, 0, wxALIGN_CENTER|wxRIGHT, 5);
		m_SliceSizerLoadPage->Add(m_SliceScannerLoadPage, 1, wxALIGN_CENTER|wxEXPAND);

		int m_CurrentSliceLoadPage=0;
		m_SliceScannerLoadPage->SetValidator(mmgValidator(this,ID_SCAN_SLICE_LOAD_PAGE,m_SliceScannerLoadPage,&m_CurrentSliceLoadPage,m_SliceTextLoadPage));
		m_SliceTextLoadPage->SetValidator(mmgValidator(this,ID_SCAN_SLICE_LOAD_PAGE,m_SliceTextLoadPage,&m_CurrentSliceLoadPage,m_SliceScannerLoadPage,0,VTK_INT_MAX));

		m_RwiSizer->Add(m_SliceSizerLoadPage,1,wxEXPAND);*/

		m_PreviewSizer->Add(m_RwiSizer,1,wxEXPAND);
	}
	if(style & medUSEGUI)
	{
		m_Gui = new mmgGui(NULL);
		m_Gui->SetListener(this);
		m_Gui->Reparent(this);
		m_PreviewSizer->Add(m_Gui,0,wxEXPAND);
	}


	SetSizer(m_PreviewSizer,true);
	m_PreviewSizer->Fit(this);
}
//----------------------------------------------------------------------------
medGUIWizardPage::~medGUIWizardPage()
//----------------------------------------------------------------------------
{
	cppDEL(m_Rwi);
}
//--------------------------------------------------------------------------------
void medGUIWizardPage::OnEvent(mafEventBase *maf_event)
//--------------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		/*switch(e->GetId())
		{
		default:*/
			mafEventMacro(*e);
		//}
	}
}
//--------------------------------------------------------------------------------
void medGUIWizardPage::AddGui(mmgGui *gui)
//--------------------------------------------------------------------------------
{
	m_Gui->AddGui(gui);
	m_Gui->Fit();
	m_Gui->Update();
}
//--------------------------------------------------------------------------------
void medGUIWizardPage::SetNextPage(medGUIWizardPage *nextPage)
//--------------------------------------------------------------------------------
{
	this->SetNext(nextPage);
	nextPage->SetPrev(this);
}