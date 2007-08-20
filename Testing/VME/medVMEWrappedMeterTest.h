/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMEWrappedMeterTest.h,v $
Language:  C++
Date:      $Date: 2007-08-20 13:50:36 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_medVMEWrappedMeterTest_H__
#define __CPP_UNIT_medVMEWrappedMeterTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class medVMEWrappedMeterTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( medVMEWrappedMeterTest );
  CPPUNIT_TEST( TestWithGenericVME );
	CPPUNIT_TEST( TestWithGenericVMEWithMiddlePoints );
	CPPUNIT_TEST( TestWithGenericLandmark );
	CPPUNIT_TEST( TestWithGenericLandmarkWithMiddlePoints );
	
	CPPUNIT_TEST( TestLineDistanceWithGenericVME );
	CPPUNIT_TEST( TestLineAngleWithGenericVME );

	CPPUNIT_TEST( TestLineDistanceWithLandmark );
	CPPUNIT_TEST( TestLineAngleWithLandmark );
	
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestWithGenericVME();
	void TestWithGenericVMEWithMiddlePoints();
	void TestWithGenericLandmark();
	void TestWithGenericLandmarkWithMiddlePoints();

	void TestLineDistanceWithGenericVME();
	void TestLineAngleWithGenericVME();

	void TestLineDistanceWithLandmark();
	void TestLineAngleWithLandmark();
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
	runner.addTest( medVMEWrappedMeterTest::suite());
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

#endif
