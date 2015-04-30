/*=========================================================================

 Program: MAF2
 Module: mafDataPipeCustomSegmentationVolumeTest
 Authors: Alberto Losi, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafDataPipeCustomSegmentationVolumeTest_H__
#define __CPP_UNIT_mafDataPipeCustomSegmentationVolumeTest_H__

class mafVMEVolumeGray;

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafDataPipeCustomSegmentationVolumeTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

	CPPUNIT_TEST_SUITE( mafDataPipeCustomSegmentationVolumeTest );
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

  mafVMEVolumeGray *m_VolumeSP;
  mafVMEVolumeGray *m_VolumeRG;
};

#endif
