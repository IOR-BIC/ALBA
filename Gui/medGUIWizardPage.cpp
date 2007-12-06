/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUIWizardPage.cpp,v $
Language:  C++
Date:      $Date: 2007-12-06 09:37:03 $
Version:   $Revision: 1.3 $
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

	m_GUISizer = new wxBoxSizer( wxHORIZONTAL );
	m_RwiSizer = new wxBoxSizer( wxHORIZONTAL );
  m_SizerAll = new wxBoxSizer( wxVERTICAL );

	m_Rwi = NULL;
	m_GuiLowerRight = NULL;
  m_GuiLowerLeft = NULL;

	if(style & medUSERWI)
	{
		m_Rwi = new mafRWI(this);
		m_Rwi->SetSize(0,0,600,600);
		m_Rwi->Show(true);
		m_RwiSizer->Add(m_Rwi->m_RwiBase,1,wxEXPAND);
    m_SizerAll->Add(m_RwiSizer,1,wxEXPAND);
	}
	if(style & medUSEGUI)
	{
		m_GuiLowerLeft = new mmgGui(this);
    m_GuiLowerLeft->FitGui();
		m_GuiLowerLeft->Reparent(this);

    m_GuiLowerRight = new mmgGui(this);
    m_GuiLowerRight->FitGui();
    m_GuiLowerRight->Reparent(this);

    m_GuiLowerCenter = new mmgGui(this);
    m_GuiLowerCenter->FitGui();
    m_GuiLowerCenter->Reparent(this);

		m_GUISizer->Add(m_GuiLowerLeft,0,wxEXPAND);
    m_GUISizer->Add(m_GuiLowerCenter,0,wxEXPAND);
    m_GUISizer->Add(m_GuiLowerRight,0,wxEXPAND);

    m_SizerAll->Add(m_GUISizer,0,wxEXPAND|wxALL);
	}
	
	SetSizer(m_SizerAll,true);
	m_SizerAll->Fit(this);
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
			mafEventMacro(*e);
	}
}
//--------------------------------------------------------------------------------
void medGUIWizardPage::AddGuiLowerRight(mmgGui *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerRight->AddGui(gui);
  m_GuiLowerRight->FitGui();
  m_GuiLowerRight->Update();
}
//--------------------------------------------------------------------------------
void medGUIWizardPage::AddGuiLowerLeft(mmgGui *gui)
//--------------------------------------------------------------------------------
{
	m_GuiLowerLeft->AddGui(gui);
	m_GuiLowerLeft->FitGui();
	m_GuiLowerLeft->Update();
}
//--------------------------------------------------------------------------------
void medGUIWizardPage::AddGuiLowerCenter(mmgGui *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerCenter->AddGui(gui);
  m_GuiLowerCenter->FitGui();
  m_GuiLowerCenter->Update();
}
//--------------------------------------------------------------------------------
void medGUIWizardPage::SetNextPage(medGUIWizardPage *nextPage)
//--------------------------------------------------------------------------------
{
	this->SetNext(nextPage);
	nextPage->SetPrev(this);
}