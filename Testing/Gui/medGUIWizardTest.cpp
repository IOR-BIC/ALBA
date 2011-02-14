/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUIWizardTest.cpp,v $
Language:  C++
Date:      $Date: 2011-02-14 08:47:30 $
Version:   $Revision: 1.1.2.3 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
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
#include "medGUIWizardTest.h"
#include "medGUIWizard.h"
#include "mafGUIFrame.h"
#include "medGUIWizardPage.h"
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
void medGUIWizardTest::setUp()
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
void medGUIWizardTest::tearDown()
//----------------------------------------------------------------------------
{
  wxModule::CleanUpModules();

  delete m_Win;

  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void medGUIWizardTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  medGUIWizard *wizard = new medGUIWizard(_("TEST"),true);

  delete wizard;
}
//----------------------------------------------------------------------------
void medGUIWizardTest::TestSetFirstPage()
//----------------------------------------------------------------------------
{
  medGUIWizard *wizard = new medGUIWizard(_("TEST"),true);

  medGUIWizardPage *page = new medGUIWizardPage(wizard,medUSEGUI);

  wizard->SetFirstPage(page);

  CPPUNIT_ASSERT(page == wizard->GetFirstPage());

  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void medGUIWizardTest::TestEnableChangePageOn()
//----------------------------------------------------------------------------
{
  medGUIWizard *wizard = new medGUIWizard(_("TEST"),true);

  medGUIWizardPage *page = new medGUIWizardPage(wizard,medUSEGUI);

  wizard->EnableChangePageOn();

  CPPUNIT_ASSERT( wizard->GetEnableChangePage() == true );

  delete page;
  delete wizard;
}
//----------------------------------------------------------------------------
void medGUIWizardTest::TestEnableChangePageOff()
//----------------------------------------------------------------------------
{
  medGUIWizard *wizard = new medGUIWizard(_("TEST"),true);

  wizard->EnableChangePageOff();

  CPPUNIT_ASSERT( wizard->GetEnableChangePage() == false );

  delete wizard;
}
//----------------------------------------------------------------------------
void medGUIWizardTest::TestRun()
//----------------------------------------------------------------------------
{
  medGUIWizard *wizard = new medGUIWizard(_("TEST"),true);

  medGUIWizardPage *page = new medGUIWizardPage(wizard,medUSEGUI);

  wizard->SetFirstPage(page);

  /** Seems impossible to test method Run() */
  //wizard->Run();

  delete page;
  delete wizard;
}
