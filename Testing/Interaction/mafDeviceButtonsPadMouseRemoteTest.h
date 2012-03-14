/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafDeviceButtonsPadMouseRemoteTest.h,v $
Language:  C++
Date:      $Date: 2010-04-06 15:05:51 $
Version:   $Revision: 1.1.2.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafDeviceButtonsPadMouseRemoteTest_H__
#define __CPP_UNIT_mafDeviceButtonsPadMouseRemoteTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafDeviceButtonsPadMouseRemoteTest : public CPPUNIT_NS::TestFixture
{
  public:
  
  /** CPPUNIT fixture: executed before each test */
  void setUp();

  /** CPPUNIT fixture: executed after each test */
  void tearDown();

  CPPUNIT_TEST_SUITE( mafDeviceButtonsPadMouseRemoteTest );

    CPPUNIT_TEST(TestFixture);
    CPPUNIT_TEST(TestConstructorDestructor);
    CPPUNIT_TEST(TestOnEventMouse2DMove);
    CPPUNIT_TEST(TestOnEventButtonDown);
    CPPUNIT_TEST(TestOnEventMouseDClick);
    CPPUNIT_TEST(TestOnEventButtonUp);
    CPPUNIT_TEST(TestOnEventViewSelect);
    CPPUNIT_TEST(TestOnEventViewDelete);
    CPPUNIT_TEST(TestOnEventMouseCharEvent);

  CPPUNIT_TEST_SUITE_END();

  protected:
    
  void TestFixture();
  void TestConstructorDestructor();
  void TestOnEventMouse2DMove();
  void TestOnEventButtonDown();
  void TestOnEventMouseDClick();
  void TestOnEventButtonUp();
  void TestOnEventViewSelect();
  void TestOnEventViewDelete();
  void TestOnEventMouseCharEvent();
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
  runner.addTest( mafDeviceButtonsPadMouseRemoteTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
