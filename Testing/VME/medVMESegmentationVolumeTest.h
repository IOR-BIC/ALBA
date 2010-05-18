/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMESegmentationVolumeTest.h,v $
Language:  C++
Date:      $Date: 2010-05-18 15:49:24 $
Version:   $Revision: 1.1.2.4 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_medVMESegmentationVolumeTest_H__
#define __CPP_UNIT_medVMESegmentationVolumeTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafVMEStorage;
class medVMESegmentationVolume;
class mafVMEVolumeGray;

class medVMESegmentationVolumeTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( medVMESegmentationVolumeTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  /*CPPUNIT_TEST( TestSetAutomaticSegmentationThresholdModality );
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
  CPPUNIT_TEST( TestSetRegionGrowingLowerThreshold );*/
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
  mafVMEStorage *m_Storage;
  mafVMEVolumeGray *m_Volume;
  mafVMEVolumeGray *m_VolumeRG;
  mafVMEVolumeGray *m_VolumeManualMask;
  mafVMEVolumeGray *m_VolumeRefinementMask;
};


int
main( int argc, char* argv[] )
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
  runner.addTest( medVMESegmentationVolumeTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
