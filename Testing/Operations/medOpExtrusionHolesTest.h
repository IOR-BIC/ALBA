/*=========================================================================

 Program: MAF2Medical
 Module: medOpExtrusionHolesTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_medOpExtrusionHolesTEST_H
#define CPP_UNIT_medOpExtrusionHolesTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class medOpExtrusionHolesTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( medOpExtrusionHolesTest );
	CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestStaticAllocation );
	CPPUNIT_TEST( TestSetGetExtrusionFactor );
	CPPUNIT_TEST( TestExtractFreeEdge );
	CPPUNIT_TEST( TestExtrude );
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestDynamicAllocation();
	void TestStaticAllocation();
	void TestSetGetExtrusionFactor();
	void TestExtractFreeEdge();
	void TestExtrude();
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
	runner.addTest( medOpExtrusionHolesTest::suite());
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

#endif
