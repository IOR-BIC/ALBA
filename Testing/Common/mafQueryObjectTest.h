/*=========================================================================

 Program: MAF2
 Module: mafQueryObjectTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafQueryObjectTest_H__
#define __CPP_UNIT_mafQueryObjectTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFixture.h>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;

/**
class name : mafQueryObjectTest
      Test class for mafQueryObject
*/
class mafQueryObjectTest : public CPPUNIT_NS::TestFixture
{
public:

	/** CPPUNIT fixture: executed before each test */
	void setUp();

	/** CPPUNIT fixture: executed after each test */
	void tearDown();

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( mafQueryObjectTest );
  /** macro for test TestDynamicAllocation */
	CPPUNIT_TEST( TestDynamicAllocation );
  /** macro for test TestStaticAllocation */
	CPPUNIT_TEST( TestStaticAllocation );
  /** macro for test TestGetValueAsString */
	CPPUNIT_TEST( TestGetValueAsString );
  /** macro for test TestGetValueAsInt */
	CPPUNIT_TEST( TestGetValueAsInt );
  /** macro for test TestGetValueAsLong */
  CPPUNIT_TEST( TestGetValueAsLong );
  /** macro for test TestGetValueAsFloat */
  CPPUNIT_TEST( TestGetValueAsFloat );
  /** macro for test TestGetValueAsDouble */
  CPPUNIT_TEST( TestGetValueAsDouble );
  /** End test suite macro */
	CPPUNIT_TEST_SUITE_END();

protected:
	/** Dynamic allocation test */
	void TestDynamicAllocation();
  /** Static allocation test */
	void TestStaticAllocation();
  /** Test if value is a string */
	void TestGetValueAsString();
  /** Test if value is a int */
  void TestGetValueAsInt();
  /** Test if value is a long */
  void TestGetValueAsLong();
  /** Test if value is a float */
  void TestGetValueAsFloat();
  /** Test if value is a double */
	void TestGetValueAsDouble();
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
	runner.addTest( mafQueryObjectTest::suite());
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

#endif
