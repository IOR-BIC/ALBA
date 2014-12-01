/*=========================================================================

 Program: MAF2
 Module: mafGUIWizard
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include <wx/statline.h>

#include "mafGUIWizard.h"
#include "wx/busyinfo.h"
#include "mafDecl.h"

//----------------------------------------------------------------------------
// Event Table:
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUIWizard, wxWizard)
	EVT_WIZARD_PAGE_CHANGING(wxID_ANY, mafGUIWizard::OnWizardPageChanging)
  EVT_WIZARD_PAGE_CHANGED(wxID_ANY, mafGUIWizard::OnWizardPageChange)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mafGUIWizard::mafGUIWizard(const wxString& title, bool testMode /* = false */)
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
mafGUIWizard::~mafGUIWizard()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIWizard::OnWizardPageChanging(wxWizardEvent& event)
  //----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,MED_WIZARD_CHANGE_PAGE,event.GetDirection()));

  if(!m_EnableChangePage)
  {
    event.Veto();
  }
}
//----------------------------------------------------------------------------
void mafGUIWizard::OnWizardPageChange(wxWizardEvent& event)
  //----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,MED_WIZARD_CHANGED_PAGE,event.GetDirection()));

  if(!m_EnableChangePage)
  {
    event.Veto();
  }
}
//--------------------------------------------------------------------------------
void mafGUIWizard::OnEvent(mafEventBase *maf_event)
//--------------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
			mafEventMacro(*e);
	}
}
//--------------------------------------------------------------------------------
bool mafGUIWizard::Run()
//--------------------------------------------------------------------------------
{
	return RunWizard(m_FirstPage);
	//ShowModal();
}
//--------------------------------------------------------------------------------
void mafGUIWizard::SetFirstPage(wxWizardPageSimple *firstPage)
//--------------------------------------------------------------------------------
{
	m_FirstPage=firstPage;

  if (GetPageAreaSizer() != NULL)
  {
    GetPageAreaSizer()->Add(m_FirstPage);
  }
}