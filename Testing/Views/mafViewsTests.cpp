/*=========================================================================

 Program: MAF2
 Module: mafWizardWaitOpTest
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "mafViewsTests.h"

#include "mafViewArbitrarySliceTest.h"
#include "mafViewOrthoSliceTest.h"
#include "mafViewRXTest.h"
#include "mafViewRXCTTest.h"
#include "mafViewSliceTest.h"
#include "mafViewGlobalSliceTest.h"
#include "mafViewGlobalSliceCompoundTest.h"
#include "mafViewImageTest.h"
#include "mafViewSingleSliceTest.h"
#include "mafViewSingleSliceCompoundTest.h"
#include "mafViewRXCompoundTest.h"
#include "mafView3DTest.h"
#include "mafViewImageCompoundTest.h"
#include "mafViewCTTest.h"
#include "mafViewSlicerTest.h"
#include "mafViewVTKCompoundTest.h"
#include "mafViewSliceOnCurveCompoundTest.h"
#include "mafViewSliceOnCurveTest.h"
#include "mafViewSliceGlobalTest.h"
#include "mafViewCompoundWindowingTest.h"


//Main Test Executor
int	main( int argc, char* argv[] )
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

	runner.addTest(mafViewArbitrarySliceTest::suite());
	runner.addTest(mafViewOrthoSliceTest::suite());
	runner.addTest(mafViewRXTest::suite());
	runner.addTest(mafViewRXCTTest::suite());
	runner.addTest(mafViewSliceTest::suite());
	runner.addTest(mafViewGlobalSliceTest::suite());
	runner.addTest(mafViewGlobalSliceCompoundTest::suite());
	runner.addTest(mafViewImageTest::suite());
	runner.addTest(mafViewSingleSliceTest::suite());
	runner.addTest(mafViewSingleSliceCompoundTest::suite());
	runner.addTest(mafViewRXCompoundTest::suite());
	runner.addTest(mafView3DTest::suite());
	runner.addTest(mafViewImageCompoundTest::suite());
	runner.addTest(mafViewCTTest::suite());
	runner.addTest(mafViewSlicerTest::suite());
	runner.addTest(mafViewVTKCompoundTest::suite());
	runner.addTest(mafViewSliceOnCurveCompoundTest::suite());
	runner.addTest(mafViewSliceOnCurveTest::suite());
	runner.addTest(mafViewSliceGlobalTest::suite());
	runner.addTest(mafViewCompoundWindowingTest::suite());
	
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

