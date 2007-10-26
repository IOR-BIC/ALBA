/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUIWizard.cpp,v $
Language:  C++
Date:      $Date: 2007-10-26 11:10:13 $
Version:   $Revision: 1.4 $
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

#include "medGUIWizard.h"
#include "wx/busyinfo.h"
#include "mafDecl.h"

//----------------------------------------------------------------------------
// Event Table:
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(medGUIWizard, wxWizard)
	EVT_WIZARD_PAGE_CHANGING(wxID_ANY, medGUIWizard::OnWizardPageChanging)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
medGUIWizard::medGUIWizard(const wxString& title)
: wxWizard()
//----------------------------------------------------------------------------
{
	m_Listener = NULL;

	m_FirstPage = NULL;

	m_EnableChangePage = true;

	Create(mafGetFrame(),-1,title,wxNullBitmap,wxDefaultPosition); 
}
//----------------------------------------------------------------------------
medGUIWizard::~medGUIWizard()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medGUIWizard::OnWizardPageChanging(wxWizardEvent& event)
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,MED_WIZARD_CHANGE_PAGE));

	if(!m_EnableChangePage)
	{
		event.Veto();
	}
}
//--------------------------------------------------------------------------------
void medGUIWizard::OnEvent(mafEventBase *maf_event)
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
bool medGUIWizard::Run()
//--------------------------------------------------------------------------------
{
	return RunWizard(m_FirstPage);
	//ShowModal();
}
//--------------------------------------------------------------------------------
void medGUIWizard::SetFirstPage(wxWizardPageSimple *firstPage)
//--------------------------------------------------------------------------------
{
	m_FirstPage=firstPage;
	GetPageAreaSizer()->Add(m_FirstPage);
}