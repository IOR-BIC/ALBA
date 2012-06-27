/*=========================================================================

 Program: MAF2
 Module: mafVectorTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVectorTest_H__
#define __CPP_UNIT_mafVectorTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafVector; Use this suite to trace memory problems */
class mafVectorTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVectorTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestGetNumberOfItems);
  CPPUNIT_TEST(TestRemoveItem);
  CPPUNIT_TEST(TestAppendItem);
  CPPUNIT_TEST(TestReplaceItem);
  CPPUNIT_TEST(TestIsItemPresent);
  CPPUNIT_TEST(TestGetSetItem);
  CPPUNIT_TEST(TestFindItem);
  CPPUNIT_TEST(TestPush);
  CPPUNIT_TEST(TestFindAndRemoveItem);
  CPPUNIT_TEST(TestRemoveAllItems);
  CPPUNIT_TEST(TestPop);
  CPPUNIT_TEST(TestVectorAPICombination);
  CPPUNIT_TEST_SUITE_END();


private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestGetNumberOfItems();
  void TestRemoveItem();
  void TestAppendItem();
  void TestReplaceItem();
  void TestIsItemPresent();
  void TestGetSetItem();
  void TestFindItem();
  void TestPush();
  void TestFindAndRemoveItem();
  void TestRemoveAllItems();
  void TestPop();
  void TestVectorAPICombination();

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
  runner.addTest( mafVectorTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
