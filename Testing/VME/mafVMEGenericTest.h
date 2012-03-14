/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEGenericTest.h,v $
Language:  C++
Date:      $Date: 2009-06-03 12:53:03 $
Version:   $Revision: 1.1.2.1 $
Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_MAFVMEGENERICTEST_H__
#define __CPP_UNIT_MAFVMEGENERICTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "mafVMERoot.h"
#include "mafVMEGeneric.h"


class mafVMEGenericTest : public CPPUNIT_NS::TestFixture
{

  public:
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( mafVMEGenericTest );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TestSetData );
    CPPUNIT_TEST( TestGetVisualPipe );
      CPPUNIT_TEST_SUITE_END();

protected:
    void TestDynamicAllocation();
    void TestSetData();
    void TestGetVisualPipe();

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
  runner.addTest( mafVMEGenericTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
