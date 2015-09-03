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
#include <cppunit/config/SourcePrefix.h>

#include "mafDataPipesTests.h"
#include "vtkMAFDataPipeTest.h"
#include "mafDataPipeInterpolatorTest.h"
#include "mafDataPipeInterpolatorVTKTest.h"
#include "mafDataPipeCustomTest.h"
#include "mafDataPipeInterpolatorScalarMatrixTest.h"
#include "mafDataPipeCustomProberTest.h"

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMESurfaceTestClass)
//-------------------------------------------------------------------------

//Main Test Executor
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

	runner.addTest(vtkMAFDataPipeTest::suite());
	runner.addTest(mafDataPipeInterpolatorTest::suite());
	runner.addTest(mafDataPipeInterpolatorVTKTest::suite());
	runner.addTest(mafDataPipeCustomTest::suite());
	runner.addTest(mafDataPipeInterpolatorScalarMatrixTest::suite());
	runner.addTest(mafDataPipeCustomProberTest::suite());

	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

