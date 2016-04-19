/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputLandmarkCloudTest
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
#include "mafVMEOutputLandmarkCloudTest.h"

#include "mafVMEOutputLandmarkCloud.h"
#include "mafVMELandmarkCloud.h"

#include "vtkDataSet.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void mafVMEOutputLandmarkCloudTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputLandmarkCloudTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputLandmarkCloud outputLandmarkCloud;
}
//----------------------------------------------------------------------------
void mafVMEOutputLandmarkCloudTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputLandmarkCloud *outputLandmarkCloud = new mafVMEOutputLandmarkCloud();
  cppDEL(outputLandmarkCloud);
}
//----------------------------------------------------------------------------
void mafVMEOutputLandmarkCloudTest::TestGetVTKData()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMELandmarkCloud> lcloud;
  lcloud->TestModeOn();
  lcloud->AppendLandmark(0.0,0.0,0.0,"first");
  lcloud->AppendLandmark(1.0,0.0,0.0,"second");
  lcloud->AppendLandmark(0.0,1.0,0.0,"third");
  lcloud->AppendLandmark(0.0,0.0,1.0,"fourth");

  mafVMEOutputLandmarkCloud *outputLandmarkCloud = NULL;
  outputLandmarkCloud = (mafVMEOutputLandmarkCloud*)lcloud->GetOutput();
  m_Result = outputLandmarkCloud->GetVTKData() == lcloud->GetOutput()->GetVTKData();

  TEST_RESULT;
  
  outputLandmarkCloud->GetVTKData()->Update();

  m_Result = 4 == lcloud->GetOutput()->GetVTKData()->GetNumberOfPoints();

  TEST_RESULT;

  outputLandmarkCloud = (mafVMEOutputLandmarkCloud*)lcloud->GetOutput();
  m_Result = lcloud->GetOutput()->GetVTKData() == NULL;

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMEOutputLandmarkCloudTest::TestGetNumberOfLandmarksSTR()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMELandmarkCloud> lcloud;
  lcloud->TestModeOn();
  lcloud->AppendLandmark(0.0,0.0,0.0,"first");
  lcloud->AppendLandmark(1.0,0.0,0.0,"second");
  lcloud->AppendLandmark(0.0,1.0,0.0,"third");
  lcloud->AppendLandmark(0.0,0.0,1.0,"fourth");

  mafVMEOutputLandmarkCloud *outputLandmarkCloud = NULL;
  outputLandmarkCloud = (mafVMEOutputLandmarkCloud*)lcloud->GetOutput();

  outputLandmarkCloud->Update();

  m_Result = outputLandmarkCloud->GetNumberOfLandmarksSTR() == "4";

  TEST_RESULT;

}
