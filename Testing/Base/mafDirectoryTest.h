/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafDirectoryTest.h,v $
Language:  C++
Date:      $Date: 2007-06-21 09:10:46 $
Version:   $Revision: 1.2 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafDirectoryTest_H__
#define __CPP_UNIT_mafDirectoryTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafDirectory;

class mafDirectoryTest : public CPPUNIT_NS::TestFixture
{
public:

	// CPPUNIT fixture: executed before each test
	void setUp();

	// CPPUNIT fixture: executed after each test
	void tearDown();

	CPPUNIT_TEST_SUITE( mafDirectoryTest );
	CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks

	CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestStaticAllocation );
	CPPUNIT_TEST( TestLoad );
	CPPUNIT_TEST( TestGetFile );
	CPPUNIT_TEST( TestGetNumberOfFiles );
	CPPUNIT_TEST_SUITE_END();

	mafDirectory *m_Directory;

protected:
	void TestFixture();
	void TestStaticAllocation();
	void TestDynamicAllocation();
	void TestLoad();
	void TestGetNumberOfFiles();
	void TestGetFile();
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
	runner.addTest( mafDirectoryTest::suite());
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

#endif
