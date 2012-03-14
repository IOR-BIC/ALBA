/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafDeviceTest.h,v $
Language:  C++
Date:      $Date: 2011-08-01 12:09:45 $
Version:   $Revision: 1.1.2.2 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafDeviceTest_H__
#define __CPP_UNIT_mafDeviceTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafDeviceTest : public CPPUNIT_NS::TestFixture
{
  public:
  
  /** CPPUNIT fixture: executed before each test */
  void setUp();

  /** CPPUNIT fixture: executed after each test */
  void tearDown();

  CPPUNIT_TEST_SUITE( mafDeviceTest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestConstructorDestructor);

  CPPUNIT_TEST(TestSetGetID);
  CPPUNIT_TEST(TestSetName);
  CPPUNIT_TEST(TestSetGetAutoStart);
  CPPUNIT_TEST(TestAutoStartOnOff);
  
  /** TODO: TestStart exposes 1 line of leaks from multithread stuff. Probably a bug in thread spawning...
  Too dangerous to touch*/
  //Commented because it genereates an error in AQTime with threads
  // This error blocks parabuild leaks dashboard 
  //CPPUNIT_TEST(TestStart);
  
  /** TODO: TestStop exposes 1 line of leaks in multithread stuff. Probably a bug in thread spawning...
  Too dangerous to touch*/
  //Commented because it genereates an errore in AQTime with threads
  // This error blocks parabuild leaks dashboard
  //CPPUNIT_TEST(TestStop);
  
  CPPUNIT_TEST(TestStartUp);
  CPPUNIT_TEST(TestLock);
  CPPUNIT_TEST(TestUnlock);
  CPPUNIT_TEST(TestIsLocked);
  CPPUNIT_TEST(TestSetGetPersistentFlag);  
  CPPUNIT_TEST(TestIsPersistent);
  CPPUNIT_TEST(TestGetGui);
  CPPUNIT_TEST(TestUpdateGui);
  CPPUNIT_TEST(TestOnEvent);

  CPPUNIT_TEST_SUITE_END();

  protected:
    
  void TestFixture();
  void TestConstructorDestructor();
  void TestSetGetID();
  void TestSetName();
  void TestSetGetAutoStart();
  void TestAutoStartOnOff();
  void TestStart();
  void TestStop();
  void TestStartUp();
  void TestLock();
  void TestUnlock();
  void TestIsLocked();
  void TestSetGetPersistentFlag();  
  void TestIsPersistent();
  void TestGetGui();
  void TestUpdateGui();
  void TestOnEvent();
  
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
  runner.addTest( mafDeviceTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
