/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMERefSysTest.h,v $
Language:  C++
Date:      $Date: 2010-01-13 15:41:37 $
Version:   $Revision: 1.1.2.1 $
Authors:   Alberto Losi
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
class mafVMERefSysTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMERefSysTest );

  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks

  CPPUNIT_TEST( TestFixture );

  CPPUNIT_TEST( TestDynamicAllocation );

  CPPUNIT_TEST( TestAllConstructor );

  CPPUNIT_TEST( TestLandmarkAccept );

  CPPUNIT_TEST( TestDeepCopy );

  CPPUNIT_TEST( TestEquals );

  CPPUNIT_TEST( TestSetGetScaleFactor );

  CPPUNIT_TEST( TestGetSurfaceOutput );

  CPPUNIT_TEST( TestSetMatrix );

  CPPUNIT_TEST( TestGetLocalTimeStamps );

  CPPUNIT_TEST( TestIsAnimated );

  CPPUNIT_TEST( TestGetMaterial );

  CPPUNIT_TEST( TestGetIcon );

  CPPUNIT_TEST( TestGetVisualPipe );

  //CPPUNIT_TEST( TestSetRefSysLink );

  CPPUNIT_TEST( TestGetOriginVME );

  CPPUNIT_TEST( TestGetPoint1VME );

  CPPUNIT_TEST( TestGetPoint2VME );

  CPPUNIT_TEST_SUITE_END();

private:

  void TestFixture();

  void TestDynamicAllocation();

  void TestAllConstructor();

	void TestLandmarkAccept();

  void TestDeepCopy();

  void TestEquals();

  void TestSetGetScaleFactor();

  void TestGetSurfaceOutput();

  void TestSetMatrix();

  void TestGetLocalTimeStamps();

  void TestIsAnimated();
  
  void TestGetMaterial();

  void TestGetIcon();

  void TestGetVisualPipe();

  //void TestSetRefSysLink();

	void TestGetOriginVME();

  void TestGetPoint1VME();

  void TestGetPoint2VME();

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
  runner.addTest( mafVMERefSysTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif

