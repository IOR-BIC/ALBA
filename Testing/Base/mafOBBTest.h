/*=========================================================================

 Program: MAF2
 Module: mafOBBTest
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOBBTest_H__
#define __CPP_UNIT_mafOBBTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafOBBTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOBBTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAllConstructors);
  CPPUNIT_TEST(TestEquals);
  CPPUNIT_TEST(TestCopy);
  CPPUNIT_TEST(TestIsValid);
  CPPUNIT_TEST(TestCopyTo);
  CPPUNIT_TEST(TestApplyTransform);
  CPPUNIT_TEST(TestMergeBounds);
  CPPUNIT_TEST(TestIsInside);
  CPPUNIT_TEST(TsetGetDimensions);
  CPPUNIT_TEST(TestGetCenter);
  CPPUNIT_TEST(TestSetCenter);
  CPPUNIT_TEST(TestSetDimensions);
  CPPUNIT_TEST(TestGetWidth);
  CPPUNIT_TEST(TestGetHeight);
  CPPUNIT_TEST(TsetGetDepth);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAllConstructors();
  void TestEquals();
  void TestCopy();
  void TestIsValid();
  void TestCopyTo();
  void TestApplyTransform();
  void TestMergeBounds();
  void TestIsInside();
  void TsetGetDimensions();
  void TestGetCenter();
  void TestSetCenter();
  void TestSetDimensions();
  void TestGetWidth();
  void TestGetHeight();
  void TsetGetDepth();

  bool result;
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
  runner.addTest( mafOBBTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
