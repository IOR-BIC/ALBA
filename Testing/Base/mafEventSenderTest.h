/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafEventSenderTest.h,v $
Language:  C++
Date:      $Date: 2007-06-04 16:46:09 $
Version:   $Revision: 1.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_MAFSTRINGTEST_H__
#define __CPP_UNIT_MAFSTRINGTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "mafObserver.h"

class DummyObserver;
class mafEventBase;
class mafEventSender;

class mafEventSenderTest : public CPPUNIT_NS::TestFixture
{
  public:

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafEventSenderTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestSetListener );
  CPPUNIT_TEST( TestGetListener );
  CPPUNIT_TEST( TestHasListener );
  CPPUNIT_TEST( TestInvokeEvent );
  CPPUNIT_TEST_SUITE_END();

  DummyObserver *m_DummyObserver;
  mafEventSender *m_Sender;

  protected:
  
  void TestFixture();
  void TestSetListener();
  void TestGetListener();
  void TestHasListener();
  void TestInvokeEvent();
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
  runner.addTest( mafEventSenderTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 
  
  return result.wasSuccessful() ? 0 : 1;

}


class DummyObserver : public mafObserver
{
public:

  DummyObserver() {};
  ~DummyObserver() {};

  virtual void OnEvent(mafEventBase *maf_event);

};

#endif
