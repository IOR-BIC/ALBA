/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputLandmarkCloudTest
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
#include "albaVMEOutputLandmarkCloudTest.h"

#include "albaVMEOutputLandmarkCloud.h"
#include "albaVMELandmarkCloud.h"

#include "vtkDataSet.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void albaVMEOutputLandmarkCloudTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEOutputLandmarkCloudTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputLandmarkCloud outputLandmarkCloud;
}
//----------------------------------------------------------------------------
void albaVMEOutputLandmarkCloudTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputLandmarkCloud *outputLandmarkCloud = new albaVMEOutputLandmarkCloud();
  cppDEL(outputLandmarkCloud);
}
//----------------------------------------------------------------------------
void albaVMEOutputLandmarkCloudTest::TestGetVTKData()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmarkCloud> lcloud;
  lcloud->TestModeOn();
  lcloud->AppendLandmark(0.0,0.0,0.0,"first");
  lcloud->AppendLandmark(1.0,0.0,0.0,"second");
  lcloud->AppendLandmark(0.0,1.0,0.0,"third");
  lcloud->AppendLandmark(0.0,0.0,1.0,"fourth");

  albaVMEOutputLandmarkCloud *outputLandmarkCloud = NULL;
  outputLandmarkCloud = (albaVMEOutputLandmarkCloud*)lcloud->GetOutput();
  m_Result = outputLandmarkCloud->GetVTKData() == lcloud->GetOutput()->GetVTKData();

  TEST_RESULT;

  m_Result = 4 == lcloud->GetOutput()->GetVTKData()->GetNumberOfPoints();

  TEST_RESULT;

  outputLandmarkCloud = (albaVMEOutputLandmarkCloud*)lcloud->GetOutput();
  m_Result = lcloud->GetOutput()->GetVTKData() != NULL;

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMEOutputLandmarkCloudTest::TestGetNumberOfLandmarksSTR()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmarkCloud> lcloud;
  lcloud->TestModeOn();
  lcloud->AppendLandmark(0.0,0.0,0.0,"first");
  lcloud->AppendLandmark(1.0,0.0,0.0,"second");
  lcloud->AppendLandmark(0.0,1.0,0.0,"third");
  lcloud->AppendLandmark(0.0,0.0,1.0,"fourth");

  albaVMEOutputLandmarkCloud *outputLandmarkCloud = NULL;
  outputLandmarkCloud = (albaVMEOutputLandmarkCloud*)lcloud->GetOutput();

  outputLandmarkCloud->Update();

  m_Result = outputLandmarkCloud->GetNumberOfLandmarksSTR() == "4";

  TEST_RESULT;

}
