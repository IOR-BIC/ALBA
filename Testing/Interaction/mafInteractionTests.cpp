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

#include "mafInteractionTests.h"

#include "mafActionTest.h"
#include "mafAgentEventQueueTest.h"
#include "mafAgentTest.h"
#include "mafAgentThreadedTest.h"
#include "mafAvatar2DTest.h"
#include "mafAvatar3DConeTest.h"
#include "mafAvatarTest.h"
#include "mafDeviceButtonsPadMouseDialogTest.h"
#include "mafDeviceButtonsPadMouseRemoteTest.h"
#include "mafDeviceButtonsPadMouseTest.h"
#include "mafDeviceButtonsPadTest.h"
#include "mafDeviceClientMAFTest.h"
#include "mafDeviceManagerTest.h"
#include "mafDeviceSetTest.h"
#include "mafDeviceTest.h"
#include "mafEventInteractionTest.h"
#include "mafFakeLogicForTest.h"
#include "mafGUIGizmoInterfaceTest.h"
#include "mafGUIGizmoRotateTest.h"
#include "mafGUIGizmoScaleTest.h"
#include "mafGUIGizmoTranslateTest.h"
#include "mafGUISaveRestorePoseTest.h"
#include "mafGUITransformInterfaceTest.h"
#include "mafGUITransformMouseTest.h"
#include "mafGUITransformSlidersTest.h"
#include "mafGUITransformTextEntriesTest.h"
#include "mafGizmoBoundingBoxTest.h"
#include "mafGizmoHandleTest.h"
#include "mafGizmoInteractionDebuggerTest.h"
#include "mafGizmoInterfaceTest.h"
#include "mafGizmoPathRulerTest.h"
#include "mafGizmoPathTest.h"
#include "mafGizmoPolylineGraphTest.h"
#include "mafGizmoROITest.h"
#include "mafGizmoRotateCircleTest.h"
#include "mafGizmoRotateFanTest.h"
#include "mafGizmoRotateTest.h"
#include "mafGizmoScaleAxisTest.h"
#include "mafGizmoScaleIsotropicTest.h"
#include "mafGizmoScaleTest.h"
#include "mafGizmoSliceTest.h"
#include "mafGizmoTranslateAxisTest.h"
#include "mafGizmoTranslatePlaneTest.h"
#include "mafGizmoTranslateTest.h"
#include "mafInteractionFactoryTest.h"
#include "mafInteractor6DOFCameraMoveTest.h"
#include "mafInteractor6DOFTest.h"
#include "mafInteractorCompositorMouseTest.h"
#include "mafInteractorConstraintTest.h"
#include "mafInteractorExtractIsosurfaceTest.h"
#include "mafInteractorGeneric6DOFTest.h"
#include "mafInteractorPERTest.h"
#include "mafInteractorSERTest.h"
#include "mafInteractorSelectCellTest.h"
#include "mafInteractorTest.h"
#include "mafRefSysTest.h"
#include "mafServiceLocator.h"

#include "mmiVTKPickerTest.h"
#include "mmiSelectPointTest.h"
#include "mmiInfoImageTest.h"
//#include "mafInteractorDICOMImporterTest.h"
#include "medInteractor2DDistanceTest.h"
#include "medInteractor2DAngleTest.h"
#include "medInteractor2DIndicatorTest.h"

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include "vtkFileOutputWindow.h"



void	DummyObserver::OnEvent(mafEventBase *maf_event)
{
	mafLogMessage("\nEntering DummyObserver::OnEvent...\n");
	m_LastReceivedEventID =  maf_event->GetId();
}

int DummyObserver::GetLastReceivedEventID()
{
	return m_LastReceivedEventID;
}

