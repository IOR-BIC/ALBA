/*=========================================================================

 Program: MAF2Medical
 Module: medDataPipeCustomSegmentationVolumeTest
 Authors: Alberto Losi, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_medDataPipeCustomSegmentationVolumeTest_H__
#define __CPP_UNIT_medDataPipeCustomSegmentationVolumeTest_H__

class mafVMEVolumeGray;

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class medDataPipeCustomSegmentationVolumeTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

	CPPUNIT_TEST_SUITE( medDataPipeCustomSegmentationVolumeTest );
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


int main( int argc, char* argv[] )
{
	// Create the event manager and test controller
	CPPUNIT_NS::TestResult controller;

	// Add a listener that colllects test result
	CPPUNIT_NS::TestResultCollector result;
	controller.addListener( &result );        

	// Add a listener that print dots as test run.
	CPPUNIT_NS::BriefTestProgressListener progress;
	controller.addListener( &progress );      

	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;
	runner.addTest( medDataPipeCustomSegmentationVolumeTest::suite());
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

#endif
