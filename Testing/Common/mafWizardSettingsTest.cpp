/*=========================================================================

 Program: MAF2
 Module: mafWizardSettingsTest
 Authors: Gianluigi Crimi
 
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
#include "mafCommonTests.h"
#include "mafWizardSettings.h"
#include "mafWizardSettingsTest.h"




#define TEST_RESULT CPPUNIT_ASSERT(m_Result)


//----------------------------------------------------------------------------
void mafWizardSettingsTest::AfterTest()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  config->DeleteAll();
  cppDEL(config);
}

//---------------------------------------------------------
void mafWizardSettingsTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  mafWizardSettings *settings = new mafWizardSettings(NULL,"test");
  cppDEL(settings);
}

//---------------------------------------------------------
void mafWizardSettingsTest::TestSettings()
//---------------------------------------------------------
{
  mafWizardSettings *settings = new mafWizardSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);

  long long_item;

  settings->SetShowInformationBoxes(true);
  CPPUNIT_ASSERT(settings->GetShowInformationBoxes()==true);

  settings->SetShowInformationBoxes(false);
  CPPUNIT_ASSERT(settings->GetShowInformationBoxes()==false);

  cppDEL(config);
  cppDEL(settings);
}