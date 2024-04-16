/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIDicomSettingsTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
#include "albaGuiTests.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "albaGUIDicomSettingsTest.h"
#include "albaGUIDicomSettings.h"





#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void albaGUIDicomSettingsTest::BeforeTest()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  config->DeleteAll();
  cppDEL(config);
}
//----------------------------------------------------------------------------
void albaGUIDicomSettingsTest::AfterTest()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  config->DeleteAll();
  cppDEL(config);
}
//---------------------------------------------------------
void albaGUIDicomSettingsTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  albaGUIDicomSettings *settings = new albaGUIDicomSettings(NULL,"test");
  cppDEL(settings);
}
//---------------------------------------------------------
void albaGUIDicomSettingsTest::TestGetBuildStep()
{
  albaGUIDicomSettings *settings = new albaGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  int buildStep = settings->GetBuildStep();
  m_Result = buildStep == albaGUIDicomSettings::ID_1X;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("StepOfBuild",albaGUIDicomSettings::ID_2X);
  settings->InitializeSettings();
  buildStep = settings->GetBuildStep();
  m_Result = buildStep == albaGUIDicomSettings::ID_2X;

  TEST_RESULT;

  config->Write("StepOfBuild",albaGUIDicomSettings::ID_4X);
  settings->InitializeSettings();
  buildStep = settings->GetBuildStep();
  m_Result = buildStep == albaGUIDicomSettings::ID_4X;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  albaEvent e;
  e.SetId(albaGUIDicomSettings::ID_STEP);

  settings->m_Step = albaGUIDicomSettings::ID_1X;
  settings->OnEvent(&e);
  config->Read("StepOfBuild",&buildStep);
  m_Result = buildStep == albaGUIDicomSettings::ID_1X;

  TEST_RESULT;

  settings->m_Step = albaGUIDicomSettings::ID_4X;
  settings->OnEvent(&e);
  config->Read("StepOfBuild",&buildStep);
  m_Result = buildStep == albaGUIDicomSettings::ID_4X;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
//---------------------------------------------------------
void albaGUIDicomSettingsTest::TestAutoVMEType()
//---------------------------------------------------------
{
  albaGUIDicomSettings *settings = new albaGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  int autoVMEType = settings->GetAutoVMEType();
  m_Result = autoVMEType == false;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("AutoVMEType",true);
  settings->InitializeSettings();
  autoVMEType = settings->GetAutoVMEType();
  m_Result = autoVMEType == true;

  TEST_RESULT;

  config->Write("AutoVMEType",false);
  settings->InitializeSettings();
  autoVMEType = settings->GetAutoVMEType();
  m_Result = autoVMEType == false;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  albaEvent e;
  e.SetId(albaGUIDicomSettings::ID_AUTO_VME_TYPE);

  settings->m_AutoVMEType = true;
  settings->OnEvent(&e);
  config->Read("AutoVMEType",&autoVMEType);
  m_Result = autoVMEType == true;

  TEST_RESULT;

  settings->m_AutoVMEType = false;
  settings->OnEvent(&e);
  config->Read("AutoVMEType",&autoVMEType);
  m_Result = autoVMEType == false;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
//---------------------------------------------------------
void albaGUIDicomSettingsTest::TestGetVMEType()
//---------------------------------------------------------
{
  albaGUIDicomSettings *settings = new albaGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  int vmeType = settings->GetOutputType();
  m_Result = vmeType == 0;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("VMEType",1);
  settings->InitializeSettings();
  vmeType = settings->GetOutputType();
  m_Result = vmeType == 1;

  TEST_RESULT;

  config->Write("VMEType",2);
  settings->InitializeSettings();
  vmeType = settings->GetOutputType();
  m_Result = vmeType == 2;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  albaEvent e;
  e.SetId(albaGUIDicomSettings::ID_SETTING_VME_TYPE);

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
