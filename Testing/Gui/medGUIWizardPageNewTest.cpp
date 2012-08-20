/*=========================================================================

 Program: MAF2Medical
 Module: medGUIWizardPageNewTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "medDefines.h"
#include "medDecl.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "medGUIWizardPageNewTest.h"
#include "medGUIWizard.h"
#include "mafGUIFrame.h"
#include "medGUIWizardPageNew.h"
#include "wx/module.h"

// ===============================================================================
// Helper class used to build a fake application needed by the wxConfig
// to store settings into the registry with the same name of the application
// ===============================================================================
class TestApp : public wxApp
// ===============================================================================
{
public:
  bool OnInit();
  int  OnExit();
};
DECLARE_APP(TestApp)

IMPLEMENT_APP(TestApp)
//--------------------------------------------------------------------------------
bool TestApp::OnInit()
//--------------------------------------------------------------------------------
{
  return TRUE;
}
//--------------------------------------------------------------------------------
int TestApp::OnExit()
//--------------------------------------------------------------------------------
{
  wxApp::CleanUp();
  return 0;
}
// ===============================================================================

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void medGUIWizardPageNewTest::setUp()
//----------------------------------------------------------------------------
{
//   m_App = new TestApp();  // Instantiate the application class
//   m_App->OnInit();
//   m_App->argc = 0;        // set the number of input argument to 0
//   m_App->argv = NULL;     // set to NULL the input argument's parameters
//   wxTheApp->SetAppName("medGUIWizardTest"); // Set the name for the application

  m_Win = new mafGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  wxModule::RegisterModules();
  wxModule::InitializeModules();

  m_Result = false;
}
//----------------------------------------------------------------------------
void medGUIWizardPageNewTest::tearDown()
//----------------------------------------------------------------------------
{
  wxModule::CleanUpModules();

  delete m_Win;

  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void medGUIWizardPageNewTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  medGUIWizard *wizard = new medGUIWizard(_("TEST"),true);
  medGUIWizardPageNew *page = new medGUIWizardPageNew(wizard,medUSEGUI);

  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void medGUIWizardPageNewTest::TestAddGuiLowerLeft()
//----------------------------------------------------------------------------
{
  medGUIWizard *wizard = new medGUIWizard(_("TEST"),true);
  medGUIWizardPageNew *page = new medGUIWizardPageNew(wizard,medUSEGUI);

  mafGUI *gui = new mafGUI(page);

  page->AddGuiLowerLeft(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( page->m_GuiLowerLeft->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void medGUIWizardPageNewTest::TestAddGuiLowerRight()
//----------------------------------------------------------------------------
{
  medGUIWizard *wizard = new medGUIWizard(_("TEST"),true);
  medGUIWizardPageNew *page = new medGUIWizardPageNew(wizard,medUSEGUI);

  mafGUI *gui = new mafGUI(page);

  page->AddGuiLowerUnderCenter(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( page->m_GuiLowerUnderCenter->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void medGUIWizardPageNewTest::TestAddGuiLowerCenter()
//----------------------------------------------------------------------------
{
  medGUIWizard *wizard = new medGUIWizard(_("TEST"),true);
  medGUIWizardPageNew *page = new medGUIWizardPageNew(wizard,medUSEGUI);

  mafGUI *gui = new mafGUI(page);

  page->AddGuiLowerUnderLeft(gui);

  //Remove return true only if the gui has been added before
  CPPUNIT_ASSERT( page->m_GuiLowerUnderLeft->Remove(gui) );

  delete gui;
  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void medGUIWizardPageNewTest::TestAddGuiLowerUnderLeft()
//----------------------------------------------------------------------------
{
    medGUIWizard *wizard = new medGUIWizard(_("TEST"),true);
    medGUIWizardPageNew *page = new medGUIWizardPageNew(wizard,medUSEGUI);

    mafGUI *gui = new mafGUI(page);

    page->AddGuiLowerUnderLeft(gui);

    //Remove return true only if the gui has been added before
    CPPUNIT_ASSERT( page->m_GuiLowerUnderLeft->Remove(gui) );

    delete gui;
    delete page;
    delete wizard;
}
//----------------------------------------------------------------------------
void medGUIWizardPageNewTest::TestAddGuiLowerUnderCenter()
//----------------------------------------------------------------------------
{
    medGUIWizard *wizard = new medGUIWizard(_("TEST"),true);
    medGUIWizardPageNew *page = new medGUIWizardPageNew(wizard,medUSEGUI);

    mafGUI *gui = new mafGUI(page);

    page->AddGuiLowerUnderCenter(gui);

    //Remove return true only if the gui has been added before
    CPPUNIT_ASSERT( page->m_GuiLowerUnderCenter->Remove(gui) );

    delete gui;
    delete page;
    delete wizard;
}
//----------------------------------------------------------------------------
void medGUIWizardPageNewTest::TestRemoveGuiLowerLeft()
//----------------------------------------------------------------------------
{
  medGUIWizard *wizard = new medGUIWizard(_("TEST"),true);
  medGUIWizardPageNew *page = new medGUIWizardPageNew(wizard,medUSEGUI);

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
void medGUIWizardPageNewTest::TestRemoveGuiLowerUnderLeft()
//----------------------------------------------------------------------------
{
  medGUIWizard *wizard = new medGUIWizard(_("TEST"),true);
  medGUIWizardPageNew *page = new medGUIWizardPageNew(wizard,medUSEGUI);

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
void medGUIWizardPageNewTest::TestSetNextPage()
//----------------------------------------------------------------------------
{
  medGUIWizard *wizard = new medGUIWizard(_("TEST"),true);
  medGUIWizardPageNew *page1 = new medGUIWizardPageNew(wizard,medUSEGUI);
  medGUIWizardPageNew *page2 = new medGUIWizardPageNew(wizard,medUSEGUI);

  page1->SetNextPage(page2);

  CPPUNIT_ASSERT( page2->GetPrev() == page1 );
  CPPUNIT_ASSERT( page1->GetNext() == page2 );

  delete page2;
  delete page1;
  delete wizard;
}
//----------------------------------------------------------------------------
void medGUIWizardPageNewTest::TestSetGetZCropBounds()
//----------------------------------------------------------------------------
{
    medGUIWizard *wizard = new medGUIWizard(_("TEST"),true);
    medGUIWizardPageNew *page = new medGUIWizardPageNew(wizard,medUSEGUI,true);

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
void medGUIWizardPageNewTest::TestUpdateWindowing()
//----------------------------------------------------------------------------
{
    medGUIWizard *wizard = new medGUIWizard(_("TEST"),true);
    medGUIWizardPageNew *page1 = new medGUIWizardPageNew(wizard,medUSEGUI);
    
    //smoke test
    page1->UpdateWindowing();

    delete page1;
    delete wizard;
}
//----------------------------------------------------------------------------
void medGUIWizardPageNewTest::TestUpdateActor()
//----------------------------------------------------------------------------
{
    medGUIWizard *wizard = new medGUIWizard(_("TEST"),true);
    medGUIWizardPageNew *page1 = new medGUIWizardPageNew(wizard,medUSEGUI);

    //smoke test
    page1->UpdateActor();

    delete page1;
    delete wizard;
}