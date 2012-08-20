/*=========================================================================

 Program: MAF2Medical
 Module: medAttributeSegmentationVolumeTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_medAttributeSegmentationVolumeTest_H__
#define __CPP_UNIT_medAttributeSegmentationVolumeTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class vtkPolyData;

class medAttributeSegmentationVolumeTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( medAttributeSegmentationVolumeTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestSetAutomaticSegmentationThresholdModality );
  CPPUNIT_TEST( TestSetAutomaticSegmentationGlobalThreshold );
  CPPUNIT_TEST( TestAddRange );
  CPPUNIT_TEST( TestUpdateRange );
  CPPUNIT_TEST( TestDeleteRange );
  CPPUNIT_TEST( TestRemoveAllRanges );
  CPPUNIT_TEST( TestGetNumberOfRanges );
  CPPUNIT_TEST( TestSetRegionGrowingUpperThreshold );
  CPPUNIT_TEST( TestSetRegionGrowingLowerThreshold );
  CPPUNIT_TEST( TestAddSeed );
  CPPUNIT_TEST( TestDeleteSeed );
  CPPUNIT_TEST( TestGetNumberOfSeeds );
  CPPUNIT_TEST( TestRemoveAllSeeds );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestSetAutomaticSegmentationThresholdModality();
  void TestSetAutomaticSegmentationGlobalThreshold();
  void TestAddRange();
  void TestUpdateRange();
  void TestDeleteRange();
  void TestRemoveAllRanges();
  void TestGetNumberOfRanges();
  void TestSetRegionGrowingUpperThreshold();
  void TestSetRegionGrowingLowerThreshold();
  void TestAddSeed();
  void TestDeleteSeed();
  void TestGetNumberOfSeeds();
  void TestRemoveAllSeeds();
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
  runner.addTest( medAttributeSegmentationVolumeTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif