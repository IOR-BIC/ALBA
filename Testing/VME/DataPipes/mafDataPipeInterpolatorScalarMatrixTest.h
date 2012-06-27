/*=========================================================================

 Program: MAF2
 Module: mafDataPipeInterpolatorScalarMatrixTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafDataPipeInterpolatorScalarMatrixTest_H__
#define __CPP_UNIT_mafDataPipeInterpolatorScalarMatrixTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <vnl/vnl_matrix.h>

/** 
   Class Name: mafDataPipeInterpolatorScalarMatrixTest.
   Test for mafScalarMatrixInterpolator; 
   Use this suite to trace memory problems 
 */
class mafDataPipeInterpolatorScalarMatrixTest : public CPPUNIT_NS::TestFixture
{
public: 
  /** CPPUNIT fixture: executed before each test */ 
  void setUp();

  /* CPPUNIT fixture: executed after each test */
  void tearDown();

  /* CPPUNIT test suite START*/
  CPPUNIT_TEST_SUITE( mafDataPipeInterpolatorScalarMatrixTest );
  /** Test to control Creation/Destruction of the object */
  CPPUNIT_TEST(TestDynamicAllocation);
  /** Test to control if the datapipe accept correctly vme types */
  CPPUNIT_TEST(TestAccept);
  /** Test to control if the events chain is correct */
  CPPUNIT_TEST(TestOnEvent);
  /** Test to control GetScalarData Retrieve the correct vnl matrix data */
  CPPUNIT_TEST(TestGetScalarData);
  /** Test to control if CurrentItem is of the correct type and contain correct data */
  CPPUNIT_TEST(TestGetCurrentItem);
  
  /* CPPUNIT test suite END*/
  CPPUNIT_TEST_SUITE_END();

private:
  /** Creation/Destruction of the object*/
  void TestDynamicAllocation();
  /** Call Accept function of Datapipe */
  void TestAccept();
  /** Call OnEvent and send event in order to test the events chain */
  void TestOnEvent();
  /** Retrieve directly vnl matrix and control if it is correct*/
  void TestGetScalarData();
  /** Retrieve Current item and control if data is correct*/
  void TestGetCurrentItem();
  
  bool m_Result;
  vnl_matrix<double> m_DataMatrix;
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
  runner.addTest( mafDataPipeInterpolatorScalarMatrixTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif

