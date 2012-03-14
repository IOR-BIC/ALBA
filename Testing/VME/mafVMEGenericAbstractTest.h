/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEGenericAbstractTest.h,v $
Language:  C++
Date:      $Date: 2009-09-24 11:32:48 $
Version:   $Revision: 1.1.2.1 $
Authors:   Alberto Losi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafVMEGenericAbstractTest_H__
#define __CPP_UNIT_mafVMEGenericAbstractTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafVMEGenericAbstract; Use this suite to trace memory problems */
class mafVMEGenericAbstractTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMEGenericAbstractTest );

  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestDynamicAllocation);

  CPPUNIT_TEST(TestAllConstructor);

  CPPUNIT_TEST(TestDeepCopy);
  CPPUNIT_TEST(TestShallowCopy);
  CPPUNIT_TEST(TestEquals);

  CPPUNIT_TEST(TestReparentTo);

  CPPUNIT_TEST(TestGetMatrixVector);
  CPPUNIT_TEST(TestSetMatrix);
  CPPUNIT_TEST(TestGetDataVector);

  CPPUNIT_TEST(TestGetDataTimeStamps);
  CPPUNIT_TEST(TestGetMatrixTimeStamps);
  CPPUNIT_TEST(TestGetLocalTimeStamps);
  CPPUNIT_TEST(TestGetLocalTimeBounds);

  CPPUNIT_TEST(TestIsAnimated);

  CPPUNIT_TEST(TestIsDataAvailable);

  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();

  void TestAllConstructor();

  void TestDeepCopy();

  void TestShallowCopy();
  void TestEquals();

  void TestReparentTo();

  void TestGetMatrixVector();
  void TestSetMatrix();
  void TestGetDataVector();

  void TestGetDataTimeStamps();
  void TestGetMatrixTimeStamps();
  void TestGetLocalTimeStamps();
  void TestGetLocalTimeBounds();

  void TestIsAnimated();

  void TestIsDataAvailable();

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
  runner.addTest( mafVMEGenericAbstractTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif