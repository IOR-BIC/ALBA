/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMETest.h,v $
Language:  C++
Date:      $Date: 2007-11-29 17:06:21 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafVMETest_H__
#define __CPP_UNIT_mafVMETest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafVMETest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMETest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAllConstructor);
  CPPUNIT_TEST(TestCanReparentTo);

  CPPUNIT_TEST(TestSetTimeStamp);

  CPPUNIT_TEST(TestSetAbsMatrix);
  CPPUNIT_TEST(TestSetAbsPose);
  CPPUNIT_TEST(TestApplyAbsMatrix);

  CPPUNIT_TEST(TestEquals);
  CPPUNIT_TEST(TestDeepCopy);
  CPPUNIT_TEST(TestShallowCopy);
  CPPUNIT_TEST(TestIsAnimated);
  CPPUNIT_TEST(TestIsDataAvailable);

  CPPUNIT_TEST(TestSetBehavior);
  CPPUNIT_TEST(TestSetCrypting);

  CPPUNIT_TEST(TestSetParent);

  CPPUNIT_TEST(TestSetMatrix);
  CPPUNIT_TEST(TestSetPose);
  CPPUNIT_TEST(TestApplyMatrix);
  
  CPPUNIT_TEST(TestSetTreeTime);
  //CPPUNIT_TEST(TestSetVisualMode);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestAllConstructor();
  void TestCanReparentTo();
  void TestEquals();
  void TestDeepCopy();
  void TestShallowCopy();
  void TestIsAnimated();
  void TestIsDataAvailable();

  void TestSetAbsMatrix();
  void TestSetAbsPose();
  void TestApplyAbsMatrix();

  void TestSetBehavior();
  void TestSetCrypting();

  void TestSetParent();

  void TestSetMatrix();
  void TestSetPose();
  void TestApplyMatrix();

  void TestSetTimeStamp();
  void TestSetTreeTime();

  void TestSetVisualMode();

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
  runner.addTest( mafVMETest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif

