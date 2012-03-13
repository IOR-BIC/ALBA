/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEOutputTest.h,v $
Language:  C++
Date:      $Date: 2008-03-27 08:15:05 $
Version:   $Revision: 1.1 $
Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafVMEOutputTest_H__
#define __CPP_UNIT_mafVMEOutputTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafVMEOutput; Use this suite to trace memory problems */
class mafVMEOutputTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMEOutputTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestSetVME);
  CPPUNIT_TEST(TestGetVME);
  CPPUNIT_TEST(TestGetPose);
  CPPUNIT_TEST(TestGetMatrix);
  CPPUNIT_TEST(TestGetAbsMatrix);
  CPPUNIT_TEST(TestGetAbsPose);
  CPPUNIT_TEST(TestGetVTKData);
  CPPUNIT_TEST(TestGetTimeBounds);
  CPPUNIT_TEST(TestGetlocalTimeBounds);
  CPPUNIT_TEST(TestGetBounds);
  CPPUNIT_TEST(TestGetVMEBounds);
  CPPUNIT_TEST(TestGetVMELocalBounds);
  CPPUNIT_TEST(TestGetTimeStamp);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestGetVME();
  void TestSetVME();
  void TestGetPose();
  void TestGetMatrix();
  void TestGetAbsMatrix();
  void TestGetAbsPose();
  void TestGetVTKData();
  void TestGetTimeBounds();
  void TestGetlocalTimeBounds();
  void TestGetBounds();
  void TestGetVMEBounds();
  void TestGetVMELocalBounds();
  void TestGetTimeStamp();

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
  runner.addTest( mafVMEOutputTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif

