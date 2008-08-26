/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFLocalAxisCoordinateTest.h,v $
Language:  C++
Date:      $Date: 2008-08-26 15:05:41 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFLocalAxisCoordinateTEST_H__
#define __CPP_UNIT_vtkMAFLocalAxisCoordinateTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/*Test for  vtkMAFLocalAxisCoordinate*/
class vtkMAFLocalAxisCoordinateTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( vtkMAFLocalAxisCoordinateTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestDynamicAllocation );
    //CPPUNIT_TEST( TestStaticAllocation );
    CPPUNIT_TEST( TestMatrixAccessors );
    CPPUNIT_TEST( TestDatasetAccessors );
    CPPUNIT_TEST( TestGetComputedUserDefinedValue );
    
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestDynamicAllocation();
    void TestStaticAllocation();
    void TestMatrixAccessors();
    void TestDatasetAccessors();
    void TestGetComputedUserDefinedValue();
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
  runner.addTest( vtkMAFLocalAxisCoordinateTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
