/*=========================================================================

 Program: MAF2
 Module: vtkMAFExtrudeToCircleTest
 Authors: Nigel McFarlane
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFExtrudeToCircletest_H__
#define __CPP_UNIT_vtkMAFExtrudeToCircletest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFixture.h>
#include "vtkPolyData.h"


//------------------------------------------------------------------------------
// Test class for vtkMAFExtrudeToCircle
//------------------------------------------------------------------------------
class vtkMAFExtrudeToCircleTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( vtkMAFExtrudeToCircleTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestExtrusion );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestExtrusion();

    // Create test polydata
    void CreateTestData() ;

    // render input and output data
    void RenderExtrusion() ;

    vtkPolyData *m_testData ;
    vtkPolyData *m_extrusion ;
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
  runner.addTest( vtkMAFExtrudeToCircleTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
