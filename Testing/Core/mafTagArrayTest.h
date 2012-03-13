/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafTagArrayTest.h,v $
Language:  C++
Date:      $Date: 2006-12-05 11:45:26 $
Version:   $Revision: 1.2 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafTagArrayTest_H__
#define __CPP_UNIT_mafTagArrayTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafTagArrayTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafTagArrayTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAllConstructor);
  CPPUNIT_TEST(TestAllCopy);
  CPPUNIT_TEST(TestGetTag);
  CPPUNIT_TEST(TestSetTag);
  CPPUNIT_TEST(TestIsTagPresent);
  CPPUNIT_TEST(TestDeleteTag);
  CPPUNIT_TEST(TestGetNumberOfTags);
  CPPUNIT_TEST(TestGetTagsByType);
  CPPUNIT_TEST(TestGetTagList);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAllConstructor();
  void TestAllCopy();
  void TestGetTag();
  void TestSetTag();
  void TestIsTagPresent();
  void TestDeleteTag();
  void TestGetNumberOfTags();
  void TestGetTagsByType();
  void TestGetTagList();

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
  runner.addTest( mafTagArrayTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
