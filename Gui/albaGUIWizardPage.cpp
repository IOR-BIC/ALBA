/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIWizardPage
 Authors: Matteo Giacomoni
 
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

#include "albaDecl.h"
#include "albaGUIWizardPage.h"
#include "albaGUIWizard.h"
#include "albaGUI.h"
#include "albaGUIValidator.h"

//----------------------------------------------------------------------------
// Event Table:
//----------------------------------------------------------------------------
//BEGIN_EVENT_TABLE(albaGUIWizardPage, wxWizardPageSimple)
//EVT_CLOSE(albaGUIWizardPage::nvOnCloseWindow)
//END_EVENT_TABLE()

enum ID
{
	ID_SCAN_SLICE_LOAD_PAGE=MINID,
};

//----------------------------------------------------------------------------
albaGUIWizardPage::albaGUIWizardPage(albaGUIWizard *wizardParent,long style /* = medUSEGUI | medUSERWI */,wxString label/* = */,bool plugCentralGui /* = true */,bool plugLeftGui /* = true */,bool plugRightGui /* = true */)
: wxWizardPageSimple(wizardParent)
//----------------------------------------------------------------------------
{

  m_Style = style;

	m_Listener = NULL;

	m_FirstPage = NULL;

	m_GUISizer = new wxBoxSizer( wxHORIZONTAL );
	m_RwiSizer = new wxBoxSizer( wxHORIZONTAL );
  m_SizerAll = new wxBoxSizer( wxVERTICAL );

	m_Rwi = NULL;
	m_GuiLowerRight = NULL;
  m_GuiLowerLeft = NULL;
  m_GuiLowerCenter = NULL;

	if(style & albaWIZARDUSERWI)
	{
		m_Rwi = new albaRWI(this);
		m_Rwi->SetSize(0,0,600,600);
		m_Rwi->Show(true);
		m_RwiSizer->Add(m_Rwi->m_RwiBase,1,wxEXPAND);
    m_SizerAll->Add(m_RwiSizer,1,wxEXPAND);
	}
	if(style & albaWIZARDUSEGUI)
	{
		if (plugLeftGui)
		{
			m_GuiLowerLeft = new albaGUI(this);
	    m_GuiLowerLeft->FitGui();
			m_GuiLowerLeft->Reparent(this);
	    m_GUISizer->Add(m_GuiLowerLeft,0,wxEXPAND);
		}

    if (plugCentralGui)
    {
      m_GuiLowerCenter = new albaGUI(this);
      m_GuiLowerCenter->FitGui();
      m_GuiLowerCenter->Reparent(this);
      m_GUISizer->Add(m_GuiLowerCenter,0,wxEXPAND);
    }

    if (plugRightGui)
    {
	    m_GuiLowerRight = new albaGUI(this);
	    m_GuiLowerRight->FitGui();
	    m_GuiLowerRight->Reparent(this);
	    m_GUISizer->Add(m_GuiLowerRight,0,wxEXPAND);
    }

    m_SizerAll->Add(m_GUISizer,0,wxEXPAND|wxALL);
	}
	
	SetSizer(m_SizerAll,true);
	m_SizerAll->Fit(this);
}
//----------------------------------------------------------------------------
albaGUIWizardPage::~albaGUIWizardPage()
//----------------------------------------------------------------------------
{
	cppDEL(m_Rwi);

  if(!(m_Style & albaWIZARDUSERWI))
  {
    delete m_RwiSizer;
  }
  if(!(m_Style & albaWIZARDUSEGUI))
  {
    delete m_GUISizer;
  }
}
//--------------------------------------------------------------------------------
void albaGUIWizardPage::OnEvent(albaEventBase *alba_event)
//--------------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
			albaEventMacro(*e);
	}
}
//--------------------------------------------------------------------------------
void albaGUIWizardPage::AddGuiLowerRight(albaGUI *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerRight->AddGui(gui);
  m_GuiLowerRight->FitGui();
  m_GuiLowerRight->Update();
}
//--------------------------------------------------------------------------------
void albaGUIWizardPage::AddGuiLowerLeft(albaGUI *gui)
//--------------------------------------------------------------------------------
{
	m_GuiLowerLeft->AddGui(gui);
	m_GuiLowerLeft->FitGui();
	m_GuiLowerLeft->Update();
}
//--------------------------------------------------------------------------------
void albaGUIWizardPage::RemoveGuiLowerLeft(albaGUI *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerLeft->Remove(gui);
  m_GuiLowerLeft->FitGui();
  m_GuiLowerLeft->Update();
}
//--------------------------------------------------------------------------------
void albaGUIWizardPage::RemoveGuiLowerCenter(albaGUI *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerCenter->Remove(gui);
  m_GuiLowerCenter->FitGui();
  m_GuiLowerCenter->Update();
}
//--------------------------------------------------------------------------------
void albaGUIWizardPage::RemoveGuiLowerRight(albaGUI *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerRight->Remove(gui);
  m_GuiLowerRight->FitGui();
  m_GuiLowerRight->Update();
}
//--------------------------------------------------------------------------------
void albaGUIWizardPage::AddGuiLowerCenter(albaGUI *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerCenter->AddGui(gui);
  m_GuiLowerCenter->FitGui();
  m_GuiLowerCenter->Update();
}
//--------------------------------------------------------------------------------
void albaGUIWizardPage::SetNextPage(albaGUIWizardPage *nextPage)
//--------------------------------------------------------------------------------
{
	this->SetNext(nextPage);
	nextPage->SetPrev(this);
}