/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUITransformMouseTest.h,v $
Language:  C++
Date:      $Date: 2011-03-29 16:26:41 $
Version:   $Revision: 1.1.2.2 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafGUITransformMouseTest_H__
#define __CPP_UNIT_mafGUITransformMouseTest_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUIFrame;

class mafGUITransformMouseTest : public CPPUNIT_NS::TestFixture
{
public:

  /** CPPUNIT fixture: executed before each test */
  void setUp();

  /** CPPUNIT fixture: executed after each test */
  void tearDown();

  CPPUNIT_TEST_SUITE( mafGUITransformMouseTest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestEnableWidgets);
  CPPUNIT_TEST(TestAttachInteractorToVme);
  CPPUNIT_TEST(TestDetachInteractorFromVme);
  CPPUNIT_TEST(TestCreateBehavior);
  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestDynamicAllocation();
  void TestEnableWidgets();
  void TestAttachInteractorToVme();
  void TestDetachInteractorFromVme();
  void TestCreateBehavior();
  void TestOnEvent();

  mafGUIFrame *m_Win;

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
  runner.addTest( mafGUITransformMouseTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
