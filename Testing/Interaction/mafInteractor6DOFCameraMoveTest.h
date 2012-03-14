/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafInteractor6DOFCameraMoveTest.h,v $
Language:  C++
Date:      $Date: 2010-04-09 07:01:52 $
Version:   $Revision: 1.1.2.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafInteractor6DOFCameraMoveTest_H__
#define __CPP_UNIT_mafInteractor6DOFCameraMoveTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafInteractor6DOFCameraMoveTest : public CPPUNIT_NS::TestFixture
{
  public:
  
  /** CPPUNIT fixture: executed before each test */
  void setUp();

  /** CPPUNIT fixture: executed after each test */
  void tearDown();

  CPPUNIT_TEST_SUITE( mafInteractor6DOFCameraMoveTest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestStartInteraction);
  CPPUNIT_TEST(TestOnEventNotHandled);
  CPPUNIT_TEST(TestOnEventOnMchInputChannelWithNULLTracker);
  CPPUNIT_TEST(TestOnEventOnMchInputChannelWithDummyTracker);
  CPPUNIT_TEST(TestOnEventTracker3DMoveOnMchInputChannelWithDummyTracker);
  CPPUNIT_TEST(TestOnEventTracker3DMoveOnMchInputChannelWithDummyTrackerAndNotNULLCamera);
  CPPUNIT_TEST_SUITE_END();

  protected:
    
  void TestFixture();
  void TestConstructorDestructor();
  void TestStartInteraction();
  void TestOnEventNotHandled();
  void TestOnEventOnMchInputChannelWithNULLTracker();
  void TestOnEventOnMchInputChannelWithDummyTracker();
  void TestOnEventTracker3DMoveOnMchInputChannelWithDummyTracker();
  void TestOnEventTracker3DMoveOnMchInputChannelWithDummyTrackerAndNotNULLCamera();
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
  runner.addTest( mafInteractor6DOFCameraMoveTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
