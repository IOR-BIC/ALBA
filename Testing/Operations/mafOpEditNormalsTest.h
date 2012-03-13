/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpEditNormalsTest.h,v $
Language:  C++
Date:      $Date: 2008-03-06 12:01:16 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi - Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#ifndef CPP_UNIT_mafOpEditNormalsTest_H
#define CPP_UNIT_mafOpEditNormalsTest_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class mafOpEditNormalsTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( mafOpEditNormalsTest );
	CPPUNIT_TEST( Test );
	CPPUNIT_TEST( TestUndo1 );
	CPPUNIT_TEST( TestUndo2 );
	CPPUNIT_TEST_SUITE_END();

protected:
	void Test();
	void TestUndo1();
	void TestUndo2();
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
	runner.addTest( mafOpEditNormalsTest::suite());
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

#endif
