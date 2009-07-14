/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFProjectSPTest.h,v $
Language:  C++
Date:      $Date: 2009-07-14 10:05:03 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFProjectSPTEST_H__
#define __CPP_UNIT_vtkMAFProjectSPTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/**
  Class Name: vtkMAFProjectSPTest.
  Test class for vtkMAFProjectSP.
*/
class vtkMAFProjectSPTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    /** Start Test Suite */
    CPPUNIT_TEST_SUITE( vtkMAFProjectSPTest );
    /** Test for dynamic allocation */
    CPPUNIT_TEST( TestDynamicAllocation );
    /** Test for projection on X */
    CPPUNIT_TEST( TestExecutionProjectionModeToX );
    /** Test for projection on Y */
    CPPUNIT_TEST( TestExecutionProjectionModeToY );
    /** Test for projection on Z */
    CPPUNIT_TEST( TestExecutionProjectionModeToZ );
    /** Test Print */
    CPPUNIT_TEST( TestPrintSelf );
    /** End Test Suite */
    CPPUNIT_TEST_SUITE_END();

  protected:
 
    /** Allocate and Deallocate filter */
    void TestDynamicAllocation();
    /** Test for projection on X and validate the result */
    void TestExecutionProjectionModeToX();
    /** Test for projection on Y and validate the result */
    void TestExecutionProjectionModeToY();
    /** Test for projection on Z and validate the result */
    void TestExecutionProjectionModeToZ();
    /** Test for Print information*/
    void TestPrintSelf();

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
  runner.addTest( vtkMAFProjectSPTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
