/*=========================================================================

 Program: MAF2Medical
 Module: medWizardSettingsTest
 Authors: Gianluigi Crimi
 
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

#include <cppunit/config/SourcePrefix.h>
#include "medWizardSettings.h"
#include "medWizardSettingsTest.h"

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
void medWizardSettingsTest::setUp()
//----------------------------------------------------------------------------
{
  m_App = new TestApp();  // Instantiate the application class
  m_App->argc = 0;        // set the number of input argument to 0
  m_App->argv = NULL;     // set to NULL the input argument's parameters
  wxTheApp->SetAppName("medGUISettingsAdvancedTest"); // Set the name for the application
}

//----------------------------------------------------------------------------
void medWizardSettingsTest::tearDown()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  config->DeleteAll();
  cppDEL(config);
  cppDEL(m_App);
}

//---------------------------------------------------------
void medWizardSettingsTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  medWizardSettings *settings = new medWizardSettings(NULL,"test");
  cppDEL(settings);
}

//---------------------------------------------------------
void medWizardSettingsTest::TestSettings()
//---------------------------------------------------------
{
  medWizardSettings *settings = new medWizardSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);

  long long_item;

  settings->SetShowInformationBoxes(true);
  CPPUNIT_ASSERT(settings->GetShowInformationBoxes()==true);

  settings->SetShowInformationBoxes(false);
  CPPUNIT_ASSERT(settings->GetShowInformationBoxes()==false);

  cppDEL(config);
  cppDEL(settings);
}