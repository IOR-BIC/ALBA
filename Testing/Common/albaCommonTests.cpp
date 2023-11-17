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

#include "albaCommonTests.h"

#include "albaQueryObjectTest.h"
#include "albaResultQueryAbstractHandlerTest.h"
#include "albaRelationalDatabaseAbstractConnectorTest.h"
#include "albaCurvilinearAbscissaOnSkeletonHelperTest.h"
#include "albaHTMLTemplateParserTest.h"
#include "albaHTMLTemplateParserBlockTest.h"
#include "albaWizardBlockInformationTest.h"
#include "albaWizardBlockOperationTest.h"
#include "albaWizardBlockSelectionTest.h"
#include "albaWizardBlockYesNoSelectionTest.h"
#include "albaWizardBlockTest.h"
#include "albaWizardBlockTypeCheckTest.h"
#include "albaWizardBlockVMESelectionTest.h"
#include "albaWizardManagerTest.h"
#include "albaWizardSettingsTest.h"
#include "albaWizardWaitOpTest.h"
#include "albaWizardTest.h"

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include "vtkFileOutputWindow.h"
#include "vtkALBASmartPointer.h"
#include "albaFakeLogicForTest.h"
#include "albaServiceLocator.h"


//Main Test Executor
int main(int argc, char* argv[])
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
	controller.addListener(&result);

	// Add a listener that print dots as test run.
	CPPUNIT_NS::BriefTestProgressListener progress;
	controller.addListener(&progress);

	albaFakeLogicForTest *logic = new albaFakeLogicForTest();
	albaServiceLocator::SetLogicManager(logic);

	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;

	runner.addTest(albaQueryObjectTest::suite());
	runner.addTest(albaResultQueryAbstractHandlerTest::suite());
	runner.addTest(albaRelationalDatabaseAbstractConnectorTest::suite());
	runner.addTest(albaCurvilinearAbscissaOnSkeletonHelperTest::suite());
	runner.addTest(albaHTMLTemplateParserTest::suite());
	runner.addTest(albaHTMLTemplateParserBlockTest::suite());
	runner.addTest(albaWizardBlockInformationTest::suite());
	runner.addTest(albaWizardBlockOperationTest::suite());
	runner.addTest(albaWizardBlockSelectionTest::suite());
	runner.addTest(albaWizardBlockYesNoSelectionTest::suite());
	runner.addTest(albaWizardBlockTest::suite());
	runner.addTest(albaWizardBlockTypeCheckTest::suite());
	runner.addTest(albaWizardBlockVMESelectionTest::suite());
	runner.addTest(albaWizardManagerTest::suite());
	runner.addTest(albaWizardSettingsTest::suite());
	runner.addTest(albaWizardWaitOpTest::suite());
	runner.addTest(albaWizardTest::suite());

	runner.run(controller);

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter(&result, CPPUNIT_NS::stdCOut());
	outputter.write();

	cppDEL(logic);

	albaTest::PauseBeforeExit();

	return result.wasSuccessful() ? 0 : 1;
}