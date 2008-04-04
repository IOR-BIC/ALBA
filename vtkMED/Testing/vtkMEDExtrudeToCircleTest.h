/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDExtrudeToCircleTest.h,v $
Language:  C++
Date:      $Date: 2008-04-04 15:17:50 $
Version:   $Revision: 1.1 $
Authors:   Nigel McFarlane

================================================================================
Copyright (c) 2007 University of Bedfordshire, UK (www.beds.ac.uk)
All rights reserved.
===============================================================================*/

#ifndef __CPP_UNIT_vtkMEDExtrudeToCircletest_H__
#define __CPP_UNIT_vtkMEDExtrudeToCircletest_H__

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
// Test class for vtkMEDExtrudeToCircle
//------------------------------------------------------------------------------
class vtkMEDExtrudeToCircleTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( vtkMEDExtrudeToCircleTest );
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
  runner.addTest( vtkMEDExtrudeToCircleTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
