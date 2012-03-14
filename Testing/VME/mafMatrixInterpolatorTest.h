/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafMatrixInterpolatorTest.h,v $
Language:  C++
Date:      $Date: 2009-05-25 14:46:06 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafMatrixInterpolatorTest_H__
#define __CPP_UNIT_mafMatrixInterpolatorTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** 
Class Name: mafMatrixInterpolatorTest.
Test for mafMatrixInterpolatorTest; 
Use this suite to trace memory problems 
*/

class mafMatrixInterpolatorTest : public CPPUNIT_NS::TestFixture
{
public: 
  /** CPPUNIT fixture: executed before each test */ 
  void setUp();

  /** CPPUNIT fixture: executed after each test */
  void tearDown();

  /** CPPUNIT test suite START*/
  CPPUNIT_TEST_SUITE( mafMatrixInterpolatorTest );
  
  /** Test to control Creation/Destruction of the object */
  CPPUNIT_TEST(TestDynamicAllocation);
  /** Test to control if the datapipe accept correctly vme types */
  CPPUNIT_TEST(TestAccept);
  /** Test to if current item is corrected and if changing timestamp and then update are successful */
  CPPUNIT_TEST(TestGetCurrentItem_SetTimeStamp_Update);
  /** Test if modified time changes after an update*/
  CPPUNIT_TEST(TestGetMTime);

  /** CPPUNIT test suite END */
  CPPUNIT_TEST_SUITE_END();

private:
  /** Creation/Destruction of the object*/
  void TestDynamicAllocation();
  /** Call Accept function of Datapipe */
  void TestAccept();
  /** Retrieve Current item and control if data is correct, use SetTimeStamp and Update the pipe */
  void TestGetCurrentItem_SetTimeStamp_Update();
  /** Check the Modified time of the pipe*/
  void TestGetMTime();
	

  bool m_Result;
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
  runner.addTest( mafMatrixInterpolatorTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif

