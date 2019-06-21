/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpApplyTrajectoryTest
 Authors: Roberto Mucci
 
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
#include "albaOpApplyTrajectoryTest.h"

#include "albaString.h"
#include "albaVME.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEGenericAbstract.h"
#include "albaOpApplyTrajectory.h"
#include "albaTransform.h"
#include "vtkALBASmartPointer.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);


//----------------------------------------------------------------------------
void albaOpApplyTrajectoryTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpApplyTrajectoryTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaOpApplyTrajectory *trajectory = new albaOpApplyTrajectory();
  albaDEL(trajectory);
}
//----------------------------------------------------------------------------
void albaOpApplyTrajectoryTest::TestFromFile()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEVolumeGray> volume;
  std::vector<albaTimeStamp> timeStamps;
  
  albaOpApplyTrajectory *trajectory = new albaOpApplyTrajectory();
  trajectory->TestModeOn();

  albaString Filename=ALBA_DATA_ROOT;
  Filename<<"/Test_ApplyTrajectory/traj.txt";
  trajectory->SetFileName(Filename);
  trajectory->SetInput(volume);
  trajectory->Read();
  
  albaVMEGenericAbstract *vme = albaVMEGenericAbstract::SafeDownCast(volume);
  volume->GetTimeStamps(timeStamps);
  result = (timeStamps.size() == 4);
  TEST_RESULT;

  volume->SetTimeStamp(3);

  double element[16] = {1,0,0,2,0,0.99619469809174556,-0.087155742747658180,0,0,0.087155742747658180,0.99619469809174556,0,0,0,0,1};
  albaMatrix *matrix = volume->GetOutput()->GetMatrix();
  double *matrixElements = *matrix->GetElements();
  result = true;
  for(int i = 0; i < 16; i++) 
  {
    result = result && albaEquals(element[i], matrixElements[i]);
  }
  TEST_RESULT;

  vme = NULL;
  timeStamps.clear();
  albaDEL(trajectory);
}
//----------------------------------------------------------------------------
void albaOpApplyTrajectoryTest::TestFromVME()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEVolumeGray> volume_static;
  albaSmartPointer<albaVMEVolumeGray> volume_variant;
  std::vector<albaTimeStamp> timeStamps;
  
  // First apply trajectories to the variant VME 
  albaOpApplyTrajectory *trajectory_variant = new albaOpApplyTrajectory();
  trajectory_variant->TestModeOn();

  albaString Filename=ALBA_DATA_ROOT;
  Filename<<"/Test_ApplyTrajectory/traj.txt";
  trajectory_variant->SetFileName(Filename);
  trajectory_variant->SetInput(volume_variant);
  trajectory_variant->Read();
  
  // Then apply trajectories to the static VME
  albaOpApplyTrajectory *trajectory_static = new albaOpApplyTrajectory();
  trajectory_static->TestModeOn();
  trajectory_static->SetVME(volume_variant);
  trajectory_static->SetInput(volume_static);
  trajectory_static->ApplyTrajectoriesFromVME();

  // Test results
  albaVMEGenericAbstract *vme = albaVMEGenericAbstract::SafeDownCast(volume_static);
  volume_static->GetTimeStamps(timeStamps);
  result = (timeStamps.size() == 4);
  TEST_RESULT;

  volume_static->SetTimeStamp(3);

  double element[16] = {1,0,0,2,0,0.99619469809174556,-0.087155742747658180,0,0,0.087155742747658180,0.99619469809174556,0,0,0,0,1};
  albaMatrix *matrix = volume_static->GetOutput()->GetMatrix();
  double *matrixElements = *matrix->GetElements();
  result = true;
  for(int i = 0; i < 16; i++) 
  {
    result = result && albaEquals(element[i], matrixElements[i]);
  }
  TEST_RESULT;

  vme = NULL;
  timeStamps.clear();
  albaDEL(trajectory_variant);
  albaDEL(trajectory_static);
}

