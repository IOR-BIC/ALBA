/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIWizardPageNewTest
 Authors: Daniele Giunchi
 
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
#include "albaGUIWizardPageNewTest.h"
#include "albaGUIWizard.h"
#include "albaGUIFrame.h"
#include "albaGUIWizardPageNew.h"
#include "wx/module.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void albaGUIWizardPageNewTest::BeforeTest()
//----------------------------------------------------------------------------
{
	m_Win = new albaGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  wxModule::RegisterModules();
  wxModule::InitializeModules();
}
//----------------------------------------------------------------------------
void albaGUIWizardPageNewTest::AfterTest()
//----------------------------------------------------------------------------
{
  wxModule::CleanUpModules();
	delete m_Win;
}
//----------------------------------------------------------------------------
void albaGUIWizardPageNewTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
  albaGUIWizardPageNew *page = new albaGUIWizardPageNew(wizard,albaWIZARDUSEGUI);

  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardPageNewTest::TestAddGuiLowerLeft()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
  albaGUIWizardPageNew *page = new albaGUIWizardPageNew(wizard,albaWIZARDUSEGUI);

  albaGUI *gui = new albaGUI(page);

  page->AddGuiLowerLeft(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( page->m_GuiLowerLeft->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardPageNewTest::TestAddGuiLowerRight()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
  albaGUIWizardPageNew *page = new albaGUIWizardPageNew(wizard,albaWIZARDUSEGUI);

  albaGUI *gui = new albaGUI(page);

  page->AddGuiLowerUnderCenter(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( page->m_GuiLowerUnderCenter->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardPageNewTest::TestAddGuiLowerCenter()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
  albaGUIWizardPageNew *page = new albaGUIWizardPageNew(wizard,albaWIZARDUSEGUI);

  albaGUI *gui = new albaGUI(page);

  page->AddGuiLowerUnderLeft(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( page->m_GuiLowerUnderLeft->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardPageNewTest::TestAddGuiLowerUnderLeft()
//----------------------------------------------------------------------------
{
    albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
    albaGUIWizardPageNew *page = new albaGUIWizardPageNew(wizard,albaWIZARDUSEGUI);

    albaGUI *gui = new albaGUI(page);

    page->AddGuiLowerUnderLeft(gui);

    //Remove return true only if the gui has been added before
    CPPUNIT_ASSERT( page->m_GuiLowerUnderLeft->Remove(gui) );

    delete gui;
    delete page;
    delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardPageNewTest::TestAddGuiLowerUnderCenter()
//----------------------------------------------------------------------------
{
    albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
    albaGUIWizardPageNew *page = new albaGUIWizardPageNew(wizard,albaWIZARDUSEGUI);

    albaGUI *gui = new albaGUI(page);

    page->AddGuiLowerUnderCenter(gui);

    //Remove return true only if the gui has been added before
    CPPUNIT_ASSERT( page->m_GuiLowerUnderCenter->Remove(gui) );

    delete gui;
    delete page;
    delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardPageNewTest::TestRemoveGuiLowerLeft()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
  albaGUIWizardPageNew *page = new albaGUIWizardPageNew(wizard,albaWIZARDUSEGUI);

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
void albaGUIWizardPageNewTest::TestRemoveGuiLowerUnderLeft()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
  albaGUIWizardPageNew *page = new albaGUIWizardPageNew(wizard,albaWIZARDUSEGUI);

  albaGUI *gui = new albaGUI(page);

  page->AddGuiLowerUnderLeft(gui);
  page->RemoveGuiLowerUnderLeft(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( !page->m_GuiLowerUnderLeft->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}

//----------------------------------------------------------------------------
void albaGUIWizardPageNewTest::TestSetNextPage()
//----------------------------------------------------------------------------
{
  albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
  albaGUIWizardPageNew *page1 = new albaGUIWizardPageNew(wizard,albaWIZARDUSEGUI);
  albaGUIWizardPageNew *page2 = new albaGUIWizardPageNew(wizard,albaWIZARDUSEGUI);

  page1->SetNextPage(page2);

  CPPUNIT_ASSERT( page2->GetPrev() == page1 );
  CPPUNIT_ASSERT( page1->GetNext() == page2 );

  delete page2;
  delete page1;
  delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardPageNewTest::TestSetGetZCropBounds()
//----------------------------------------------------------------------------
{
    albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
    albaGUIWizardPageNew *page = new albaGUIWizardPageNew(wizard,albaWIZARDUSEGUI,true);

    albaGUI *gui = new albaGUI(page);

    page->AddGuiLowerUnderLeft(gui);
    page->RemoveGuiLowerUnderLeft(gui);

    //smoke test
    page->SetZCropBounds(5., 10.);
    double ZCropBounds[2];
    page->GetZCropBounds(ZCropBounds);

    //Remove return true only if the gui has been added before
    CPPUNIT_ASSERT( !page->m_GuiLowerUnderLeft->Remove(gui) );

    delete gui;
    delete page;
    delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardPageNewTest::TestUpdateWindowing()
//----------------------------------------------------------------------------
{
    albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
    albaGUIWizardPageNew *page1 = new albaGUIWizardPageNew(wizard,albaWIZARDUSEGUI);
    
    //smoke test
    page1->UpdateWindowing();

    delete page1;
    delete wizard;
}
//----------------------------------------------------------------------------
void albaGUIWizardPageNewTest::TestUpdateActor()
//----------------------------------------------------------------------------
{
    albaGUIWizard *wizard = new albaGUIWizard(_("TEST"),true);
    albaGUIWizardPageNew *page1 = new albaGUIWizardPageNew(wizard,albaWIZARDUSEGUI);

    //smoke test
    page1->UpdateActor();

    delete page1;
    delete wizard;
}