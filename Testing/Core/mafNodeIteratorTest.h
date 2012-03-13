/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafNodeIteratorTest.h,v $
Language:  C++
Date:      $Date: 2008-02-20 12:59:11 $
Version:   $Revision: 1.1 $
Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafNodeIteratorTest_H__
#define __CPP_UNIT_mafNodeIteratorTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafNodeIteratorTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafNodeIteratorTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(GetFirstNodeTest);
  CPPUNIT_TEST(GetLastNodeTest);
  CPPUNIT_TEST(GetNextNodeTest);
  CPPUNIT_TEST(GetPreviousNodeTest);
  CPPUNIT_TEST(IsVisibleTest);
  CPPUNIT_TEST(SetRootNodeTest);
  CPPUNIT_TEST(GetTraversalModeTest);
  CPPUNIT_TEST(SetTraversalModeToPreOrderTest);
  CPPUNIT_TEST(SetTraversalModeToPostOrderTest);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void GetFirstNodeTest();
  void GetLastNodeTest();
  void GetNextNodeTest();
  void GetPreviousNodeTest();
  void IsVisibleTest();
  void SetRootNodeTest();
  void GetTraversalModeTest();
  void IsDoneWithTraversalTest();
  void TestBuildAndDestroyATree();
  void SetTraversalModeToPreOrderTest();
  void SetTraversalModeToPostOrderTest();

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
  runner.addTest( mafNodeIteratorTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
