/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmuIdFactoryTest.h,v $
Language:  C++
Date:      $Date: 2007-11-05 13:57:47 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mmuIdFactoryTest_H__
#define __CPP_UNIT_mmuIdFactoryTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mmuIdFactory; Use this suite to trace memory problems */
class mmuIdFactoryTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mmuIdFactoryTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(StaticAllocationTest);
  CPPUNIT_TEST(DynamicAllocationTest);
  CPPUNIT_TEST(GetNextIdTest);
  CPPUNIT_TEST(GetIdTest);
  CPPUNIT_TEST(GetIdNameTest);
  CPPUNIT_TEST_SUITE_END();
  

private:
  void TestFixture();
  void StaticAllocationTest();
  void DynamicAllocationTest();
  void GetNextIdTest();
  void GetIdTest();
  void GetIdNameTest();
};

int
main( int argc, char* argv[] )
{
  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that collects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( mmuIdFactoryTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
