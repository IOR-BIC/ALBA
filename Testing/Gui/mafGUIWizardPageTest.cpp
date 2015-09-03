/*=========================================================================

 Program: MAF2
 Module: mafGUIWizardPageTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
#include "mafDecl.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafGuiTests.h"
#include "mafGUIWizardPageTest.h"
#include "mafGUIWizard.h"
#include "mafGUIFrame.h"
#include "mafGUIWizardPage.h"
#include "wx/module.h"


#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void mafGUIWizardPageTest::BeforeTest()
//----------------------------------------------------------------------------
{
	wxAppConsole::SetInstance(NULL);
  m_Win = new mafGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  wxModule::RegisterModules();
  wxModule::InitializeModules();
}
//----------------------------------------------------------------------------
void mafGUIWizardPageTest::AfterTest()
//----------------------------------------------------------------------------
{
  wxModule::CleanUpModules();
	delete m_Win;
}
//----------------------------------------------------------------------------
void mafGUIWizardPageTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
  mafGUIWizardPage *page = new mafGUIWizardPage(wizard,medUSEGUI);

  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardPageTest::TestAddGuiLowerLeft()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
  mafGUIWizardPage *page = new mafGUIWizardPage(wizard,medUSEGUI);

  mafGUI *gui = new mafGUI(page);

  page->AddGuiLowerLeft(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( page->m_GuiLowerLeft->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardPageTest::TestAddGuiLowerRight()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
  mafGUIWizardPage *page = new mafGUIWizardPage(wizard,medUSEGUI);

  mafGUI *gui = new mafGUI(page);

  page->AddGuiLowerRight(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( page->m_GuiLowerRight->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardPageTest::TestAddGuiLowerCenter()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
  mafGUIWizardPage *page = new mafGUIWizardPage(wizard,medUSEGUI);

  mafGUI *gui = new mafGUI(page);

  page->AddGuiLowerCenter(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( page->m_GuiLowerCenter->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardPageTest::TestRemoveGuiLowerLeft()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
  mafGUIWizardPage *page = new mafGUIWizardPage(wizard,medUSEGUI);

  mafGUI *gui = new mafGUI(page);

  page->AddGuiLowerLeft(gui);
  page->RemoveGuiLowerLeft(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( !page->m_GuiLowerLeft->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardPageTest::TestRemoveGuiLowerRight()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
  mafGUIWizardPage *page = new mafGUIWizardPage(wizard,medUSEGUI);

  mafGUI *gui = new mafGUI(page);

  page->AddGuiLowerRight(gui);
  page->RemoveGuiLowerRight(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( !page->m_GuiLowerRight->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardPageTest::TestRemoveGuiLowerCenter()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
  mafGUIWizardPage *page = new mafGUIWizardPage(wizard,medUSEGUI);

  mafGUI *gui = new mafGUI(page);

  page->AddGuiLowerCenter(gui);
  page->RemoveGuiLowerCenter(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( !page->m_GuiLowerCenter->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardPageTest::TestSetNextPage()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
  mafGUIWizardPage *page1 = new mafGUIWizardPage(wizard,medUSEGUI);
  mafGUIWizardPage *page2 = new mafGUIWizardPage(wizard,medUSEGUI);

  page1->SetNextPage(page2);

  CPPUNIT_ASSERT( page2->GetPrev() == page1 );
  CPPUNIT_ASSERT( page1->GetNext() == page2 );

  delete page2;
  delete page1;
  delete wizard;
}
