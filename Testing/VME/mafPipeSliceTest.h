/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeSliceTest.h,v $
Language:  C++
Date:      $Date: 2009-11-09 15:56:28 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafPipeSliceTest_H__
#define __CPP_UNIT_mafPipeSliceTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafPipeSliceTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafPipeSliceTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestGetOrigin1 ); 
  CPPUNIT_TEST( TestGetOrigin2 );
  CPPUNIT_TEST( TestGetNormal1 );
  CPPUNIT_TEST( TesGetNormal2 );
  CPPUNIT_TEST( TestGetSlice );   
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  void TestGetOrigin1(); 
  void TestGetOrigin2();
  void TestGetNormal1();
  void TesGetNormal2();
  void TestGetSlice();  

};


int
main( int argc, char* argv[] )
{
  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( mafPipeSliceTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
