/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpImporterASCIITest.h,v $
Language:  C++
Date:      $Date: 2009-10-08 07:26:58 $
Version:   $Revision: 1.1.2.1 $
Authors:   Alberto Losi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafOpImporterASCIITest_H__
#define __CPP_UNIT_mafOpImporterASCIITest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafOpImporterASCIITest : public CPPUNIT_NS::TestFixture
{
public:

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();
  
  CPPUNIT_TEST_SUITE( mafOpImporterASCIITest );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestCopy );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestImportSingleASCIIFile );
  CPPUNIT_TEST( TestImportMultipleASCIIFiles );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestConstructor();
  void TestCopy();
  void TestAccept();
  void TestImportSingleASCIIFile();
  void TestImportMultipleASCIIFiles();
  bool result;
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
	runner.addTest( mafOpImporterASCIITest::suite());
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

#endif
