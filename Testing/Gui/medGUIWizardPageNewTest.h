/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUIWizardPageNewTest.h,v $
Language:  C++
Date:      $Date: 2011-02-15 09:42:57 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_medGUIWizardPageNewTest_H__
#define __CPP_UNIT_medGUIWizardPageNewTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class TestApp;
class mafGUIFrame;

class medGUIWizardPageNewTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( medGUIWizardPageNewTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAddGuiLowerLeft );
  CPPUNIT_TEST( TestAddGuiLowerRight );
  CPPUNIT_TEST( TestAddGuiLowerCenter );
  
  CPPUNIT_TEST( TestAddGuiLowerUnderLeft );
  CPPUNIT_TEST( TestAddGuiLowerUnderCenter );
  CPPUNIT_TEST( TestRemoveGuiLowerLeft );
  CPPUNIT_TEST( TestRemoveGuiLowerUnderLeft );

  CPPUNIT_TEST( TestSetNextPage );
  CPPUNIT_TEST( TestSetGetZCropBounds );
  CPPUNIT_TEST( TestUpdateWindowing );
  CPPUNIT_TEST( TestUpdateActor );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestDynamicAllocation();
  void TestAddGuiLowerLeft();
  void TestAddGuiLowerRight();
  void TestAddGuiLowerCenter();

  void TestAddGuiLowerUnderLeft();
  void TestAddGuiLowerUnderCenter();

  void TestRemoveGuiLowerLeft();
  void TestRemoveGuiLowerUnderLeft();

  void TestSetNextPage();
  void TestSetGetZCropBounds();
  void TestUpdateWindowing();
  void TestUpdateActor();

  bool m_Result;

  mafGUIFrame *m_Win;

  TestApp *m_App;
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
  runner.addTest( medGUIWizardPageNewTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
