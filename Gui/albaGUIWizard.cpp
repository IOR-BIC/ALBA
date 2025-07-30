/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIWizard
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include <wx/statline.h>

#include "albaGUIWizard.h"
#include "albaGUIBusyInfo.h"
#include "albaDecl.h"

//----------------------------------------------------------------------------
// Event Table:
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(albaGUIWizard, wxWizard)
	EVT_WIZARD_PAGE_CHANGING(wxID_ANY, albaGUIWizard::OnWizardPageChanging)
  EVT_WIZARD_PAGE_CHANGED(wxID_ANY, albaGUIWizard::OnWizardPageChange)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
albaGUIWizard::albaGUIWizard(const wxString& title, bool testMode /* = false */)
: wxWizard()
//----------------------------------------------------------------------------
{
	m_Listener = NULL;

	m_FirstPage = NULL;

	m_EnableChangePage = true;

  m_TestMode = testMode;

  if (!m_TestMode)
  {
    Create(albaGetFrame(),-1,title,wxNullBitmap,wxDefaultPosition, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER ); 
  }
}
//----------------------------------------------------------------------------
albaGUIWizard::~albaGUIWizard()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUIWizard::OnWizardPageChanging(wxWizardEvent& event)
  //----------------------------------------------------------------------------
{
  albaEventMacro(albaEvent(this,ALBA_WIZARD_CHANGE_PAGE,event.GetDirection()));

  if(!m_EnableChangePage)
  {
    event.Veto();
  }
}
//----------------------------------------------------------------------------
void albaGUIWizard::OnWizardPageChange(wxWizardEvent& event)
  //----------------------------------------------------------------------------
{
  albaEventMacro(albaEvent(this,ALBA_WIZARD_CHANGED_PAGE,event.GetDirection()));

  if(!m_EnableChangePage)
  {
    event.Veto();
  }
}
//--------------------------------------------------------------------------------
void albaGUIWizard::OnEvent(albaEventBase *alba_event)
//--------------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
			albaEventMacro(*e);
	}
}
//--------------------------------------------------------------------------------
bool albaGUIWizard::Run()
//--------------------------------------------------------------------------------
{
	return RunWizard(m_FirstPage);
	//ShowModal();
}
//--------------------------------------------------------------------------------
void albaGUIWizard::SetFirstPage(wxWizardPageSimple *firstPage)
//--------------------------------------------------------------------------------
{
	m_FirstPage=firstPage;

  if (GetPageAreaSizer() != NULL)
  {
    GetPageAreaSizer()->Add(m_FirstPage);
  }
}