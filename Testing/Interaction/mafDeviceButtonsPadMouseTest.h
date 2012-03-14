/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafDeviceButtonsPadMouseTest.h,v $
Language:  C++
Date:      $Date: 2010-01-13 15:40:23 $
Version:   $Revision: 1.1.2.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafDeviceButtonsPadMouseTest_H__
#define __CPP_UNIT_mafDeviceButtonsPadMouseTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafDeviceButtonsPadMouseTest : public CPPUNIT_NS::TestFixture
{
  public:
  
  /** CPPUNIT fixture: executed before each test */
  void setUp();

  /** CPPUNIT fixture: executed after each test */
  void tearDown();

  CPPUNIT_TEST_SUITE( mafDeviceButtonsPadMouseTest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestSetGetLastPosition);
  CPPUNIT_TEST(TestGetRenderer);
  CPPUNIT_TEST(TestGetView);
  CPPUNIT_TEST(TestGetInteractor);
  CPPUNIT_TEST(TestGetRWI);
  CPPUNIT_TEST(TestSetUpdateRWIDuringMotion);
  CPPUNIT_TEST(TestUpdateRWIDuringMotionOnOff);
  CPPUNIT_TEST(TestIsUpdateRWIDuringMotion);
  CPPUNIT_TEST(TestDisplayToNormalizedDisplay);
  CPPUNIT_TEST(TestNormalizedDisplayToDisplay);
  CPPUNIT_TEST(TestCollaborate);
  CPPUNIT_TEST_SUITE_END();

  protected:
    
  void TestFixture();
  void TestConstructorDestructor();
  void TestSetGetLastPosition();
  void TestGetRenderer();
  void TestGetView();
  void TestGetInteractor();
  void TestGetRWI();
  void TestSetUpdateRWIDuringMotion();
  void TestUpdateRWIDuringMotionOnOff();
  void TestIsUpdateRWIDuringMotion();
  void TestDisplayToNormalizedDisplay();
  void TestNormalizedDisplayToDisplay();
  void TestCollaborate();

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
  runner.addTest( mafDeviceButtonsPadMouseTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
