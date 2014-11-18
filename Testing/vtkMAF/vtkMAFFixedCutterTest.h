/*=========================================================================

 Program: MAF2
 Module: vtkMAFFixedCutterTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFFixedCutterTest_H__
#define __CPP_UNIT_vtkMAFFixedCutterTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/**
Class Name: vtkMAFFixedCutterTest.
Test class for vtkMAFFixedCutter.
*/
class vtkMAFFixedCutterTest : public CPPUNIT_NS::TestFixture
{
public:

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  /** Start Test Suite */
  CPPUNIT_TEST_SUITE( vtkMAFFixedCutterTest );
  /** Test for dynamic allocation */
  CPPUNIT_TEST( TestDynamicAllocation );
  /** Test of the execution of the filte */
  CPPUNIT_TEST( TestExecute );
  /** End Test Suite */
  CPPUNIT_TEST_SUITE_END();

protected:

  /** Allocate and Deallocate filter */
  void TestDynamicAllocation();
  /** Execution of the filter */
  void TestExecute();

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
  runner.addTest( vtkMAFFixedCutterTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
