/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafMatrixPipeTest.h,v $
Language:  C++
Date:      $Date: 2008-03-13 16:58:16 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2008
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafMatrixPipeTest_H__
#define __CPP_UNIT_mafMatrixPipeTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

//forward reference
class mafMatrixPipe;
class mafVMESurface;

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafMatrixPipeTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafMatrixPipeTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestDynamicAllocation ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestUpdateMatrixObserverSet_Get_On_Off );
  CPPUNIT_TEST( TestSetGetVME );
  CPPUNIT_TEST( TestSetGetTimeStamp );
  CPPUNIT_TEST( TestGetMTime );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestGetMatrix );
  CPPUNIT_TEST( TestMakeACopy );
  CPPUNIT_TEST( TestDeepCopy );
  CPPUNIT_TEST( TestUpdate );

  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestUpdateMatrixObserverSet_Get_On_Off();
  void TestSetGetVME();
  void TestSetGetTimeStamp();
  void TestGetMTime();
  void TestAccept();
  void TestGetMatrix();
  void TestMakeACopy();
  void TestDeepCopy();
  void TestUpdate();

  mafVMESurface *m_SurfaceTest;
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
  runner.addTest( mafMatrixPipeTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif