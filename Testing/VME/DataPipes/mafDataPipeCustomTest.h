/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafDataPipeCustomTest.h,v $
Language:  C++
Date:      $Date: 2008-07-23 11:59:36 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafDataPipeCustomTest_H__
#define __CPP_UNIT_mafDataPipeCustomTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafDataPipeCustom; Use this suite to trace memory problems */
class mafDataPipeCustomTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafDataPipeCustomTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);

  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST(TestGetVTKData);
  CPPUNIT_TEST(TestGetVTKDataPipe);
  CPPUNIT_TEST(TestUpdate);
  CPPUNIT_TEST(TestUpdateBounds);
  CPPUNIT_TEST(TestSetInput);
  CPPUNIT_TEST(TestSetNthInput);
  
  
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestOnEvent();
  void TestGetVTKData();
  void TestGetVTKDataPipe();
  void TestUpdate();
  void TestUpdateBounds();
  void TestSetInput();
  void TestSetNthInput();
	

  bool result;
};

int main( int argc, char* argv[] )
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
  runner.addTest( mafDataPipeCustomTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif

