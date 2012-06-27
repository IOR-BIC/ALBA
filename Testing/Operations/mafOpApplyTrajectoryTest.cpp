/*=========================================================================

 Program: MAF2
 Module: mafOpApplyTrajectoryTest
 Authors: Roberto Mucci
 
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
#include "mafOpApplyTrajectoryTest.h"

#include "mafString.h"
#include "mafVME.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEGenericAbstract.h"
#include "mafOpApplyTrajectory.h"
#include "mafTransform.h"
#include "vtkMAFSmartPointer.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);


//----------------------------------------------------------------------------
void mafOpApplyTrajectoryTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpApplyTrajectoryTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpApplyTrajectoryTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpApplyTrajectoryTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafOpApplyTrajectory *trajectory = new mafOpApplyTrajectory();
  mafDEL(trajectory);
}
//----------------------------------------------------------------------------
void mafOpApplyTrajectoryTest::TestFromFile()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volume;
  std::vector<mafTimeStamp> timeStamps;
  
  mafOpApplyTrajectory *trajectory = new mafOpApplyTrajectory();
  trajectory->TestModeOn();

  mafString Filename=MAF_DATA_ROOT;
  Filename<<"/Test_ApplyTrajectory/traj.txt";
  trajectory->SetFileName(Filename);
  trajectory->SetInput(volume);
  trajectory->Read();
  
  mafVMEGenericAbstract *vme = mafVMEGenericAbstract::SafeDownCast(volume);
  volume->GetTimeStamps(timeStamps);
  result = (timeStamps.size() == 4);
  TEST_RESULT;

  volume->SetTimeStamp(3);

  double element[16] = {1,0,0,2,0,0.99619469809174556,-0.087155742747658180,0,0,0.087155742747658180,0.99619469809174556,0,0,0,0,1};
  mafMatrix *matrix = volume->GetOutput()->GetMatrix();
  double *matrixElements = *matrix->GetElements();
  result = true;
  for(int i = 0; i < 16; i++) 
  {
    result = result && mafEquals(element[i], matrixElements[i]);
  }
  TEST_RESULT;

  vme = NULL;
  timeStamps.clear();
  mafDEL(trajectory);
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafOpApplyTrajectoryTest::TestFromVME()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volume_static;
  mafSmartPointer<mafVMEVolumeGray> volume_variant;
  std::vector<mafTimeStamp> timeStamps;
  
  // First apply trajectories to the variant VME 
  mafOpApplyTrajectory *trajectory_variant = new mafOpApplyTrajectory();
  trajectory_variant->TestModeOn();

  mafString Filename=MAF_DATA_ROOT;
  Filename<<"/Test_ApplyTrajectory/traj.txt";
  trajectory_variant->SetFileName(Filename);
  trajectory_variant->SetInput(volume_variant);
  trajectory_variant->Read();
  
  // Then apply trajectories to the static VME
  mafOpApplyTrajectory *trajectory_static = new mafOpApplyTrajectory();
  trajectory_static->TestModeOn();
  trajectory_static->SetVME(volume_variant);
  trajectory_static->SetInput(volume_static);
  trajectory_static->ApplyTrajectoriesFromVME();

  // Test results
  mafVMEGenericAbstract *vme = mafVMEGenericAbstract::SafeDownCast(volume_static);
  volume_static->GetTimeStamps(timeStamps);
  result = (timeStamps.size() == 4);
  TEST_RESULT;

  volume_static->SetTimeStamp(3);

  double element[16] = {1,0,0,2,0,0.99619469809174556,-0.087155742747658180,0,0,0.087155742747658180,0.99619469809174556,0,0,0,0,1};
  mafMatrix *matrix = volume_static->GetOutput()->GetMatrix();
  double *matrixElements = *matrix->GetElements();
  result = true;
  for(int i = 0; i < 16; i++) 
  {
    result = result && mafEquals(element[i], matrixElements[i]);
  }
  TEST_RESULT;

  vme = NULL;
  timeStamps.clear();
  mafDEL(trajectory_variant);
  mafDEL(trajectory_static);
  delete wxLog::SetActiveTarget(NULL);
}