//Main Test Executor
int
	main( int argc, char* argv[] )
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

	runner.addTest(mafGizmoRotateFanTest::suite()); // Fix
	runner.addTest(mafGizmoPathTest::suite());
	runner.addTest(mafGizmoPathRulerTest::suite());
	runner.addTest(mafInteractorSelectCellTest::suite());
	runner.addTest(mafGizmoTranslateAxisTest::suite());
	runner.addTest(mafGizmoTranslatePlaneTest::suite());
	runner.addTest(mafGizmoBoundingBoxTest::suite());
	runner.addTest(mafGizmoScaleIsotropicTest::suite()); // Fix
	runner.addTest(mafGizmoROITest::suite());
	runner.addTest(mafGizmoHandleTest::suite());
	runner.addTest(mafAgentTest::suite());
	runner.addTest(mafGizmoRotateCircleTest::suite());
	runner.addTest(mafGizmoTranslateTest::suite());
	runner.addTest(mafGizmoRotateTest::suite());
	runner.addTest(mafGizmoSliceTest::suite());
	runner.addTest(mafGizmoInteractionDebuggerTest::suite());
	runner.addTest(mafGizmoInterfaceTest::suite());
	runner.addTest(mafInteractorConstraintTest::suite());
	runner.addTest(mafRefSysTest::suite());
	runner.addTest(mafDeviceButtonsPadTest::suite());
	runner.addTest(mafDeviceSetTest::suite());
	runner.addTest(mafGUIGizmoTranslateTest::suite());
	runner.addTest(mafGUIGizmoRotateTest::suite());
	runner.addTest(mafGUIGizmoScaleTest::suite());
	runner.addTest(mafEventInteractionTest::suite());
	runner.addTest(mafDeviceManagerTest::suite());
	runner.addTest(mafInteractionFactoryTest::suite());
	runner.addTest(mafGUITransformTextEntriesTest::suite());
	runner.addTest(mafInteractorCompositorMouseTest::suite());
	runner.addTest(mafActionTest::suite());
	runner.addTest(mafGUISaveRestorePoseTest::suite());
	runner.addTest(mafAgentEventQueueTest::suite());
	runner.addTest(mafAgentThreadedTest::suite());
	runner.addTest(mafDeviceButtonsPadMouseTest::suite());
	runner.addTest(mafInteractorPERTest::suite());
	runner.addTest(mafDeviceTest::suite());
	runner.addTest(mafGizmoScaleAxisTest::suite());
	runner.addTest(mafGizmoScaleTest::suite());
	runner.addTest(mafDeviceClientMAFTest::suite());
	runner.addTest(mafInteractorExtractIsosurfaceTest::suite());
	runner.addTest(mafInteractorTest::suite());
	runner.addTest(mafDeviceButtonsPadMouseRemoteTest::suite());
	runner.addTest(mafInteractor6DOFCameraMoveTest::suite());
	runner.addTest(mafAvatar2DTest::suite());
	runner.addTest(mafAvatarTest::suite());
	runner.addTest(mafInteractorSERTest::suite());
	runner.addTest(mafInteractor6DOFTest::suite());
	runner.addTest(mafInteractorGeneric6DOFTest::suite());
	runner.addTest(mafGUIGizmoInterfaceTest::suite());
	runner.addTest(mafGUITransformInterfaceTest::suite());
	runner.addTest(mafAvatar3DConeTest::suite());
	runner.addTest(mafGUITransformMouseTest::suite());
	runner.addTest(mafGizmoPolylineGraphTest::suite());
	runner.addTest(mafDeviceButtonsPadMouseDialogTest::suite());
	runner.addTest(mafGUITransformSlidersTest::suite());
	runner.addTest(mmiVTKPickerTest::suite());
	runner.addTest(mmiSelectPointTest::suite());
	runner.addTest(mmiInfoImageTest::suite());
	//runner.addTest(mafInteractorDICOMImporterTest::suite());
	runner.addTest(medInteractor2DDistanceTest::suite());
	runner.addTest(medInteractor2DAngleTest::suite());
	runner.addTest(medInteractor2DIndicatorTest::suite());

	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	cppDEL(logic);

	return result.wasSuccessful() ? 0 : 1;
}


void mockListener::OnEvent(mafEventBase *maf_event)
{
	m_Event = *maf_event;
}

mafEventBase * mockListener::GetEvent()
{
	return &m_Event;
}
