/*=========================================================================

 Program: MAF2Medical
 Module: medGUIWizardPage
 Authors: Matteo Giacomoni
 
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

#include "mafDecl.h"
#include "medGUIWizardPage.h"
#include "medGUIWizard.h"
#include "mafGUI.h"
#include "mafGUIValidator.h"

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
medGUIWizardPage::medGUIWizardPage(medGUIWizard *wizardParent,long style /* = medUSEGUI | medUSERWI */,wxString label/* = */,bool plugCentralGui /* = true */,bool plugLeftGui /* = true */,bool plugRightGui /* = true */)
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
		if (plugLeftGui)
		{
			m_GuiLowerLeft = new mafGUI(this);
	    m_GuiLowerLeft->FitGui();
			m_GuiLowerLeft->Reparent(this);
	    m_GUISizer->Add(m_GuiLowerLeft,0,wxEXPAND);
		}

    if (plugCentralGui)
    {
      m_GuiLowerCenter = new mafGUI(this);
      m_GuiLowerCenter->FitGui();
      m_GuiLowerCenter->Reparent(this);
      m_GUISizer->Add(m_GuiLowerCenter,0,wxEXPAND);
    }

    if (plugRightGui)
    {
	    m_GuiLowerRight = new mafGUI(this);
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
medGUIWizardPage::~medGUIWizardPage()
//----------------------------------------------------------------------------
{
	cppDEL(m_Rwi);

  if(!(m_Style & medUSERWI))
  {
    delete m_RwiSizer;
  }
  if(!(m_Style & medUSEGUI))
  {
    delete m_GUISizer;
  }
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
void medGUIWizardPage::AddGuiLowerRight(mafGUI *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerRight->AddGui(gui);
  m_GuiLowerRight->FitGui();
  m_GuiLowerRight->Update();
}
//--------------------------------------------------------------------------------
void medGUIWizardPage::AddGuiLowerLeft(mafGUI *gui)
//--------------------------------------------------------------------------------
{
	m_GuiLowerLeft->AddGui(gui);
	m_GuiLowerLeft->FitGui();
	m_GuiLowerLeft->Update();
}
//--------------------------------------------------------------------------------
void medGUIWizardPage::RemoveGuiLowerLeft(mafGUI *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerLeft->Remove(gui);
  m_GuiLowerLeft->FitGui();
  m_GuiLowerLeft->Update();
}
//--------------------------------------------------------------------------------
void medGUIWizardPage::RemoveGuiLowerCenter(mafGUI *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerCenter->Remove(gui);
  m_GuiLowerCenter->FitGui();
  m_GuiLowerCenter->Update();
}
//--------------------------------------------------------------------------------
void medGUIWizardPage::RemoveGuiLowerRight(mafGUI *gui)
//--------------------------------------------------------------------------------
{
  m_GuiLowerRight->Remove(gui);
  m_GuiLowerRight->FitGui();
  m_GuiLowerRight->Update();
}
//--------------------------------------------------------------------------------
void medGUIWizardPage::AddGuiLowerCenter(mafGUI *gui)
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