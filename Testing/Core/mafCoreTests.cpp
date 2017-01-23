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

#include "mafCoreTests.h"

#include "mafAbsMatrixPipeTest.h"
#include "mafAttributeTest.h"
#include "mafAttributeTraceabilityTest.h"
#include "mafAxesTest.h"
#include "mafCoreFactoryTest.h"
#include "mafDataPipeTest.h"
#include "mafEventIOTest.h"
#include "mafExpirationDateTest.h"
#include "mafFakeLogicForTest.h"
#include "mafGUITreeContextualMenuTest.h"
#include "mafLODActorTest.h"
#include "mafMatrixPipeTest.h"
#include "mafOpContextStackTest.h"
#include "mafOpCopyTest.h"
#include "mafOpCutTest.h"
#include "mafOpEditTest.h"
#include "mafOpManagerTest.h"
#include "mafOpPasteTest.h"
#include "mafOpSelectTest.h"
#include "mafOpStackTest.h"
#include "mafOpTest.h"
#include "mafPipeBoxTest.h"
#include "mafPipeFactoryTest.h"
#include "mafPipeTest.h"
#include "mafProgressBarHelperTest.h"
#include "mafRootTest.h"
#include "mafSceneGraphTest.h"
#include "mafSceneNodeTest.h"
#include "mafServiceLocator.h"
#include "mafTagArrayTest.h"
#include "mafTagItemTest.h"
#include "mafTimeMapScalarTest.h"
#include "mafTimeMapTest.h"
#include "mafUserTest.h"
#include "mafVMEFactoryTest.h"
#include "mafVMEIteratorTest.h"
#include "mafVMEManagerTest.h"
#include "mafVMEManagerTest.h"
#include "mafVMEOutputNULLTest.h"
#include "mafVMEOutputTest.h"
#include "mafVMERootTest.h"
#include "mafVMEStorageTest.h"
#include "mafVMEStorageTest2.h"
#include "mafVMETest.h"
#include "mafVMETest.h"
#include "mafViewHTMLTest.h"
#include "mafViewManagerTest.h"
#include "mafViewPlotTest.h"
#include "mafViewVTKTest.h"
#include "mmaApplicationLayoutTest.h"
#include "mmaMaterialTest.h"
#include "mmaVolumeMaterialTest.h"

#include "TimeSetTest.h"
#include "VMEPoseTest.h"

#include <wx/dir.h>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "vtkFileOutputWindow.h"
#include "vtkMAFAssemblyTest.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEHelper);

//----------------------------------------------------------------------------
mafVMEHelper::mafVMEHelper()
{
	mafNEW(m_Transform);
	mafVMEOutputNULL *output = mafVMEOutputNULL::New();
	output->SetTransform(m_Transform);
	SetOutput(output);
}


//----------------------------------------------------------------------------
void DummyObserver::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		mafEvent *eventToCopy = new mafEvent(e->GetSender(),e->GetId(),e->GetVme());
		m_ListEvent.push_back(eventToCopy);
	}
}

//----------------------------------------------------------------------------
DummyObserver::~DummyObserver()
//----------------------------------------------------------------------------
{
	for(int i=0;i<m_ListEvent.size();i++)
	{
		delete m_ListEvent[i];
	}
	m_ListEvent.clear();
}


//----------------------------------------------------------------------------
mafOpDummyHelper::mafOpDummyHelper(wxString label,  bool canundo, int opType, bool inputPreserving)
//----------------------------------------------------------------------------
{
	m_Canundo = canundo;
	m_OpType = opType;
	m_InputPreserving = inputPreserving;
}


//----------------------------------------------------------------------------
bool RemoveDir(const char *dirName)
//----------------------------------------------------------------------------
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

//Main Test Executor
int
main(int argc, char* argv[])
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
	controller.addListener(&result);

	// Add a listener that print dots as test run.
	CPPUNIT_NS::BriefTestProgressListener progress;
	controller.addListener(&progress);

	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;

	mafFakeLogicForTest *logic = new mafFakeLogicForTest();
	mafServiceLocator::SetLogicManager(logic);

	//this test must be run first because manages factories
	runner.addTest(mafCoreFactoryTest::suite());
	runner.addTest(vtkMAFAssemblyTest::suite());
	runner.addTest(mafAbsMatrixPipeTest::suite());
	runner.addTest(mmaVolumeMaterialTest::suite());
	runner.addTest(mmaMaterialTest::suite());
	runner.addTest(mmaApplicationLayoutTest::suite());
	runner.addTest(mafVMEStorageTest::suite());
	runner.addTest(mafVMEStorageTest2::suite());
	runner.addTest(mafVMETest::suite());
	runner.addTest(mafVMERootTest::suite());
	runner.addTest(mafVMEOutputTest::suite());
	runner.addTest(mafVMEOutputNULLTest::suite());
	runner.addTest(mafVMEManagerTest::suite());
	runner.addTest(mafViewVTKTest::suite());
	runner.addTest(mafViewPlotTest::suite());
	runner.addTest(mafViewManagerTest::suite());
	runner.addTest(mafViewHTMLTest::suite());
	runner.addTest(mafUserTest::suite());
	runner.addTest(mafTimeMapTest::suite());
	runner.addTest(mafTimeMapScalarTest::suite());
	runner.addTest(mafTagItemTest::suite());
	runner.addTest(mafTagArrayTest::suite());
	runner.addTest(mafSceneNodeTest::suite());
	runner.addTest(mafSceneGraphTest::suite());
	runner.addTest(mafRootTest::suite());
	runner.addTest(mafPipeTest::suite());
	runner.addTest(mafPipeFactoryTest::suite());
	runner.addTest(mafPipeBoxTest::suite());
	runner.addTest(mafOpTest::suite());
	runner.addTest(mafOpStackTest::suite());
	runner.addTest(mafOpSelectTest::suite());
	runner.addTest(mafOpPasteTest::suite());
	runner.addTest(mafOpManagerTest::suite());
	runner.addTest(mafOpEditTest::suite());
	runner.addTest(mafOpCutTest::suite());
	runner.addTest(mafOpContextStackTest::suite());
	runner.addTest(mafOpCopyTest::suite());
	runner.addTest(mafVMETest::suite());
	runner.addTest(mafVMEManagerTest::suite());
	runner.addTest(mafVMEIteratorTest::suite());
	runner.addTest(mafVMEFactoryTest::suite());
	runner.addTest(mafMatrixPipeTest::suite());
	runner.addTest(mafLODActorTest::suite());
	runner.addTest(mafGUITreeContextualMenuTest::suite());
	runner.addTest(mafExpirationDateTest::suite());
	runner.addTest(mafEventIOTest::suite());
	runner.addTest(mafDataPipeTest::suite());
	runner.addTest(mafAxesTest::suite());
	runner.addTest(mafAttributeTraceabilityTest::suite());
	runner.addTest(mafAttributeTest::suite());
	runner.addTest(mafViewPlotTest::suite());
	runner.addTest(mafProgressBarHelperTest::suite());

	runner.run(controller);

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter(&result, CPPUNIT_NS::stdCOut());
	outputter.write();

	cppDEL(logic);

	return result.wasSuccessful() ? 0 : 1;
}

