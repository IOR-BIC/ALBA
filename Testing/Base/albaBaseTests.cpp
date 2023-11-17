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

#include "albaBaseTests.h"

#include "albaStringTest.h"
#include "albaTransformTest.h"
#include "albaTransformFrameTest.h"
#include "albaMatrixTest.h"
#include "albaMatrix3x3Test.h"
#include "albaOBBTest.h"
#include "albaDirectoryTest.h"
#include "albaEventSenderTest.h"
#include "albaEventBaseTest.h"
#include "albaEventBroadcasterTest.h"
#include "albaObjectTest.h"
#include "albaIndentTest.h"
#include "albaDirectoryTest.h"
#include "albaTransformBaseTest.h"
#include "albaObserverTest.h"
#include "albaVersionTest.h"
#include "mmuIdFactoryTest.h"
#include "albaVectorTest.h"
#include "albaObjectFactoryTest.h"
#include "vtkALBAToLinearTransformTest.h"
#include "albaReferenceCountedTest.h"
#include "albaLUTLibraryTest.h"
#include "albaDynamicLoaderTest.h"
#include "albaDataChecksumTest.h"
#include "albaVect3dTest.h"
#include "albaEventTest.h"
#include "multiThreaderTest.h"

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include "vtkFileOutputWindow.h"

//--------------------------------------------------------------------------------
albaCxxTypeMacro(albaTestObject);
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
albaCxxTypeMacro(albaFooObject);
//--------------------------------------------------------------------------------

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


	runner.addTest(albaStringTest::suite());
	runner.addTest(albaTransformTest::suite());
	runner.addTest(albaTransformFrameTest::suite());
	runner.addTest(albaMatrixTest::suite());
	runner.addTest(albaMatrix3x3Test::suite());
	runner.addTest(albaOBBTest::suite());
	runner.addTest(albaDirectoryTest::suite());
	runner.addTest(albaEventSenderTest::suite());
	runner.addTest(albaEventBaseTest::suite());
	runner.addTest(albaEventBroadcasterTest::suite());
	runner.addTest(albaObjectTest::suite());
	runner.addTest(albaIndentTest::suite());
	runner.addTest(albaDirectoryTest::suite());
	runner.addTest(albaTransformBaseTest::suite());
	runner.addTest(albaObserverTest::suite());
	runner.addTest(albaVersionTest::suite());
	runner.addTest(mmuIdFactoryTest::suite());
	runner.addTest(albaVectorTest::suite());
	runner.addTest(albaObjectFactoryTest::suite());
	runner.addTest(vtkALBAToLinearTransformTest::suite());
	runner.addTest(albaReferenceCountedTest::suite());
	runner.addTest(albaLUTLibraryTest::suite());
	runner.addTest(albaDynamicLoaderTest::suite());
	runner.addTest(albaDataChecksumTest::suite());
	runner.addTest(albaVect3dTest::suite());
	runner.addTest(albaEventTest::suite());
	runner.addTest(multiThreaderTest::suite());


	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	albaTest::PauseBeforeExit();

	return result.wasSuccessful() ? 0 : 1;
}