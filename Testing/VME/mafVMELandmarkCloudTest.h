/*=========================================================================

 Program: MAF2
 Module: mafVMELandmarkCloudTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MAFVMELANDMARKCLOUDTEST_H__
#define __CPP_UNIT_MAFVMELANDMARKCLOUDTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "mafVMERoot.h"
#include "mafVMEGeneric.h" 

/** Test for mafVMELandmarkCloud */
class mafVMELandmarkCloudTest : public CPPUNIT_NS::TestFixture
{

  public:
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( mafVMELandmarkCloudTest );
    CPPUNIT_TEST( DynamicAllocationTest );
    CPPUNIT_TEST( AppendLandmarkTest );
    CPPUNIT_TEST( SetLandmarkTest );
    CPPUNIT_TEST( FindLandmarkIndexTest );
    CPPUNIT_TEST( RemoveLandmarkTest );
    CPPUNIT_TEST( SetNumberOfLandmarksTest );
    CPPUNIT_TEST( GetLandmarkNameTest );
    CPPUNIT_TEST( SetRadiusTest );
    CPPUNIT_TEST( SetSphereResolutionTest );
    CPPUNIT_TEST( DeepCopyTest );
    CPPUNIT_TEST_SUITE_END();

  protected:
    void DynamicAllocationTest();
    void AppendLandmarkTest();
    void SetLandmarkTest();
    void FindLandmarkIndexTest();
    void RemoveLandmarkTest();
    void SetNumberOfLandmarksTest();
    void GetLandmarkNameTest();
    void SetRadiusTest();
    void SetSphereResolutionTest();
    void DeepCopyTest();

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
  runner.addTest( mafVMELandmarkCloudTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
