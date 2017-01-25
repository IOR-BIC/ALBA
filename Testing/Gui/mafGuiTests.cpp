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


#include "mafGuiTests.h"
#include "mafAttachCameraTest.h"
#include "mafGUISettingsAdvancedTest.h"
#include "mafGUIDicomSettingsTest.h"
#include "mafGUIWizardTest.h"
#include "mafGUIWizardPageTest.h"
#include "mafGUILandmarkTest.h"
#include "mafGUIWizardPageNewTest.h"
#include "mafFakeLogicForTest.h"
#include "mafServiceLocator.h"

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include "vtkFileOutputWindow.h"
#include "vtkMAFSmartPointer.h"

//Main Test Executor
int main( int argc, char* argv[] )
{
	// Create log of VTK error messages
	vtkMAFSmartPointer<vtkFileOutputWindow> log;
	vtkFileOutputWindow::SetInstance(log);
	mafString logPath = wxGetWorkingDirectory();
	logPath << "\\VTKTest.log";
	log->SetFileName(logPath);

	// Create the event manager and test controller
	CPPUNIT_NS::TestResult controller;

	// Add a listener that collects test result
	CPPUNIT_NS::TestResultCollector result;
	controller.addListener( &result );        

	// Add a listener that print dots as test run.
	CPPUNIT_NS::BriefTestProgressListener progress;
	controller.addListener( &progress );      
	
	mafFakeLogicForTest *logic = new mafFakeLogicForTest();
	mafServiceLocator::SetLogicManager(logic);
	
	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;

	runner.addTest(mafGUIWizardTest::suite());
	runner.addTest(mafAttachCameraTest::suite());
	runner.addTest(mafGUISettingsAdvancedTest::suite());
	runner.addTest(mafGUIDicomSettingsTest::suite());
	runner.addTest(mafGUIWizardPageTest::suite());
	runner.addTest(mafGUILandmarkTest::suite());
	runner.addTest(mafGUIWizardPageNewTest::suite());

	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	cppDEL(logic);

	return result.wasSuccessful() ? 0 : 1;
}

