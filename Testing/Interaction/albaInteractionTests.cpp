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

#include "albaActionTest.h"
#include "albaAgentEventQueueTest.h"
#include "albaAgentTest.h"
#include "albaAgentThreadedTest.h"
#include "albaAvatar2DTest.h"
#include "albaAvatar3DConeTest.h"
#include "albaAvatarTest.h"
#include "albaDeviceButtonsPadMouseDialogTest.h"
#include "albaDeviceClientALBATest.h"
#include "albaDeviceManagerTest.h"
#include "albaDeviceSetTest.h"
#include "albaDeviceTest.h"
#include "albaEventInteractionTest.h"
#include "albaFakeLogicForTest.h"
#include "albaGUIGizmoInterfaceTest.h"
#include "albaGUIGizmoRotateTest.h"
#include "albaGUIGizmoScaleTest.h"
#include "albaGUIGizmoTranslateTest.h"
#include "albaGUISaveRestorePoseTest.h"
#include "albaGUITransformInterfaceTest.h"
#include "albaGUITransformMouseTest.h"
#include "albaGUITransformSlidersTest.h"
#include "albaGUITransformTextEntriesTest.h"
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
#include "albaInteractor6DOFCameraMoveTest.h"
#include "albaInteractor6DOFTest.h"
#include "albaInteractorCompositorMouseTest.h"
#include "albaInteractorConstraintTest.h"
#include "albaInteractorExtractIsosurfaceTest.h"
#include "albaInteractorGeneric6DOFTest.h"
#include "albaInteractorPERTest.h"
#include "albaInteractorSERTest.h"
#include "albaInteractorSelectCellTest.h"
#include "albaInteractorTest.h"
#include "albaRefSysTest.h"
#include "albaServiceLocator.h"

#include "mmiVTKPickerTest.h"
#include "mmiSelectPointTest.h"
#include "mmiInfoImageTest.h"
#include "albaInteractor2DDistanceTest.h"
#include "albaInteractor2DAngleTest.h"
#include "albaInteractor2DIndicatorTest.h"
#include "albaInteractorDICOMImporterTest.h"
#include "albaInteractorSliderTest.h"

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

	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;


	runner.addTest(albaGizmoRotateFanTest::suite()); 
	runner.addTest(albaGizmoPathTest::suite());
	runner.addTest(albaGizmoPathRulerTest::suite());
	runner.addTest(albaInteractorSelectCellTest::suite());
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
	runner.addTest(albaGUIGizmoTranslateTest::suite());
	runner.addTest(albaGUIGizmoRotateTest::suite());
	runner.addTest(albaGUIGizmoScaleTest::suite());
	runner.addTest(albaEventInteractionTest::suite());
	runner.addTest(albaDeviceManagerTest::suite());
	runner.addTest(albaInteractionFactoryTest::suite());
	runner.addTest(albaGUITransformTextEntriesTest::suite());
	runner.addTest(albaInteractorCompositorMouseTest::suite());
	runner.addTest(albaActionTest::suite());
	runner.addTest(albaGUISaveRestorePoseTest::suite());
	runner.addTest(albaAgentEventQueueTest::suite());
	runner.addTest(albaAgentThreadedTest::suite());
	runner.addTest(albaInteractorPERTest::suite());
	runner.addTest(albaDeviceTest::suite());
	runner.addTest(albaGizmoScaleAxisTest::suite());
	runner.addTest(albaGizmoScaleTest::suite());
	runner.addTest(albaInteractorExtractIsosurfaceTest::suite());
	runner.addTest(albaInteractorTest::suite());
	runner.addTest(albaInteractor6DOFCameraMoveTest::suite());
	runner.addTest(albaAvatar2DTest::suite());
	runner.addTest(albaAvatarTest::suite());
	runner.addTest(albaInteractorSERTest::suite());
	runner.addTest(albaInteractor6DOFTest::suite());
	runner.addTest(albaInteractorGeneric6DOFTest::suite());
	runner.addTest(albaGUIGizmoInterfaceTest::suite());
	runner.addTest(albaGUITransformInterfaceTest::suite());
	runner.addTest(albaAvatar3DConeTest::suite());
	runner.addTest(albaGUITransformMouseTest::suite());
	runner.addTest(albaGizmoPolylineGraphTest::suite());
	runner.addTest(albaDeviceButtonsPadMouseDialogTest::suite());
	runner.addTest(albaGUITransformSlidersTest::suite());
	runner.addTest(mmiVTKPickerTest::suite());
	runner.addTest(mmiSelectPointTest::suite());
	runner.addTest(mmiInfoImageTest::suite());
	runner.addTest(albaInteractorDICOMImporterTest::suite());
	runner.addTest(albaInteractor2DDistanceTest::suite());
 	runner.addTest(albaInteractor2DAngleTest::suite());
 	runner.addTest(albaInteractor2DIndicatorTest::suite());
	runner.addTest(albaInteractorSliderTest::suite());

	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	cppDEL(logic);

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
