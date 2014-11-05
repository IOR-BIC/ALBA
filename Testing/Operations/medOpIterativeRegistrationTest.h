/*=========================================================================

 Program: MAF2Medical
 Module: medOpIterativeRegistrationTest
 Authors: Alberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_medOpIterativeRegistrationTest_H
#define CPP_UNIT_medOpIterativeRegistrationTest_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
using namespace std;
 
class medOpIterativeRegistrationTest : public CPPUNIT_NS::TestFixture
{

public:
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( medOpIterativeRegistrationTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  // CPPUNIT_TEST( TestOnEvent );
  CPPUNIT_TEST( TestCopy);
  CPPUNIT_TEST( TestAccept);
  // CPPUNIT_TEST( TestOpRun );
  CPPUNIT_TEST( TestVmeAccept );
  CPPUNIT_TEST( TestOpDoUndo );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestOnEvent();
  void TestCopy();
  void TestAccept();
  void TestOpRun();
  void TestVmeAccept();
  void TestOpDoUndo();
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
  runner.addTest( medOpIterativeRegistrationTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
