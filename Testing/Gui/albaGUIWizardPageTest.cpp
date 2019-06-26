/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIWizardPageTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
#include "albaDecl.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "albaGuiTests.h"
#include "albaGUIWizardPageTest.h"
#include "albaGUIWizard.h"
#include "albaGUIFrame.h"
#include "albaGUIWizardPage.h"
#include "wx/module.h"


#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void albaGUIWizardPageTest::BeforeTest()
//----------------------------------------------------------------------------
{
	wxAppConsole::SetInstance(NULL);
  m_Win = new albaGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  wxModule::RegisterModules();
  wxModule::InitializeModules();
}
//----------------------------------------------------------------------------
void albaGUIWizardPageTest::AfterTest()
//----------------------------------------------------------------------------
{
  wxModule::CleanUpModules();
	delete m_Win;
}
//----------------------------------------------------------------------------
void albaGUIWizardPageTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
  albaGUIWizardPage *page = new albaGUIWizardPage(wizard,albaWIZARDUSEGUI);

  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardPageTest::TestAddGuiLowerLeft()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
  albaGUIWizardPage *page = new albaGUIWizardPage(wizard,albaWIZARDUSEGUI);

  albaGUI *gui = new albaGUI(page);

  page->AddGuiLowerLeft(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( page->m_GuiLowerLeft->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardPageTest::TestAddGuiLowerRight()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
  albaGUIWizardPage *page = new albaGUIWizardPage(wizard,albaWIZARDUSEGUI);

  albaGUI *gui = new albaGUI(page);

  page->AddGuiLowerRight(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( page->m_GuiLowerRight->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardPageTest::TestAddGuiLowerCenter()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
  albaGUIWizardPage *page = new albaGUIWizardPage(wizard,albaWIZARDUSEGUI);

  albaGUI *gui = new albaGUI(page);

  page->AddGuiLowerCenter(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( page->m_GuiLowerCenter->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardPageTest::TestRemoveGuiLowerLeft()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
  albaGUIWizardPage *page = new albaGUIWizardPage(wizard,albaWIZARDUSEGUI);

  albaGUI *gui = new albaGUI(page);

  page->AddGuiLowerLeft(gui);
  page->RemoveGuiLowerLeft(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( !page->m_GuiLowerLeft->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardPageTest::TestRemoveGuiLowerRight()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
  albaGUIWizardPage *page = new albaGUIWizardPage(wizard,albaWIZARDUSEGUI);

  albaGUI *gui = new albaGUI(page);

  page->AddGuiLowerRight(gui);
  page->RemoveGuiLowerRight(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( !page->m_GuiLowerRight->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardPageTest::TestRemoveGuiLowerCenter()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
  albaGUIWizardPage *page = new albaGUIWizardPage(wizard,albaWIZARDUSEGUI);

  albaGUI *gui = new albaGUI(page);

  page->AddGuiLowerCenter(gui);
  page->RemoveGuiLowerCenter(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( !page->m_GuiLowerCenter->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardPageTest::TestSetNextPage()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
  albaGUIWizardPage *page1 = new albaGUIWizardPage(wizard,albaWIZARDUSEGUI);
  albaGUIWizardPage *page2 = new albaGUIWizardPage(wizard,albaWIZARDUSEGUI);

  page1->SetNextPage(page2);

  CPPUNIT_ASSERT( page2->GetPrev() == page1 );
  CPPUNIT_ASSERT( page1->GetNext() == page2 );

  delete page2;
  delete page1;
  delete wizard;
}
