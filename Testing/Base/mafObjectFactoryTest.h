/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafObjectFactoryTest.h,v $
Language:  C++
Date:      $Date: 2007-12-18 10:46:18 $
Version:   $Revision: 1.1 $
Authors:   Stefano Perticoni,Marco Petrone,Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_MAFOBJECTFACTORYTEST_H__
#define __CPP_UNIT_MAFOBJECTFACTORYTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>



class mafObjectFactoryTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( mafObjectFactoryTest );
	CPPUNIT_TEST( TestStaticAllocation );
	CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestCreateInstance );
	CPPUNIT_TEST( TestGetRegisteredFactories );
	CPPUNIT_TEST( TestRegisterFactory );
	CPPUNIT_TEST( TestUnRegisterAllFactories );
  CPPUNIT_TEST( TestRegisterOverride );
  CPPUNIT_TEST( TestRegisterNewObject );
  CPPUNIT_TEST( TestDisable );
  CPPUNIT_TEST( TestSetEnableFlag );
  CPPUNIT_TEST( TestUnRegisterFactory );
  CPPUNIT_TEST( TestGetArgs );
  CPPUNIT_TEST( TestReHash );
  CPPUNIT_TEST( TestCreateAllInstance );
  CPPUNIT_TEST_SUITE_END();

  protected:
  void TestCreateInstance();
	void TestStaticAllocation();
	void TestDynamicAllocation();
	void TestGetRegisteredFactories();
	void TestRegisterFactory();
	void TestUnRegisterAllFactories();
  void TestRegisterOverride();
  void TestRegisterNewObject();
  void TestDisable();
  void TestSetEnableFlag();
  void TestUnRegisterFactory();
  void TestGetArgs();
  void TestReHash();
  void TestCreateAllInstance();
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
  runner.addTest( mafObjectFactoryTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
