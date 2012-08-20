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

#include "medDefines.h" 
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
  EVT_WIZARD_PAGE_CHANGED(wxID_ANY, medGUIWizard::OnWizardPageChange)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
medGUIWizard::medGUIWizard(const wxString& title, bool testMode /* = false */)
: wxWizard()
//----------------------------------------------------------------------------
{
	m_Listener = NULL;

	m_FirstPage = NULL;

	m_EnableChangePage = true;

  m_TestMode = testMode;

  if (!m_TestMode)
  {
    Create(mafGetFrame(),-1,title,wxNullBitmap,wxDefaultPosition, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER ); 
  }
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
  mafEventMacro(mafEvent(this,MED_WIZARD_CHANGE_PAGE,event.GetDirection()));

  if(!m_EnableChangePage)
  {
    event.Veto();
  }
}
//----------------------------------------------------------------------------
void medGUIWizard::OnWizardPageChange(wxWizardEvent& event)
  //----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,MED_WIZARD_CHANGED_PAGE,event.GetDirection()));

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
			mafEventMacro(*e);
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

  if (GetPageAreaSizer() != NULL)
  {
    GetPageAreaSizer()->Add(m_FirstPage);
  }
}