/*=========================================================================

 Program: MAF2
 Module: mafGUIDicomSettingsTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
#include "mafGuiTests.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafGUIDicomSettingsTest.h"
#include "mafGUIDicomSettings.h"





#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void mafGUIDicomSettingsTest::BeforeTest()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  config->DeleteAll();
  cppDEL(config);
}
//----------------------------------------------------------------------------
void mafGUIDicomSettingsTest::AfterTest()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  config->DeleteAll();
  cppDEL(config);
}
//---------------------------------------------------------
void mafGUIDicomSettingsTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  mafGUIDicomSettings *settings = new mafGUIDicomSettings(NULL,"test");
  cppDEL(settings);
}
//---------------------------------------------------------
void mafGUIDicomSettingsTest::TestGetBuildStep()
{
  mafGUIDicomSettings *settings = new mafGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  int buildStep = settings->GetBuildStep();
  m_Result = buildStep == mafGUIDicomSettings::ID_1X;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("StepOfBuild",mafGUIDicomSettings::ID_2X);
  settings->InitializeSettings();
  buildStep = settings->GetBuildStep();
  m_Result = buildStep == mafGUIDicomSettings::ID_2X;

  TEST_RESULT;

  config->Write("StepOfBuild",mafGUIDicomSettings::ID_4X);
  settings->InitializeSettings();
  buildStep = settings->GetBuildStep();
  m_Result = buildStep == mafGUIDicomSettings::ID_4X;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  mafEvent e;
  e.SetId(mafGUIDicomSettings::ID_STEP);

  settings->m_Step = mafGUIDicomSettings::ID_1X;
  settings->OnEvent(&e);
  config->Read("StepOfBuild",&buildStep);
  m_Result = buildStep == mafGUIDicomSettings::ID_1X;

  TEST_RESULT;

  settings->m_Step = mafGUIDicomSettings::ID_4X;
  settings->OnEvent(&e);
  config->Read("StepOfBuild",&buildStep);
  m_Result = buildStep == mafGUIDicomSettings::ID_4X;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
//---------------------------------------------------------
void mafGUIDicomSettingsTest::TestAutoVMEType()
//---------------------------------------------------------
{
  mafGUIDicomSettings *settings = new mafGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  int autoVMEType = settings->AutoVMEType();
  m_Result = autoVMEType == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("AutoVMEType",TRUE);
  settings->InitializeSettings();
  autoVMEType = settings->AutoVMEType();
  m_Result = autoVMEType == TRUE;

  TEST_RESULT;

  config->Write("AutoVMEType",FALSE);
  settings->InitializeSettings();
  autoVMEType = settings->AutoVMEType();
  m_Result = autoVMEType == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  mafEvent e;
  e.SetId(mafGUIDicomSettings::ID_AUTO_VME_TYPE);

  settings->m_AutoVMEType = TRUE;
  settings->OnEvent(&e);
  config->Read("AutoVMEType",&autoVMEType);
  m_Result = autoVMEType == TRUE;

  TEST_RESULT;

  settings->m_AutoVMEType = FALSE;
  settings->OnEvent(&e);
  config->Read("AutoVMEType",&autoVMEType);
  m_Result = autoVMEType == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
//---------------------------------------------------------
void mafGUIDicomSettingsTest::TestGetVMEType()
//---------------------------------------------------------
{
  mafGUIDicomSettings *settings = new mafGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  int vmeType = settings->GetVMEType();
  m_Result = vmeType == 0;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("VMEType",1);
  settings->InitializeSettings();
  vmeType = settings->GetVMEType();
  m_Result = vmeType == 1;

  TEST_RESULT;

  config->Write("VMEType",2);
  settings->InitializeSettings();
  vmeType = settings->GetVMEType();
  m_Result = vmeType == 2;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  mafEvent e;
  e.SetId(mafGUIDicomSettings::ID_SETTING_VME_TYPE);

  settings->m_OutputType = 1;
  settings->OnEvent(&e);
  config->Read("VMEType",&vmeType);
  m_Result = vmeType == 1;

  TEST_RESULT;

  settings->m_OutputType = 0;
  settings->OnEvent(&e);
  config->Read("VMEType",&vmeType);
  m_Result = vmeType == 0;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
