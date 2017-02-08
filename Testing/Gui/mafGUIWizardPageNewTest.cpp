/*=========================================================================

 Program: MAF2
 Module: mafGUIWizardPageNewTest
 Authors: Daniele Giunchi
 
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
#include "mafGUIWizardPageNewTest.h"
#include "mafGUIWizard.h"
#include "mafGUIFrame.h"
#include "mafGUIWizardPageNew.h"
#include "wx/module.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void mafGUIWizardPageNewTest::BeforeTest()
//----------------------------------------------------------------------------
{
	m_Win = new mafGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  wxModule::RegisterModules();
  wxModule::InitializeModules();
}
//----------------------------------------------------------------------------
void mafGUIWizardPageNewTest::AfterTest()
//----------------------------------------------------------------------------
{
  wxModule::CleanUpModules();
	delete m_Win;
}
//----------------------------------------------------------------------------
void mafGUIWizardPageNewTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
  mafGUIWizardPageNew *page = new mafGUIWizardPageNew(wizard,mafWIZARDUSEGUI);

  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardPageNewTest::TestAddGuiLowerLeft()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
  mafGUIWizardPageNew *page = new mafGUIWizardPageNew(wizard,mafWIZARDUSEGUI);

  mafGUI *gui = new mafGUI(page);

  page->AddGuiLowerLeft(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( page->m_GuiLowerLeft->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardPageNewTest::TestAddGuiLowerRight()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
  mafGUIWizardPageNew *page = new mafGUIWizardPageNew(wizard,mafWIZARDUSEGUI);

  mafGUI *gui = new mafGUI(page);

  page->AddGuiLowerUnderCenter(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( page->m_GuiLowerUnderCenter->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardPageNewTest::TestAddGuiLowerCenter()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
  mafGUIWizardPageNew *page = new mafGUIWizardPageNew(wizard,mafWIZARDUSEGUI);

  mafGUI *gui = new mafGUI(page);

  page->AddGuiLowerUnderLeft(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( page->m_GuiLowerUnderLeft->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardPageNewTest::TestAddGuiLowerUnderLeft()
//----------------------------------------------------------------------------
{
    mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
    mafGUIWizardPageNew *page = new mafGUIWizardPageNew(wizard,mafWIZARDUSEGUI);

    mafGUI *gui = new mafGUI(page);

    page->AddGuiLowerUnderLeft(gui);

    //Remove return true only if the gui has been added before
    CPPUNIT_ASSERT( page->m_GuiLowerUnderLeft->Remove(gui) );

    delete gui;
    delete page;
    delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardPageNewTest::TestAddGuiLowerUnderCenter()
//----------------------------------------------------------------------------
{
    mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
    mafGUIWizardPageNew *page = new mafGUIWizardPageNew(wizard,mafWIZARDUSEGUI);

    mafGUI *gui = new mafGUI(page);

    page->AddGuiLowerUnderCenter(gui);

    //Remove return true only if the gui has been added before
    CPPUNIT_ASSERT( page->m_GuiLowerUnderCenter->Remove(gui) );

    delete gui;
    delete page;
    delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardPageNewTest::TestRemoveGuiLowerLeft()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
  mafGUIWizardPageNew *page = new mafGUIWizardPageNew(wizard,mafWIZARDUSEGUI);

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
void mafGUIWizardPageNewTest::TestRemoveGuiLowerUnderLeft()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
  mafGUIWizardPageNew *page = new mafGUIWizardPageNew(wizard,mafWIZARDUSEGUI);

  mafGUI *gui = new mafGUI(page);

  page->AddGuiLowerUnderLeft(gui);
  page->RemoveGuiLowerUnderLeft(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( !page->m_GuiLowerUnderLeft->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}

//----------------------------------------------------------------------------
void mafGUIWizardPageNewTest::TestSetNextPage()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
  mafGUIWizardPageNew *page1 = new mafGUIWizardPageNew(wizard,mafWIZARDUSEGUI);
  mafGUIWizardPageNew *page2 = new mafGUIWizardPageNew(wizard,mafWIZARDUSEGUI);

  page1->SetNextPage(page2);

  CPPUNIT_ASSERT( page2->GetPrev() == page1 );
  CPPUNIT_ASSERT( page1->GetNext() == page2 );

  delete page2;
  delete page1;
  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardPageNewTest::TestSetGetZCropBounds()
//----------------------------------------------------------------------------
{
    mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
    mafGUIWizardPageNew *page = new mafGUIWizardPageNew(wizard,mafWIZARDUSEGUI,true);

    mafGUI *gui = new mafGUI(page);

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
void mafGUIWizardPageNewTest::TestUpdateWindowing()
//----------------------------------------------------------------------------
{
    mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
    mafGUIWizardPageNew *page1 = new mafGUIWizardPageNew(wizard,mafWIZARDUSEGUI);
    
    //smoke test
    page1->UpdateWindowing();

    delete page1;
    delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardPageNewTest::TestUpdateActor()
//----------------------------------------------------------------------------
{
    mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);
    mafGUIWizardPageNew *page1 = new mafGUIWizardPageNew(wizard,mafWIZARDUSEGUI);

    //smoke test
    page1->UpdateActor();

    delete page1;
    delete wizard;
}