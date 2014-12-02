/*=========================================================================

 Program: MAF2
 Module: mafGUIWizardTest
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
#include "mafGUIWizardTest.h"
#include "mafGUIWizard.h"
#include "mafGUIFrame.h"
#include "mafGUIWizardPage.h"
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
void mafGUIWizardTest::setUp()
//----------------------------------------------------------------------------
{
//   m_App = new TestApp();  // Instantiate the application class
//   m_App->OnInit();
//   m_App->argc = 0;        // set the number of input argument to 0
//   m_App->argv = NULL;     // set to NULL the input argument's parameters
//   wxTheApp->SetAppName("mafGUIWizardTest"); // Set the name for the application

  m_Win = new mafGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  wxModule::RegisterModules();
  wxModule::InitializeModules();

  m_Result = false;
}
//----------------------------------------------------------------------------
void mafGUIWizardTest::tearDown()
//----------------------------------------------------------------------------
{
  wxModule::CleanUpModules();

  delete m_Win;

  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafGUIWizardTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);

  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardTest::TestSetFirstPage()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);

  mafGUIWizardPage *page = new mafGUIWizardPage(wizard,medUSEGUI);

  wizard->SetFirstPage(page);

  CPPUNIT_ASSERT(page == wizard->GetFirstPage());

  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardTest::TestEnableChangePageOn()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);

  mafGUIWizardPage *page = new mafGUIWizardPage(wizard,medUSEGUI);

  wizard->EnableChangePageOn();

  CPPUNIT_ASSERT( wizard->GetEnableChangePage() == true );

  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardTest::TestEnableChangePageOff()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);

  wizard->EnableChangePageOff();

  CPPUNIT_ASSERT( wizard->GetEnableChangePage() == false );

  delete wizard;
}
//----------------------------------------------------------------------------
void mafGUIWizardTest::TestRun()
//----------------------------------------------------------------------------
{
  mafGUIWizard *wizard = new mafGUIWizard(_("TEST"),true);

  mafGUIWizardPage *page = new mafGUIWizardPage(wizard,medUSEGUI);

  wizard->SetFirstPage(page);

  /** Seems impossible to test method Run() */
  //wizard->Run();

  delete page;
  delete wizard;
}
