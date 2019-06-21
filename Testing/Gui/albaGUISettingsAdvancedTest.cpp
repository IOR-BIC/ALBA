/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUISettingsAdvancedTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "albaGuiTests.h"
#include "albaGUISettingsAdvancedTest.h"
#include "albaGUISettingsAdvanced.h"



#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void albaGUISettingsAdvancedTest::AfterTest()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  config->DeleteAll();
  cppDEL(config);
}
//---------------------------------------------------------
void albaGUISettingsAdvancedTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  albaGUISettingsAdvanced *settings = new albaGUISettingsAdvanced(NULL,"test");
  cppDEL(settings);
}
//---------------------------------------------------------
void albaGUISettingsAdvancedTest::TestSetConversionType()
//---------------------------------------------------------
{
  albaGUISettingsAdvanced *settings = new albaGUISettingsAdvanced(NULL,"test");
  settings->SetConversionType(albaGUISettingsAdvanced::NONE);
  m_Result = settings->GetConversionType() == albaGUISettingsAdvanced::NONE;
  TEST_RESULT;
  settings->SetConversionType(albaGUISettingsAdvanced::mm2m);
  m_Result = settings->GetConversionType() == albaGUISettingsAdvanced::mm2m;
  TEST_RESULT;
  cppDEL(settings);
}
//---------------------------------------------------------
void albaGUISettingsAdvancedTest::TestSettingsAdvanced()
//---------------------------------------------------------
{
  albaGUISettingsAdvanced *settings = new albaGUISettingsAdvanced(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);

  long long_item;

  m_Result = config->Read("ConversionUnits", &long_item);
  TEST_RESULT;

  m_Result = long_item == albaGUISettingsAdvanced::NONE;
  TEST_RESULT;

  settings->SetConversionType(albaGUISettingsAdvanced::mm2m);

  m_Result = config->Read("ConversionUnits", &long_item);
  TEST_RESULT;

  m_Result = long_item == albaGUISettingsAdvanced::mm2m;
  TEST_RESULT;

  cppDEL(config);
  cppDEL(settings);
}