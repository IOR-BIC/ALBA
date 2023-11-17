/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardWaitOpTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "albaViewsTests.h"

#include "albaViewArbitrarySliceTest.h"
#include "albaViewOrthoSliceTest.h"
#include "albaViewRXTest.h"
#include "albaViewRXCTTest.h"
#include "albaViewSliceTest.h"
#include "albaViewGlobalSliceTest.h"
#include "albaViewGlobalSliceCompoundTest.h"
#include "albaViewImageTest.h"
#include "albaViewRXCompoundTest.h"
#include "albaView3DTest.h"
#include "albaViewImageCompoundTest.h"
#include "albaViewCTTest.h"
#include "albaViewSlicerTest.h"
#include "albaViewVTKCompoundTest.h"
#include "albaViewSliceOnCurveCompoundTest.h"
#include "albaViewSliceOnCurveTest.h"
#include "albaViewSliceGlobalTest.h"
#include "albaViewCompoundWindowingTest.h"
#include "albaViewIsosurfaceCompoundTest.h"

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include "vtkFileOutputWindow.h"
#include "vtkALBASmartPointer.h"

//Main Test Executor
int	main( int argc, char* argv[] )
{
	// Create log of VTK error messages
	vtkALBASmartPointer<vtkFileOutputWindow> log;
	vtkOutputWindow::SetInstance(log);
	albaString logPath = wxGetCwd();
	logPath << "\\vtkLog.txt";
	log->SetFileName(logPath);

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

	runner.addTest(albaViewArbitrarySliceTest::suite());
	runner.addTest(albaViewOrthoSliceTest::suite());
	runner.addTest(albaViewRXTest::suite());
	runner.addTest(albaViewRXCTTest::suite());
	runner.addTest(albaViewSliceTest::suite());
	runner.addTest(albaViewGlobalSliceTest::suite());
	runner.addTest(albaViewGlobalSliceCompoundTest::suite());
	runner.addTest(albaViewImageTest::suite());
	runner.addTest(albaViewRXCompoundTest::suite());
	runner.addTest(albaView3DTest::suite());
	runner.addTest(albaViewImageCompoundTest::suite());
	runner.addTest(albaViewCTTest::suite());
	runner.addTest(albaViewSlicerTest::suite());
	runner.addTest(albaViewVTKCompoundTest::suite());
	runner.addTest(albaViewSliceOnCurveCompoundTest::suite());
	runner.addTest(albaViewSliceOnCurveTest::suite());
	runner.addTest(albaViewSliceGlobalTest::suite());
	runner.addTest(albaViewCompoundWindowingTest::suite());
	runner.addTest(albaViewIsosurfaceCompoundTest::suite());
	
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	albaTest::PauseBeforeExit();

	return result.wasSuccessful() ? 0 : 1;
}