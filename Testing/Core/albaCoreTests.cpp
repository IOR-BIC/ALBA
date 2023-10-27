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

#include "albaCoreTests.h"

#include "albaAbsMatrixPipeTest.h"
#include "albaAttributeTest.h"
#include "albaAttributeTraceabilityTest.h"
#include "albaAxesTest.h"
#include "albaCoreFactoryTest.h"
#include "albaDataPipeTest.h"
#include "albaEventIOTest.h"
#include "albaFakeLogicForTest.h"
#include "albaGUITreeContextualMenuTest.h"
#include "albaLODActorTest.h"
#include "albaMatrixPipeTest.h"
#include "albaOpContextStackTest.h"
#include "albaOpCopyTest.h"
#include "albaOpCutTest.h"
#include "albaOpEditTest.h"
#include "albaOpManagerTest.h"
#include "albaOpPasteTest.h"
#include "albaOpSelectTest.h"
#include "albaOpStackTest.h"
#include "albaOpTest.h"
#include "albaPipeBoxTest.h"
#include "albaPipeFactoryTest.h"
#include "albaPipeTest.h"
#include "albaProgressBarHelperTest.h"
#include "albaSceneGraphTest.h"
#include "albaSceneNodeTest.h"
#include "albaServiceLocator.h"
#include "albaTagArrayTest.h"
#include "albaTagItemTest.h"
#include "albaTimeMapScalarTest.h"
#include "albaTimeMapTest.h"
#include "albaUserTest.h"
#include "albaVMEFactoryTest.h"
#include "albaVMEIteratorTest.h"
#include "albaVMEManagerTest.h"
#include "albaVMEManagerTest.h"
#include "albaVMEOutputNULLTest.h"
#include "albaVMEOutputTest.h"
#include "albaVMERootTest.h"
#include "albaVMEStorageTest.h"
#include "albaVMEStorageTest2.h"
#include "albaVMETest.h"
#include "albaVMETest.h"
#include "albaViewHTMLTest.h"
#include "albaViewManagerTest.h"
#include "albaViewPlotTest.h"
#include "albaViewVTKTest.h"
#include "mmaApplicationLayoutTest.h"
#include "mmaMaterialTest.h"
#include "mmaVolumeMaterialTest.h"
#include "albaVMEOutputNULL.h"
#include "TimeSetTest.h"
#include "VMEPoseTest.h"

#include <wx/dir.h>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "vtkFileOutputWindow.h"
#include "vtkALBAAssemblyTest.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEHelper);

//----------------------------------------------------------------------------
albaVMEHelper::albaVMEHelper()
{
	albaNEW(m_Transform);
	albaVMEOutputNULL *output = albaVMEOutputNULL::New();
	output->SetTransform(m_Transform);
	SetOutput(output);
}

//----------------------------------------------------------------------------
void DummyObserver::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		albaEvent *eventToCopy = new albaEvent(e->GetSender(),e->GetId(),e->GetVme());
		m_ListEvent.push_back(eventToCopy);
	}
}

//----------------------------------------------------------------------------
DummyObserver::~DummyObserver()
{
	for(int i=0;i<m_ListEvent.size();i++)
	{
		delete m_ListEvent[i];
	}
	m_ListEvent.clear();
}

//----------------------------------------------------------------------------
albaOpDummyHelper::albaOpDummyHelper(wxString label,  bool canundo, int opType, bool inputPreserving)
{
	m_Canundo = canundo;
	m_OpType = opType;
	m_InputPreserving = inputPreserving;
}

//----------------------------------------------------------------------------
bool RemoveDir(const char *dirName)
{
	wxArrayString filenameArray;
	wxDir::GetAllFiles(dirName,&filenameArray);
	for (int index = 0; index < filenameArray.size(); index++)
	{
		wxRemoveFile(filenameArray.Item(index));
	}

	bool result = TRUE;

	result = wxRmdir(dirName);
	if (result == false)
	{
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------
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

	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;

	albaFakeLogicForTest *logic = new albaFakeLogicForTest();
	albaServiceLocator::SetLogicManager(logic);

	//this test must be run first because manages factories
	runner.addTest(albaCoreFactoryTest::suite());
	runner.addTest(vtkALBAAssemblyTest::suite());
	runner.addTest(albaAbsMatrixPipeTest::suite());
	runner.addTest(mmaVolumeMaterialTest::suite());
	runner.addTest(mmaMaterialTest::suite());
	runner.addTest(mmaApplicationLayoutTest::suite());
	runner.addTest(albaVMEStorageTest::suite());
	runner.addTest(albaVMEStorageTest2::suite());
	runner.addTest(albaVMETest::suite());
	runner.addTest(albaVMERootTest::suite());
	runner.addTest(albaVMEOutputTest::suite());
	runner.addTest(albaVMEOutputNULLTest::suite());
	runner.addTest(albaVMEManagerTest::suite());
	runner.addTest(albaViewVTKTest::suite());
	runner.addTest(albaViewPlotTest::suite());
	runner.addTest(albaViewManagerTest::suite());
	runner.addTest(albaViewHTMLTest::suite());
	runner.addTest(albaUserTest::suite());
	runner.addTest(albaTimeMapTest::suite());
	runner.addTest(albaTimeMapScalarTest::suite());
	runner.addTest(albaTagItemTest::suite());
	runner.addTest(albaTagArrayTest::suite());
	runner.addTest(albaSceneNodeTest::suite());
	runner.addTest(albaSceneGraphTest::suite());
	runner.addTest(albaPipeTest::suite());
	runner.addTest(albaPipeFactoryTest::suite());
	runner.addTest(albaPipeBoxTest::suite());
	runner.addTest(albaOpTest::suite());
	runner.addTest(albaOpStackTest::suite());
	runner.addTest(albaOpSelectTest::suite());
	runner.addTest(albaOpPasteTest::suite());
	runner.addTest(albaOpManagerTest::suite());
	runner.addTest(albaOpEditTest::suite());
	//runner.addTest(albaOpCutTest::suite());
	runner.addTest(albaOpContextStackTest::suite());
	runner.addTest(albaOpCopyTest::suite());
	runner.addTest(albaVMETest::suite());
	runner.addTest(albaVMEManagerTest::suite());
	runner.addTest(albaVMEIteratorTest::suite());
	runner.addTest(albaVMEFactoryTest::suite());
	runner.addTest(albaMatrixPipeTest::suite());
	runner.addTest(albaLODActorTest::suite());
	runner.addTest(albaGUITreeContextualMenuTest::suite());
	runner.addTest(albaEventIOTest::suite());
	runner.addTest(albaDataPipeTest::suite());
	runner.addTest(albaAxesTest::suite());
	runner.addTest(albaAttributeTraceabilityTest::suite());
	runner.addTest(albaAttributeTest::suite());
	runner.addTest(albaViewPlotTest::suite());
	runner.addTest(albaProgressBarHelperTest::suite());

	runner.run(controller);

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter(&result, CPPUNIT_NS::stdCOut());
	outputter.write();

	cppDEL(logic);

	return result.wasSuccessful() ? 0 : 1;
}