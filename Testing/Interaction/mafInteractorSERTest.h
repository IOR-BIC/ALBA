/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafInteractorSERTest.h,v $
Language:  C++
Date:      $Date: 2010-05-24 08:17:48 $
Version:   $Revision: 1.1.2.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafInteractorPERTest_H__
#define __CPP_UNIT_mafInteractorPERTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafInteractorSERTest : public CPPUNIT_NS::TestFixture
{
public:
  
  /** CPPUNIT fixture: executed before each test */
  void setUp();

  /** CPPUNIT fixture: executed after each test */
  void tearDown();

  CPPUNIT_TEST_SUITE( mafInteractorSERTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructorDestructor );
  CPPUNIT_TEST( TestBindDeviceToAction );
  CPPUNIT_TEST( TestUnBindDeviceFromAction );
  CPPUNIT_TEST( TestUnBindDeviceFromAllActions);
  CPPUNIT_TEST( TestGetDeviceBindings);
  CPPUNIT_TEST( TestBindAction);
  CPPUNIT_TEST( TestUnBindAction);
  CPPUNIT_TEST( TestAddAction);
  CPPUNIT_TEST( TestGetAction);
  CPPUNIT_TEST( TestGetActions);
  CPPUNIT_TEST_SUITE_END();

protected:
  
  void TestFixture();
  void TestConstructorDestructor();
  void TestBindDeviceToAction();
  void TestUnBindDeviceFromAction();
  void TestUnBindDeviceFromAllActions();
  void TestGetDeviceBindings();
  void TestBindAction();
  void TestUnBindAction();
  void TestAddAction();
  void TestGetAction();
  void TestGetActions();

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
  runner.addTest( mafInteractorSERTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
