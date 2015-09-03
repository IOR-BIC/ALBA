/*=========================================================================

 Program: MAF2
 Module: mafGUISettingsAdvancedTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafGuiTests.h"
#include "mafGUISettingsAdvancedTest.h"
#include "mafGUISettingsAdvanced.h"



#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void mafGUISettingsAdvancedTest::AfterTest()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  config->DeleteAll();
  cppDEL(config);
}
//---------------------------------------------------------
void mafGUISettingsAdvancedTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  mafGUISettingsAdvanced *settings = new mafGUISettingsAdvanced(NULL,"test");
  cppDEL(settings);
}
//---------------------------------------------------------
void mafGUISettingsAdvancedTest::TestSetConversionType()
//---------------------------------------------------------
{
  mafGUISettingsAdvanced *settings = new mafGUISettingsAdvanced(NULL,"test");
  settings->SetConversionType(mafGUISettingsAdvanced::NONE);
  m_Result = settings->GetConversionType() == mafGUISettingsAdvanced::NONE;
  TEST_RESULT;
  settings->SetConversionType(mafGUISettingsAdvanced::mm2m);
  m_Result = settings->GetConversionType() == mafGUISettingsAdvanced::mm2m;
  TEST_RESULT;
  cppDEL(settings);
}
//---------------------------------------------------------
void mafGUISettingsAdvancedTest::TestSettingsAdvanced()
//---------------------------------------------------------
{
  mafGUISettingsAdvanced *settings = new mafGUISettingsAdvanced(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);

  long long_item;

  m_Result = config->Read("ConversionUnits", &long_item);
  TEST_RESULT;

  m_Result = long_item == mafGUISettingsAdvanced::NONE;
  TEST_RESULT;

  settings->SetConversionType(mafGUISettingsAdvanced::mm2m);

  m_Result = config->Read("ConversionUnits", &long_item);
  TEST_RESULT;

  m_Result = long_item == mafGUISettingsAdvanced::mm2m;
  TEST_RESULT;

  cppDEL(config);
  cppDEL(settings);
}