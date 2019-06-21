/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMESegmentationVolumeTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMESegmentationVolumeTest_H__
#define __CPP_UNIT_albaVMESegmentationVolumeTest_H__

#include "albaTest.h"

class albaVMEStorage;
class albaVMESegmentationVolume;
class albaVMEVolumeGray;

class albaVMESegmentationVolumeTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaVMESegmentationVolumeTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestSetAutomaticSegmentationThresholdModality );
  CPPUNIT_TEST( TestUpdateRange );
  CPPUNIT_TEST( TestAddRange );
  CPPUNIT_TEST( TestDeleteRange );
  CPPUNIT_TEST( TestSetAutomaticSegmentationGlobalThreshold );
  CPPUNIT_TEST( TestSetVolumeLink );
  CPPUNIT_TEST( TestAutomaticSegmentation );
  CPPUNIT_TEST( TestRefinementSegmentation );
  CPPUNIT_TEST( TestManualSegmentation );
  CPPUNIT_TEST( TestSetManualVolumeMask );
  CPPUNIT_TEST( TestSetRefinementVolumeMask );
  CPPUNIT_TEST( TestAddSeed );
  CPPUNIT_TEST( TestRemoveAllSeeds );
  CPPUNIT_TEST( TestSetRegionGrowingUpperThreshold );
  CPPUNIT_TEST( TestSetRegionGrowingLowerThreshold );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestSetAutomaticSegmentationThresholdModality();
  void TestAddRange();
  void TestUpdateRange();
  void TestDeleteRange();
  void TestSetAutomaticSegmentationGlobalThreshold();
  void TestSetVolumeLink();
  void TestAutomaticSegmentation();
  void TestRefinementSegmentation();
  void TestManualSegmentation();
  void TestSetManualVolumeMask();
  void TestSetRefinementVolumeMask();
  void TestAddSeed();
  void TestRemoveAllSeeds();
  void TestSetRegionGrowingUpperThreshold();
  void TestSetRegionGrowingLowerThreshold();

  bool m_Result;
  albaVMEStorage *m_Storage;
  albaVMEVolumeGray *m_Volume;
  albaVMEVolumeGray *m_VolumeRG;
  albaVMEVolumeGray *m_VolumeManualMask;
  albaVMEVolumeGray *m_VolumeRefinementMask;
};

#endif
