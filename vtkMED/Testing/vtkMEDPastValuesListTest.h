/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDPastValuesListTest.h,v $
Language:  C++
Date:      $Date: 2008-04-04 15:17:50 $
Version:   $Revision: 1.1 $
Authors:   Nigel McFarlane

================================================================================
Copyright (c) 2007 University of Bedfordshire, UK (www.beds.ac.uk)
All rights reserved.
===============================================================================*/

#ifndef __CPP_UNIT_vtkMEDPastValuesListtest_H__
#define __CPP_UNIT_vtkMEDPastValuesListtest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFixture.h>


//------------------------------------------------------------------------------
// Test class for vtkMEDPastValuesList
//------------------------------------------------------------------------------
class vtkMEDPastValuesListTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( vtkMEDPastValuesListTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestPastValues );
    CPPUNIT_TEST_SUITE_END();

  protected:
    void Test();
    void TestFixture();
    void TestPastValues();
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
  runner.addTest( vtkMEDPastValuesListTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
