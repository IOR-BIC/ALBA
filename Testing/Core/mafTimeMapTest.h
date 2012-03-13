/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafTimeMapTest.h,v $
Language:  C++
Date:      $Date: 2006-11-08 15:34:40 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafTimeMapTest_H__
#define __CPP_UNIT_mafTimeMapTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafTimeMapTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafTimeMapTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestAllocation);
  CPPUNIT_TEST(TestNumberOfItems);
  CPPUNIT_TEST(TestOrdering);
  CPPUNIT_TEST(TestFindByIndex);
  CPPUNIT_TEST(TestInsertion);
  CPPUNIT_TEST(TestSortIntegrity);
  CPPUNIT_TEST(TestFindAlgorithm);
  CPPUNIT_TEST(TestSearchOutOfBounds);
  CPPUNIT_TEST(TestItemsRemoval);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestAllocation();
  void TestNumberOfItems();
  void TestOrdering();
  void TestFindByIndex();
  void TestInsertion();
  void TestSortIntegrity();
  void TestFindAlgorithm();
  void TestSearchOutOfBounds();
  void TestItemsRemoval();

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
  runner.addTest( mafTimeMapTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
