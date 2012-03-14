/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafStorageTest.h,v $
Language:  C++
Date:      $Date: 2010-05-31 13:34:16 $
Version:   $Revision: 1.1.2.1 $
Authors:   Alberto Losi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafStorageTest_H__
#define __CPP_UNIT_mafStorageTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafStorageTest : public CPPUNIT_NS::TestFixture
{
public:

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

	CPPUNIT_TEST_SUITE( mafStorageTest );

  CPPUNIT_TEST( SetGetURLTest );

  CPPUNIT_TEST( ForceGetParserURLTest );

  CPPUNIT_TEST( StoreTest );

  CPPUNIT_TEST( RestoreTest );

  CPPUNIT_TEST( SetGetDocumentTest );

  CPPUNIT_TEST( GetTmpFileTest );

  CPPUNIT_TEST( ReleaseTmpFileTest );

  CPPUNIT_TEST( IsFileInDirectoryTest );

  CPPUNIT_TEST( SetGetTmpFolderTest );

  CPPUNIT_TEST( SetGetErrorCodeTest );

  CPPUNIT_TEST( NeedsUpgradeTest );

	CPPUNIT_TEST_SUITE_END();

protected:

  void SetGetURLTest();

  void ForceGetParserURLTest();

  void StoreTest();

  void RestoreTest();

  void SetGetDocumentTest();

  // void ResolveInputURLTest(); Not implemented in the tested class

  // void StoreToURLTest(); Not implemented in the tested class

  // void ReleaseURL(); Not implemented in the tested class

  void GetTmpFileTest();

  void ReleaseTmpFileTest();

  void IsFileInDirectoryTest();

  void SetGetTmpFolderTest();

  void SetGetErrorCodeTest();

  void NeedsUpgradeTest();
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
	runner.addTest( mafStorageTest::suite());
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

#endif // __CPP_UNIT_mafStorageTest_H__