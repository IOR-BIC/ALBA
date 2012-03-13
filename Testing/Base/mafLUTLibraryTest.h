/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafLUTLibraryTest.h,v $
Language:  C++
Date:      $Date: 2008-06-03 16:48:45 $
Version:   $Revision: 1.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

using namespace std;

#ifndef __CPP_UNIT_mafLUTLibraryTest_H__
#define __CPP_UNIT_mafLUTLibraryTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "mafLUTLibrary.h"
#include "mafString.h"
#include "vtkMAFSmartPointer.h"
#include "vtkLookupTable.h"

class mafLUTLibraryTest : public CPPUNIT_NS::TestFixture
{
  public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

	CPPUNIT_TEST_SUITE( mafLUTLibraryTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
	CPPUNIT_TEST( TestSetGetDir );
  CPPUNIT_TEST(TestAdd);
  CPPUNIT_TEST(TestRemove);
  CPPUNIT_TEST(TestLoadSave);
  CPPUNIT_TEST(TestGetNumberOfLuts);
  CPPUNIT_TEST(TestGetLutNames);
  CPPUNIT_TEST(TestGetLutByName);
  CPPUNIT_TEST(TestHasLut);
	CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
	void TestSetGetDir();
  void TestAdd();
  void TestRemove();
  void TestLoadSave();
  void TestGetNumberOfLuts();
  void TestGetLutNames();
  void TestGetLutByName();
  void TestHasLut();
 
  vtkLookupTable *m_LutDefault;
  vtkLookupTable *m_LutEField;
  vtkLookupTable *m_LutGlow;
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
	runner.addTest( mafLUTLibraryTest::suite());
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
};

#endif
