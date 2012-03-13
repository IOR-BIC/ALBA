/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafDataPipeTest.h,v $
Language:  C++
Date:      $Date: 2008-04-01 13:12:17 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafDataPipeTest_H__
#define __CPP_UNIT_mafDataPipeTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafDataPipe; Use this suite to trace memory problems */
class mafDataPipeTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafDataPipeTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestUpdateBounds);
  CPPUNIT_TEST(TestUpdate);
  CPPUNIT_TEST(TestGetBounds);
  CPPUNIT_TEST(TestGetVTKData);
  CPPUNIT_TEST(TestSetGetTimeStamp);
  CPPUNIT_TEST(TestGetMTime);
  CPPUNIT_TEST(TestAccept);
  CPPUNIT_TEST(TestMakeACopy);
	CPPUNIT_TEST(TestDeepCopy);
	CPPUNIT_TEST(TestSetGetVME);
	CPPUNIT_TEST(TestSetGetDependOnPose);
	CPPUNIT_TEST(TestSetGetDependOnVMETime);
	CPPUNIT_TEST(TestSetGetDependOnAbsPose);
	CPPUNIT_TEST(TestPrint);
	CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
	void TestUpdateBounds();
	void TestUpdate();
	void TestGetBounds();
	void TestGetVTKData();
	void TestSetGetTimeStamp();
	void TestGetMTime();
	void TestAccept();
	void TestMakeACopy();
	void TestDeepCopy();
	void TestSetGetVME();
	void TestSetGetDependOnPose();
	void TestSetGetDependOnVMETime();
	void TestSetGetDependOnAbsPose();
	void TestPrint();
	void TestOnEvent();

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
  runner.addTest( mafDataPipeTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif

