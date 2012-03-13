/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEScalarMatrixTest.h,v $
Language:  C++
Date:      $Date: 2007-12-12 08:28:48 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_MAFVMESCALARTEST_H__
#define __CPP_UNIT_MAFVMESCALARTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <vnl/vnl_matrix.h>

/** Test for mafVMEScalarMatrix.*/
class mafVMEScalarMatrixTest : public CPPUNIT_NS::TestFixture
{
public: 

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafVMEScalarMatrixTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestVMEScalarFactory );
  CPPUNIT_TEST( TestCopyVMEScalar );
  CPPUNIT_TEST( TestVMEScalarData );
  CPPUNIT_TEST( TestVMEScalarMethods );
  CPPUNIT_TEST( TestAnimatedVMEScalar );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestVMEScalarFactory();
  void TestCopyVMEScalar();
  void TestVMEScalarData();
  void TestVMEScalarMethods();
  void TestAnimatedVMEScalar();

  vnl_matrix<double> in_data;
};

int main(int argc, char* argv[])
{
  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that collects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener(&result);        

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener(&progress);      

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest(mafVMEScalarMatrixTest::suite());
  runner.run(controller);

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif


