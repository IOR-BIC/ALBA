/*=========================================================================

 Program: MAF2Medical
 Module: mafOpFreezeVMETest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpFreezeVMETEST_H
#define CPP_UNIT_mafOpFreezeVMETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class mafOpFreezeVMETest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( mafOpFreezeVMETest );
	CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestStaticAllocation );
	CPPUNIT_TEST( TestFreezeVMESurfaceParametric );
	CPPUNIT_TEST( TestFreezeVMEMeter );
	CPPUNIT_TEST( TestFreezeVMEWrappedMeter );
	CPPUNIT_TEST( TestFreezeVMESlicer );
	CPPUNIT_TEST( TestFreezeVMEProber );
  CPPUNIT_TEST( TestFreezeVMEProfileSpline );
  CPPUNIT_TEST( TestFreezeVMERefSys );
	CPPUNIT_TEST_SUITE_END();

protected:

  void TestDynamicAllocation();
  void TestStaticAllocation();
	void TestFreezeVMESurfaceParametric();
	void TestFreezeVMEMeter();
	void TestFreezeVMEWrappedMeter();
	void TestFreezeVMESlicer();
	void TestFreezeVMEProber();
  void TestFreezeVMEProfileSpline();
  void TestFreezeVMERefSys();
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
	runner.addTest( mafOpFreezeVMETest::suite());
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

#endif
