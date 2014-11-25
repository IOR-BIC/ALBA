/*=========================================================================

 Program: MAF2Medical
 Module: medGUIDicomSettingsTest
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
#include "medGUIDicomSettingsTest.h"
#include "medGUIDicomSettings.h"

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
void medGUIDicomSettingsTest::setUp()
//----------------------------------------------------------------------------
{
  m_App = new TestApp();  // Instantiate the application class
  m_App->argc = 0;        // set the number of input argument to 0
  m_App->argv = NULL;     // set to NULL the input argument's parameters
  wxTheApp->SetAppName("medGUIDicomSettingsTest"); // Set the name for the application

  wxConfig *config = new wxConfig(wxEmptyString);
  config->DeleteAll();
  cppDEL(config);

  m_Result = false;
}
//----------------------------------------------------------------------------
void medGUIDicomSettingsTest::tearDown()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  config->DeleteAll();
  cppDEL(config);
  cppDEL(m_App);
}
//---------------------------------------------------------
void medGUIDicomSettingsTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");
  cppDEL(settings);
}
//---------------------------------------------------------
void medGUIDicomSettingsTest::TestGetDictionary()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");

  mafString dic = settings->GetDictionary();
  m_Result = dic == "";

  TEST_RESULT;

  cppDEL(settings);
}
//---------------------------------------------------------
void medGUIDicomSettingsTest::TestAutoCropPosition()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  int autoCropPosition = settings->AutoCropPosition();
  m_Result = autoCropPosition == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("AutoCropPos",TRUE);
  settings->InitializeSettings();
  autoCropPosition = settings->AutoCropPosition();
  m_Result = autoCropPosition == TRUE;

  TEST_RESULT;

  config->Write("AutoCropPos",FALSE);
  settings->InitializeSettings();
  autoCropPosition = settings->AutoCropPosition();
  m_Result = autoCropPosition == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  mafEvent e;
  e.SetId(medGUIDicomSettings::ID_AUTO_POS_CROP);

  settings->m_AutoCropPos = TRUE;
  settings->OnEvent(&e);
  config->Read("AutoCropPos",&autoCropPosition);
  m_Result = autoCropPosition == TRUE;

  TEST_RESULT;

  settings->m_AutoCropPos = FALSE;
  settings->OnEvent(&e);
  config->Read("AutoCropPos",&autoCropPosition);
  m_Result = autoCropPosition == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
//---------------------------------------------------------
void medGUIDicomSettingsTest::TestEnableNumberOfTime()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  int enableNumberOfTime = settings->EnableNumberOfTime();
  m_Result = enableNumberOfTime == TRUE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("EnableTimeBar",FALSE);
  settings->InitializeSettings();
  enableNumberOfTime = settings->EnableNumberOfTime();
  m_Result = enableNumberOfTime == FALSE;

  TEST_RESULT;

  config->Write("EnableTimeBar",TRUE);
  settings->InitializeSettings();
  enableNumberOfTime = settings->EnableNumberOfTime();
  m_Result = enableNumberOfTime == TRUE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  mafEvent e;
  e.SetId(medGUIDicomSettings::ID_ENALBLE_TIME_BAR);

  settings->m_EnableNumberOfTime = FALSE;
  settings->OnEvent(&e);
  config->Read("EnableTimeBar",&enableNumberOfTime);
  m_Result = enableNumberOfTime == FALSE;

  TEST_RESULT;

  settings->m_EnableNumberOfTime = TRUE;
  settings->OnEvent(&e);
  config->Read("EnableTimeBar",&enableNumberOfTime);
  m_Result = enableNumberOfTime == TRUE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
//---------------------------------------------------------
void medGUIDicomSettingsTest::TestEnableNumberOfSlice()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  int enableNumberOfSlice = settings->EnableNumberOfSlice();
  m_Result = enableNumberOfSlice == TRUE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("EnableNumberOfSlice",FALSE);
  settings->InitializeSettings();
  enableNumberOfSlice = settings->EnableNumberOfSlice();
  m_Result = enableNumberOfSlice == FALSE;

  TEST_RESULT;

  config->Write("EnableNumberOfSlice",TRUE);
  settings->InitializeSettings();
  enableNumberOfSlice = settings->EnableNumberOfSlice();
  m_Result = enableNumberOfSlice == TRUE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  mafEvent e;
  e.SetId(medGUIDicomSettings::ID_ENALBLE_NUMBER_OF_SLICE);

  settings->m_EnableNumberOfSlice = FALSE;
  settings->OnEvent(&e);
  config->Read("EnableNumberOfSlice",&enableNumberOfSlice);
  m_Result = enableNumberOfSlice == FALSE;

  TEST_RESULT;

  settings->m_EnableNumberOfSlice = TRUE;
  settings->OnEvent(&e);
  config->Read("EnableNumberOfSlice",&enableNumberOfSlice);
  m_Result = enableNumberOfSlice == TRUE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
//---------------------------------------------------------
void medGUIDicomSettingsTest::TestGetBuildStep()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  int buildStep = settings->GetBuildStep();
  m_Result = buildStep == medGUIDicomSettings::ID_1X;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("StepOfBuild",medGUIDicomSettings::ID_2X);
  settings->InitializeSettings();
  buildStep = settings->GetBuildStep();
  m_Result = buildStep == medGUIDicomSettings::ID_2X;

  TEST_RESULT;

  config->Write("StepOfBuild",medGUIDicomSettings::ID_4X);
  settings->InitializeSettings();
  buildStep = settings->GetBuildStep();
  m_Result = buildStep == medGUIDicomSettings::ID_4X;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  mafEvent e;
  e.SetId(medGUIDicomSettings::ID_STEP);

  settings->m_Step = medGUIDicomSettings::ID_1X;
  settings->OnEvent(&e);
  config->Read("StepOfBuild",&buildStep);
  m_Result = buildStep == medGUIDicomSettings::ID_1X;

  TEST_RESULT;

  settings->m_Step = medGUIDicomSettings::ID_4X;
  settings->OnEvent(&e);
  config->Read("StepOfBuild",&buildStep);
  m_Result = buildStep == medGUIDicomSettings::ID_4X;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
//---------------------------------------------------------
void medGUIDicomSettingsTest::TestEnableChangeSide()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  int enableChageSide = settings->EnableChangeSide();
  m_Result = enableChageSide == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("EnableSide",TRUE);
  settings->InitializeSettings();
  enableChageSide = settings->EnableChangeSide();
  m_Result = enableChageSide == TRUE;

  TEST_RESULT;

  config->Write("EnableSide",FALSE);
  settings->InitializeSettings();
  enableChageSide = settings->EnableChangeSide();
  m_Result = enableChageSide == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  mafEvent e;
  e.SetId(medGUIDicomSettings::ID_SIDE);

  settings->m_EnableChangeSide = TRUE;
  settings->OnEvent(&e);
  config->Read("EnableSide",&enableChageSide);
  m_Result = enableChageSide == TRUE;

  TEST_RESULT;

  settings->m_EnableChangeSide = FALSE;
  settings->OnEvent(&e);
  config->Read("EnableSide",&enableChageSide);
  m_Result = enableChageSide == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
//---------------------------------------------------------
void medGUIDicomSettingsTest::TestEnableDiscardPosition()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  int enableDiscardPosition = settings->EnableDiscardPosition();
  m_Result = enableDiscardPosition == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("EnableDiscardPosition",TRUE);
  settings->InitializeSettings();
  enableDiscardPosition = settings->EnableDiscardPosition();
  m_Result = enableDiscardPosition == TRUE;

  TEST_RESULT;

  config->Write("EnableDiscardPosition",FALSE);
  settings->InitializeSettings();
  enableDiscardPosition = settings->EnableDiscardPosition();
  m_Result = enableDiscardPosition == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  mafEvent e;
  e.SetId(medGUIDicomSettings::ID_DISCARD_ORIGIN);

  settings->m_EnableDiscardPosition = TRUE;
  settings->OnEvent(&e);
  config->Read("EnableDiscardPosition",&enableDiscardPosition);
  m_Result = enableDiscardPosition == TRUE;

  TEST_RESULT;

  settings->m_EnableDiscardPosition = FALSE;
  settings->OnEvent(&e);
  config->Read("EnableDiscardPosition",&enableDiscardPosition);
  m_Result = enableDiscardPosition == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
//---------------------------------------------------------
void medGUIDicomSettingsTest::TestEnableResampleVolume()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  int enableResampleVolume = settings->EnableResampleVolume();
  m_Result = enableResampleVolume == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("EnableResampleVolume",TRUE);
  settings->InitializeSettings();
  enableResampleVolume = settings->EnableResampleVolume();
  m_Result = enableResampleVolume == TRUE;

  TEST_RESULT;

  config->Write("EnableResampleVolume",FALSE);
  settings->InitializeSettings();
  enableResampleVolume = settings->EnableResampleVolume();
  m_Result = enableResampleVolume == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  mafEvent e;
  e.SetId(medGUIDicomSettings::ID_RESAMPLE_VOLUME);

  settings->m_EnableResampleVolume = TRUE;
  settings->OnEvent(&e);
  config->Read("EnableResampleVolume",&enableResampleVolume);
  m_Result = enableResampleVolume == TRUE;

  TEST_RESULT;

  settings->m_EnableResampleVolume = FALSE;
  settings->OnEvent(&e);
  config->Read("EnableResampleVolume",&enableResampleVolume);
  m_Result = enableResampleVolume == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
//---------------------------------------------------------
void medGUIDicomSettingsTest::TestEnableRescaleTo16Bit()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  int enableRescaleTo16Bit = settings->EnableRescaleTo16Bit();
  m_Result = enableRescaleTo16Bit == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("EnableRescaleTo16Bit",TRUE);
  settings->InitializeSettings();
  enableRescaleTo16Bit = settings->EnableRescaleTo16Bit();
  m_Result = enableRescaleTo16Bit == TRUE;

  TEST_RESULT;

  config->Write("EnableRescaleTo16Bit",FALSE);
  settings->InitializeSettings();
  enableRescaleTo16Bit = settings->EnableRescaleTo16Bit();
  m_Result = enableRescaleTo16Bit == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  mafEvent e;
  e.SetId(medGUIDicomSettings::ID_RESCALE_TO_16_BIT);

  settings->m_EnableRescaleTo16Bit = TRUE;
  settings->OnEvent(&e);
  config->Read("EnableRescaleTo16Bit",&enableRescaleTo16Bit);
  m_Result = enableRescaleTo16Bit == TRUE;

  TEST_RESULT;

  settings->m_EnableRescaleTo16Bit = FALSE;
  settings->OnEvent(&e);
  config->Read("EnableRescaleTo16Bit",&enableRescaleTo16Bit);
  m_Result = enableRescaleTo16Bit == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
//---------------------------------------------------------
void medGUIDicomSettingsTest::TestEnableVisualizationPosition()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  int enableVisualizationPosition = settings->EnableVisualizationPosition();
  m_Result = enableVisualizationPosition == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("EnableVisualizationPosition",TRUE);
  settings->InitializeSettings();
  enableVisualizationPosition = settings->EnableVisualizationPosition();
  m_Result = enableVisualizationPosition == TRUE;

  TEST_RESULT;

  config->Write("EnableVisualizationPosition",FALSE);
  settings->InitializeSettings();
  enableVisualizationPosition = settings->EnableVisualizationPosition();
  m_Result = enableVisualizationPosition == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  mafEvent e;
  e.SetId(medGUIDicomSettings::ID_ENABLE_POS_INFO);

  settings->m_VisualizePosition = TRUE;
  settings->OnEvent(&e);
  config->Read("EnableVisualizationPosition",&enableVisualizationPosition);
  m_Result = enableVisualizationPosition == TRUE;

  TEST_RESULT;

  settings->m_VisualizePosition = FALSE;
  settings->OnEvent(&e);
  config->Read("EnableVisualizationPosition",&enableVisualizationPosition);
  m_Result = enableVisualizationPosition == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
//---------------------------------------------------------
void medGUIDicomSettingsTest::TestEnableZCrop()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  int enableZCrop = settings->EnableZCrop();
  m_Result = enableZCrop == TRUE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("EnableZCrop",FALSE);
  settings->InitializeSettings();
  enableZCrop = settings->EnableZCrop();
  m_Result = enableZCrop == FALSE;

  TEST_RESULT;

  config->Write("EnableZCrop",TRUE);
  settings->InitializeSettings();
  enableZCrop = settings->EnableZCrop();
  m_Result = enableZCrop == TRUE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  mafEvent e;
  e.SetId(medGUIDicomSettings::ID_Z_CROP);

  settings->m_EnableZCrop = FALSE;
  settings->OnEvent(&e);
  config->Read("EnableZCrop",&enableZCrop);
  m_Result = enableZCrop == FALSE;

  TEST_RESULT;

  settings->m_EnableZCrop = TRUE;
  settings->OnEvent(&e);
  config->Read("EnableZCrop",&enableZCrop);
  m_Result = enableZCrop == TRUE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
//---------------------------------------------------------
void medGUIDicomSettingsTest::TestEnableScalarTolerance()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  int enableScalarTolerance = settings->EnableScalarTolerance();
  m_Result = enableScalarTolerance == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("EnableScalarDistance",TRUE);
  settings->InitializeSettings();
  enableScalarTolerance = settings->EnableScalarTolerance();
  m_Result = enableScalarTolerance == TRUE;

  TEST_RESULT;

  config->Write("EnableScalarDistance",FALSE);
  settings->InitializeSettings();
  enableScalarTolerance = settings->EnableScalarTolerance();
  m_Result = enableScalarTolerance == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  mafEvent e;
  e.SetId(medGUIDicomSettings::ID_SCALAR_DISTANCE_TOLERANCE);

  settings->m_ScalarTolerance = TRUE;
  settings->OnEvent(&e);
  config->Read("EnableScalarDistance",&enableScalarTolerance);
  m_Result = enableScalarTolerance == TRUE;

  TEST_RESULT;

  settings->m_ScalarTolerance = FALSE;
  settings->OnEvent(&e);
  config->Read("EnableScalarDistance",&enableScalarTolerance);
  m_Result = enableScalarTolerance == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
//---------------------------------------------------------
void medGUIDicomSettingsTest::TestGetScalarTolerance()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  double scalarTolerance = settings->GetScalarTolerance();
  m_Result = scalarTolerance == 0.3;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("ScalarDistance",0.8);
  settings->InitializeSettings();
  scalarTolerance = settings->GetScalarTolerance();
  m_Result = scalarTolerance == 0.8;

  TEST_RESULT;

  config->Write("ScalarDistance",1.2);
  settings->InitializeSettings();
  scalarTolerance = settings->GetScalarTolerance();
  m_Result = scalarTolerance == 1.2;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  mafEvent e;
  e.SetId(medGUIDicomSettings::ID_SCALAR_TOLERANCE);

  settings->m_ScalarDistanceTolerance = 0.8;
  settings->OnEvent(&e);
  config->Read("ScalarDistance",&scalarTolerance);
  m_Result = scalarTolerance == 0.8;

  TEST_RESULT;

  settings->m_ScalarDistanceTolerance = 1.2;
  settings->OnEvent(&e);
  config->Read("ScalarDistance",&scalarTolerance);
  m_Result = scalarTolerance == 1.2;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
//---------------------------------------------------------
void medGUIDicomSettingsTest::TestEnablePercentageTolerance()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  int enablePercentageTolerance = settings->EnablePercentageTolerance();
  m_Result = enablePercentageTolerance == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("EnablePercentageDistance",TRUE);
  settings->InitializeSettings();
  enablePercentageTolerance = settings->EnablePercentageTolerance();
  m_Result = enablePercentageTolerance == TRUE;

  TEST_RESULT;

  config->Write("EnablePercentageDistance",FALSE);
  settings->InitializeSettings();
  enablePercentageTolerance = settings->EnableScalarTolerance();
  m_Result = enablePercentageTolerance == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  mafEvent e;
  e.SetId(medGUIDicomSettings::ID_PERCENTAGE_DISTANCE_TOLERANCE);

  settings->m_PercentageTolerance = TRUE;
  settings->OnEvent(&e);
  config->Read("EnablePercentageDistance",&enablePercentageTolerance);
  m_Result = enablePercentageTolerance == TRUE;

  TEST_RESULT;

  settings->m_PercentageTolerance = FALSE;
  settings->OnEvent(&e);
  config->Read("EnablePercentageDistance",&enablePercentageTolerance);
  m_Result = enablePercentageTolerance == FALSE;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
//---------------------------------------------------------
void medGUIDicomSettingsTest::TestGetPercentageTolerance()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  double percentageTolerance = settings->GetPercentageTolerance();
  m_Result = percentageTolerance == 88.0;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("PercentageDistance",50.0);
  settings->InitializeSettings();
  percentageTolerance = settings->GetPercentageTolerance();
  m_Result = percentageTolerance == 50.0;

  TEST_RESULT;

  config->Write("PercentageDistance",33.5);
  settings->InitializeSettings();
  percentageTolerance = settings->GetPercentageTolerance();
  m_Result = percentageTolerance == 33.5;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  mafEvent e;
  e.SetId(medGUIDicomSettings::ID_PERCENTAGE_TOLERANCE);

  settings->m_PercentageDistanceTolerance = 50.0;
  settings->OnEvent(&e);
  config->Read("PercentageDistance",&percentageTolerance);
  m_Result = percentageTolerance == 50.0;

  TEST_RESULT;

  settings->m_PercentageDistanceTolerance = 33.5;
  settings->OnEvent(&e);
  config->Read("PercentageDistance",&percentageTolerance);
  m_Result = percentageTolerance == 33.5;

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
//---------------------------------------------------------
void medGUIDicomSettingsTest::TestAutoVMEType()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");
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
  e.SetId(medGUIDicomSettings::ID_AUTO_VME_TYPE);

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
void medGUIDicomSettingsTest::TestGetVMEType()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");
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
  e.SetId(medGUIDicomSettings::ID_SETTING_VME_TYPE);

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
//---------------------------------------------------------
void medGUIDicomSettingsTest::TestEnableToRead()
//---------------------------------------------------------
{
  medGUIDicomSettings *settings = new medGUIDicomSettings(NULL,"test");
  wxConfig *config = new wxConfig(wxEmptyString);
  config->SetPath("Importer Dicom");

  //////////////////////////////////////////////////////////////////////////
  //Test default value
  //////////////////////////////////////////////////////////////////////////
  m_Result = settings->EnableToRead("CT");
  TEST_RESULT;
  m_Result = settings->EnableToRead("CR");
  TEST_RESULT;
  m_Result = settings->EnableToRead("OT");
  TEST_RESULT;
  m_Result = settings->EnableToRead("XA");
  TEST_RESULT;
  m_Result = settings->EnableToRead("MR");
  TEST_RESULT;
  m_Result = settings->EnableToRead("SC");
  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test read value
  //////////////////////////////////////////////////////////////////////////
  config->Write("EnableReadCT",FALSE);
  settings->InitializeSettings();
  m_Result = !settings->EnableToRead("CT");
  TEST_RESULT;

  config->Write("EnableReadCR",FALSE);
  settings->InitializeSettings();
  m_Result = !settings->EnableToRead("CR");
  TEST_RESULT;

  config->Write("EnableReadOT",FALSE);
  settings->InitializeSettings();
  m_Result = !settings->EnableToRead("OT");
  TEST_RESULT;

  config->Write("EnableReadXA",FALSE);
  settings->InitializeSettings();
  m_Result = !settings->EnableToRead("XA");
  TEST_RESULT;

  config->Write("EnableReadMI",FALSE);
  settings->InitializeSettings();
  m_Result = !settings->EnableToRead("MR");
  TEST_RESULT;

  config->Write("EnableReadSC",FALSE);
  settings->InitializeSettings();
  m_Result = !settings->EnableToRead("SC");
  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test write value
  //////////////////////////////////////////////////////////////////////////
  settings->SetEnableToRead("CT",true);
  config->Read("EnableReadCT",&m_Result);
  TEST_RESULT;

  settings->SetEnableToRead("CR",true);
  config->Read("EnableReadCR",&m_Result);
  TEST_RESULT;

  settings->SetEnableToRead("OT",true);
  config->Read("EnableReadOT",&m_Result);
  TEST_RESULT;

  settings->SetEnableToRead("XA",true);
  config->Read("EnableReadXA",&m_Result);
  TEST_RESULT;

  settings->SetEnableToRead("MR",true);
  config->Read("EnableReadMI",&m_Result);
  TEST_RESULT;

  settings->SetEnableToRead("SC",true);
  config->Read("EnableReadSC",&m_Result);
  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////

  cppDEL(config);
  cppDEL(settings);
}
