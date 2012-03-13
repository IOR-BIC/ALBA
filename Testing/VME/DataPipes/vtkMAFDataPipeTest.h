/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFDataPipeTest.h,v $
Language:  C++
Date:      $Date: 2008-05-06 10:18:45 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFDataPipeTest_H__
#define __CPP_UNIT_vtkMAFDataPipeTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for vtkMAFDataPipe; Use this suite to trace memory problems */
class vtkMAFDataPipeTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( vtkMAFDataPipeTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestSetNthInput);
  CPPUNIT_TEST(TestGetOutput);
  CPPUNIT_TEST(TestGetMTime);
  CPPUNIT_TEST(TestGetInformationTime);
  CPPUNIT_TEST(TestSetGetDataPipe);
  CPPUNIT_TEST(TestUpdateInformation);
  
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
	void TestSetNthInput();
	void TestGetOutput();
	void TestGetMTime();
	void TestGetInformationTime();
	void TestSetGetDataPipe();
	void TestUpdateInformation();
	

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
  runner.addTest( vtkMAFDataPipeTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif

