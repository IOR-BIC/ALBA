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

#include "albaInteractionTests.h"

#include "albaAgentEventQueueTest.h"
#include "albaAgentTest.h"
#include "albaDeviceButtonsPadTest.h"
#include "albaDeviceManagerTest.h"
#include "albaDeviceSetTest.h"
#include "albaDeviceTest.h"
#include "albaEventInteractionTest.h"
#include "albaFakeLogicForTest.h"
#include "albaFakeViewManagerForTest.h"
#include "albaGizmoInterfaceGUITest.h"
#include "albaGizmoRotateGUITest.h"
#include "albaGizmoScaleGUITest.h"
#include "albaGizmoTranslateGUITest.h"
#include "albaSaveRestorePoseGUITest.h"
#include "albaTransformInterfaceGUITest.h"
#include "albaRotateTranslateGUITest.h"
#include "albaBehaviorRotateTranslateTest.h"
#include "albaToolFlyToTest.h"
#include "albaToolSelectTest.h"
#include "albaTransformTextEntriesGUITest.h"
#include "albaGizmoBoundingBoxTest.h"
#include "albaGizmoHandleTest.h"
#include "albaGizmoInteractionDebuggerTest.h"
#include "albaGizmoInterfaceTest.h"
#include "albaGizmoPathRulerTest.h"
#include "albaGizmoPathTest.h"
#include "albaGizmoPolylineGraphTest.h"
#include "albaGizmoROITest.h"
#include "albaGizmoRotateCircleTest.h"
#include "albaGizmoRotateFanTest.h"
#include "albaGizmoRotateTest.h"
#include "albaGizmoScaleAxisTest.h"
#include "albaGizmoScaleIsotropicTest.h"
#include "albaGizmoScaleTest.h"
#include "albaGizmoSliceTest.h"
#include "albaGizmoTranslateAxisTest.h"
#include "albaGizmoTranslatePlaneTest.h"
#include "albaGizmoTranslateTest.h"
#include "albaInteractionFactoryTest.h"
#include "albaInteractorConstraintTest.h"
#include "albaToolExtractIsosurfaceTest.h"
#include "albaPositionalEventRouterTest.h"
#include "albaToolSelectCellTest.h"
#include "albaInteractionBehaviorTest.h"
#include "albaRefSysTest.h"
#include "albaServiceLocator.h"

#include "albaToolVTKPickerTest.h"
#include "albaToolSelectPointTest.h"
#include "albaPickHelperTest.h"
#include "albaInteractionRoutingTest.h"
#include "albaToolDICOMImporterTest.h"

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include "vtkFileOutputWindow.h"

//----------------------------------------------------------------------------
void	DummyObserver::OnEvent(albaEventBase *alba_event)
{
	albaLogMessage("\nEntering DummyObserver::OnEvent...\n");
	m_LastReceivedEventID =  alba_event->GetId();
}

//----------------------------------------------------------------------------
int DummyObserver::GetLastReceivedEventID()
{
	return m_LastReceivedEventID;
}

//----------------------------------------------------------------------------
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

	albaFakeLogicForTest *logic = new albaFakeLogicForTest();
	albaServiceLocator::SetLogicManager(logic);
	albaFakeViewManagerForTest *viewManager = new albaFakeViewManagerForTest();
	albaServiceLocator::SetViewManager(viewManager);

	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;


	runner.addTest(albaGizmoRotateFanTest::suite()); 
	runner.addTest(albaGizmoPathTest::suite());
	runner.addTest(albaGizmoPathRulerTest::suite());
	runner.addTest(albaToolSelectCellTest::suite());
	runner.addTest(albaGizmoTranslateAxisTest::suite());
	runner.addTest(albaGizmoTranslatePlaneTest::suite());
	runner.addTest(albaGizmoBoundingBoxTest::suite());
	runner.addTest(albaGizmoScaleIsotropicTest::suite()); 
	runner.addTest(albaGizmoROITest::suite());
	runner.addTest(albaGizmoHandleTest::suite());
	runner.addTest(albaAgentTest::suite());
	runner.addTest(albaGizmoRotateCircleTest::suite());
	runner.addTest(albaGizmoTranslateTest::suite());
	runner.addTest(albaGizmoRotateTest::suite());
	runner.addTest(albaGizmoSliceTest::suite());
	runner.addTest(albaGizmoInteractionDebuggerTest::suite());
	runner.addTest(albaGizmoInterfaceTest::suite());
	runner.addTest(albaInteractorConstraintTest::suite());
	runner.addTest(albaRefSysTest::suite());
	runner.addTest(albaDeviceSetTest::suite());
	runner.addTest(albaGizmoTranslateGUITest::suite());
	runner.addTest(albaGizmoRotateGUITest::suite());
	runner.addTest(albaGizmoScaleGUITest::suite());
	runner.addTest(albaEventInteractionTest::suite());
	runner.addTest(albaDeviceManagerTest::suite());
	runner.addTest(albaInteractionFactoryTest::suite());
	runner.addTest(albaTransformTextEntriesGUITest::suite());
	runner.addTest(albaSaveRestorePoseGUITest::suite());
	runner.addTest(albaAgentEventQueueTest::suite());
	runner.addTest(albaPositionalEventRouterTest::suite());
	runner.addTest(albaDeviceTest::suite());
	runner.addTest(albaDeviceButtonsPadTest::suite());
	runner.addTest(albaGizmoScaleAxisTest::suite());
	runner.addTest(albaGizmoScaleTest::suite());
	runner.addTest(albaToolExtractIsosurfaceTest::suite());
	runner.addTest(albaInteractionBehaviorTest::suite());
	runner.addTest(albaGizmoInterfaceGUITest::suite());
	runner.addTest(albaTransformInterfaceGUITest::suite());
	runner.addTest(albaRotateTranslateGUITest::suite());
	runner.addTest(albaBehaviorRotateTranslateTest::suite());
	runner.addTest(albaToolFlyToTest::suite());
	runner.addTest(albaToolSelectTest::suite());
	runner.addTest(albaGizmoPolylineGraphTest::suite());
	runner.addTest(albaToolVTKPickerTest::suite());
	runner.addTest(albaToolSelectPointTest::suite());
	runner.addTest(albaPickHelperTest::suite());
	runner.addTest(albaInteractionRoutingTest::suite());
	runner.addTest(albaToolDICOMImporterTest::suite());

	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	cppDEL(logic);
	cppDEL(viewManager);

	albaTest::PauseBeforeExit();

	return result.wasSuccessful() ? 0 : 1;
}

//----------------------------------------------------------------------------
void mockListener::OnEvent(albaEventBase *alba_event)
{
	m_Event = *alba_event;
}
//----------------------------------------------------------------------------
albaEventBase * mockListener::GetEvent()
{
	return &m_Event;
}
