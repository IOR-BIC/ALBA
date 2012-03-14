/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafXMLStringTest.h,v $
Language:  C++
Date:      $Date: 2010-05-18 07:17:14 $
Version:   $Revision: 1.1.2.1 $
Authors:   Alberto Losi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafXMLStringTest_H__
#define __CPP_UNIT_mafXMLStringTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafXMLString;

class mafXMLStringTest : public CPPUNIT_NS::TestFixture
{
public:

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

	CPPUNIT_TEST_SUITE( mafXMLStringTest );
	CPPUNIT_TEST( TestStaticAllocation );
	CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAllConstructors );
  CPPUNIT_TEST( TestAppend );
  CPPUNIT_TEST( TestErase );
  CPPUNIT_TEST( TestBegin );
  CPPUNIT_TEST( TestEnd );
  CPPUNIT_TEST( TestSize );
  CPPUNIT_TEST( TestGetCStr );
  CPPUNIT_TEST( TestAllOperators );
	CPPUNIT_TEST_SUITE_END();

protected:

  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAllConstructors();
  void TestAppend();
  void TestErase();
  void TestBegin();
  void TestEnd();
  void TestSize();
  void TestGetCStr();
  void TestAllOperators();
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
	runner.addTest( mafXMLStringTest::suite());
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

#endif // __CPP_UNIT_mafXMLStringTest_H__