/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardSettingsTest
 Authors: Gianluigi Crimi
 
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
#include "albaCommonTests.h"
#include "albaWizardSettings.h"
#include "albaWizardSettingsTest.h"




#define TEST_RESULT CPPUNIT_ASSERT(m_Result)


//----------------------------------------------------------------------------
void albaWizardSettingsTest::AfterTest()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  config->DeleteAll();
  cppDEL(config);
}

//---------------------------------------------------------
void albaWizardSettingsTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  albaWizardSettings *settings = new albaWizardSettings(NULL,"test");
  cppDEL(settings);
}

//---------------------------------------------------------
void albaWizardSettingsTest::TestSettings()
//---------------------------------------------------------
{
  albaWizardSettings *settings = new albaWizardSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);

  long long_item;

  settings->SetShowInformationBoxes(true);
  CPPUNIT_ASSERT(settings->GetShowInformationBoxes()==true);

  settings->SetShowInformationBoxes(false);
  CPPUNIT_ASSERT(settings->GetShowInformationBoxes()==false);

  cppDEL(config);
  cppDEL(settings);
}