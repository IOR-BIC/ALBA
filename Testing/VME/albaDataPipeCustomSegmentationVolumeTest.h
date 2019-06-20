/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeCustomSegmentationVolumeTest
 Authors: Alberto Losi, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaDataPipeCustomSegmentationVolumeTest_H__
#define __CPP_UNIT_albaDataPipeCustomSegmentationVolumeTest_H__

class albaVMEVolumeGray;

#include "albaTest.h"

class albaDataPipeCustomSegmentationVolumeTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

	CPPUNIT_TEST_SUITE( albaDataPipeCustomSegmentationVolumeTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( DeepCopyTest );
  CPPUNIT_TEST( SetGetVolumeTest );
  CPPUNIT_TEST( SetGetAutomaticSegmentationThresholdModalityTest );
  CPPUNIT_TEST( AddGetRangeTest );
  CPPUNIT_TEST( UpdateRangeTest );
  CPPUNIT_TEST( DeleteRangeTest );
  CPPUNIT_TEST( RemoveAllRangesTest );
  CPPUNIT_TEST( GetNumberOfRangesTest );
  CPPUNIT_TEST( SetGetAutomaticSegmentationGlobalThresholdTest );
  CPPUNIT_TEST( CheckNumberOfThresholdsTest );
  CPPUNIT_TEST( SetGetManualVolumeMaskTest );
  CPPUNIT_TEST( GetAutomaticOutputTest );
  CPPUNIT_TEST( GetManualOutputTest );
  CPPUNIT_TEST( GetRefinementOutputTest );
  CPPUNIT_TEST( GetRegionGrowingOutputTest );
  CPPUNIT_TEST( SetGetRegionGrowingUpperThresholdTest );
  CPPUNIT_TEST( SetGetRegionGrowingLowerThresholdTest );
  CPPUNIT_TEST( SetGetRegionGrowingSliceRangeTest );
  CPPUNIT_TEST( AddGetSeedTest );
  CPPUNIT_TEST( DeleteSeedTest );
  CPPUNIT_TEST( RemoveAllSeedsTest );
  CPPUNIT_TEST( GetNumberOfSeedsTest );
	CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestDynamicAllocation();
  void DeepCopyTest();
  void SetGetVolumeTest();
  void SetGetAutomaticSegmentationThresholdModalityTest();
  void AddGetRangeTest();
  void UpdateRangeTest();
  void DeleteRangeTest();
  void RemoveAllRangesTest();
  void GetNumberOfRangesTest();
  void SetGetAutomaticSegmentationGlobalThresholdTest();
  void CheckNumberOfThresholdsTest();
  void SetGetManualVolumeMaskTest();

  void GetAutomaticOutputTest();
  void GetManualOutputTest();
  void GetRefinementOutputTest();
  void GetRegionGrowingOutputTest();
  void SetGetRegionGrowingUpperThresholdTest();
  void SetGetRegionGrowingLowerThresholdTest();
  void SetGetRegionGrowingSliceRangeTest();
  void AddGetSeedTest();
  void DeleteSeedTest();
  void RemoveAllSeedsTest();
  void GetNumberOfSeedsTest();

  albaVMEVolumeGray *m_VolumeSP;
  albaVMEVolumeGray *m_VolumeRG;
};

#endif
