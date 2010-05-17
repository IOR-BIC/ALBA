/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUISettingsAdvancedTest.cpp,v $
Language:  C++
Date:      $Date: 2010-05-17 13:20:34 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "medGUISettingsAdvancedTest.h"
#include "medGUISettingsAdvanced.h"

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
void medGUISettingsAdvancedTest::setUp()
//----------------------------------------------------------------------------
{
  m_App = new TestApp();  // Instantiate the application class
  m_App->argc = 0;        // set the number of input argument to 0
  m_App->argv = NULL;     // set to NULL the input argument's parameters
  wxTheApp->SetAppName("medGUISettingsAdvancedTest"); // Set the name for the application

  m_Result = false;
}
//----------------------------------------------------------------------------
void medGUISettingsAdvancedTest::tearDown()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  config->DeleteAll();
  cppDEL(config);
  cppDEL(m_App);
}
//---------------------------------------------------------
void medGUISettingsAdvancedTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  medGUISettingsAdvanced *settings = new medGUISettingsAdvanced(NULL,"test");
  cppDEL(settings);
}
//---------------------------------------------------------
void medGUISettingsAdvancedTest::TestSetConversionType()
//---------------------------------------------------------
{
  medGUISettingsAdvanced *settings = new medGUISettingsAdvanced(NULL,"test");
  settings->SetConversionType(medGUISettingsAdvanced::NONE);
  m_Result = settings->GetConversionType() == medGUISettingsAdvanced::NONE;
  TEST_RESULT;
  settings->SetConversionType(medGUISettingsAdvanced::mm2m);
  m_Result = settings->GetConversionType() == medGUISettingsAdvanced::mm2m;
  TEST_RESULT;
  cppDEL(settings);
}
//---------------------------------------------------------
void medGUISettingsAdvancedTest::TestSettingsAdvanced()
//---------------------------------------------------------
{
  medGUISettingsAdvanced *settings = new medGUISettingsAdvanced(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);

  long long_item;

  m_Result = config->Read("ConversionUnits", &long_item);
  TEST_RESULT;

  m_Result = long_item == medGUISettingsAdvanced::NONE;
  TEST_RESULT;

  settings->SetConversionType(medGUISettingsAdvanced::mm2m);

  m_Result = config->Read("ConversionUnits", &long_item);
  TEST_RESULT;

  m_Result = long_item == medGUISettingsAdvanced::mm2m;
  TEST_RESULT;

  cppDEL(config);
  cppDEL(settings);
}