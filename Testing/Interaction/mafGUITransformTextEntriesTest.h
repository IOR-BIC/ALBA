/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUITransformTextEntriesTest.h,v $
Language:  C++
Date:      $Date: 2009-10-02 08:56:15 $
Version:   $Revision: 1.1.2.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNITmafGUITransformTextEntriesTest_H__
#define __CPP_UNITmafGUITransformTextEntriesTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "mafEventBase.h"
#include "mafObserver.h"

class vtkSphereSource;
class mafVMESurface;

/** mafGUITransformTextEntriesTest */
class mafGUITransformTextEntriesTest : public CPPUNIT_NS::TestFixture
{
public: 
  /** CPPUNIT fixture: executed before each test */
  void setUp();

  /** CPPUNIT fixture: executed after each test */
  void tearDown();

  CPPUNIT_TEST_SUITE( mafGUITransformTextEntriesTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestSetAbsPose);
  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST_SUITE_END();

  
private:

  void TestFixture();
  void TestConstructorDestructor();
  void TestSetAbsPose();
  void TestOnEvent();
  
  mafVMESurface *m_VMESphere;
  vtkSphereSource *m_VTKSphere;
};

int
main( int argc, char* argv[] )
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
  runner.addTest( mafGUITransformTextEntriesTest::suite());
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

  int GetLastReceivedEventID();;

private:
  int m_LastReceivedEventID;

};


#endif
