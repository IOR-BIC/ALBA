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
#include "mafViewVTKTest.h"
#include "mafViewHTMLTest.h"
#include "mafAttributeTest.h"
#include "mafTagItemTest.h"
#include "mafTagArrayTest.h"
#include "mafNodeTest.h"
#include "mafTimeMapTest.h"
#include "mafNodeFactoryTest.h"
#include "mmaApplicationLayoutTest.h"
#include "mafVMETest.h"
#include "mafOpTest.h"
#include "mafEventIOTest.h"
#include "mafUserTest.h"
#include "mafLODActorTest.h"
#include "mafTimeMapScalarTest.h"
#include "mafOpSelectTest.h"
#include "mafRootTest.h"
#include "mafVMERootTest.h"
#include "mafOpCutTest.h"
#include "mafOpCopyTest.h"
#include "mafOpPasteTest.h"
#include "mafOpEditTest.h"
#include "mafOpContextStackTest.h"
#include "mafNodeIteratorTest.h"
#include "mafNodeLayoutTest.h"
#include "mafPipeTest.h"
#include "mafMatrixPipeTest.h"
#include "mafPipeBoxTest.h"
#include "mafVMEOutputTest.h"
#include "mafDataPipeTest.h"
#include "mafNodeRootTest.h"
#include "mafPipeFactoryTest.h"
#include "mmaMaterialTest.h"
#include "mmaVolumeMaterialTest.h"
#include "mafSceneNodeTest.h"
#include "mafSceneGraphTest.h"
#include "mafVMEOutputNULLTest.h"
#include "mafVMEStorageTest.h"
#include "mafVMEStorageTest2.h"
#include "mafGUITreeContextualMenuTest.h"
#include "mafAttributeTraceabilityTest.h"
#include "mafAxesTest.h"
#include "mafCoreFactoryTest.h"
#include "mafOpStackTest.h"
#include "mafNodeGenericTest.h"
#include "mafExpirationDateTest.h"
#include "mafNodeManagerTest.h"
#include "mafOpManagerTest.h"
#include "mafViewManagerTest.h"
#include "mafVMEManagerTest.h"
#include "mafAbsMatrixPipeTest.h"
#include "vtkMAFAssemblyTest.h"
#include "mafViewPlotTest.h"
#include "mafProgressBarHelperTest.h"
#include "TimeSetTest.h"
#include "VMEPoseTest.h"
#include <wx/dir.h>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafNodeHelper);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafNodeBHelper)
//-------------------------------------------------------------------------

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

	//this test must be run first because manages factories
	runner.addTest( mafCoreFactoryTest::suite());

	runner.addTest( vtkMAFAssemblyTest::suite());
	runner.addTest( mafAbsMatrixPipeTest::suite());
	runner.addTest( mmaVolumeMaterialTest::suite());
	runner.addTest( mmaMaterialTest::suite());
	runner.addTest( mmaApplicationLayoutTest::suite());
	runner.addTest( mafVMEStorageTest::suite());
	runner.addTest( mafVMEStorageTest2::suite());
	runner.addTest( mafVMETest::suite());
	runner.addTest( mafVMERootTest::suite());
	runner.addTest( mafVMEOutputTest::suite());
	runner.addTest( mafVMEOutputNULLTest::suite());
	runner.addTest( mafVMEManagerTest::suite());
	runner.addTest( mafViewVTKTest::suite());
	runner.addTest( mafViewPlotTest::suite());
  runner.addTest( mafViewManagerTest::suite());
	runner.addTest( mafViewHTMLTest::suite());
	runner.addTest( mafUserTest::suite());
	runner.addTest( mafTimeMapTest::suite());
  runner.addTest( mafTimeMapScalarTest::suite());
  runner.addTest( mafTagItemTest::suite());
	runner.addTest( mafTagArrayTest::suite());
	runner.addTest( mafSceneNodeTest::suite());
	runner.addTest( mafSceneGraphTest::suite());
	runner.addTest( mafRootTest::suite());
	runner.addTest( mafPipeTest::suite());
	runner.addTest( mafPipeFactoryTest::suite());
	runner.addTest( mafPipeBoxTest::suite());
	runner.addTest( mafOpTest::suite());
	runner.addTest( mafOpStackTest::suite());
	runner.addTest( mafOpSelectTest::suite());
	runner.addTest( mafOpPasteTest::suite());
	runner.addTest( mafOpManagerTest::suite() );
	runner.addTest( mafOpEditTest::suite());
  runner.addTest( mafOpCutTest::suite());
	runner.addTest( mafOpContextStackTest::suite());
	runner.addTest( mafOpCopyTest::suite());
  runner.addTest( mafNodeTest::suite());
	runner.addTest( mafNodeRootTest::suite());
	runner.addTest( mafNodeManagerTest::suite() );
	runner.addTest( mafNodeLayoutTest::suite());
  runner.addTest( mafNodeIteratorTest::suite());
  runner.addTest( mafNodeGenericTest::suite());
  runner.addTest( mafNodeFactoryTest::suite());
  runner.addTest( mafMatrixPipeTest::suite());
	runner.addTest( mafLODActorTest::suite());
  runner.addTest( mafGUITreeContextualMenuTest::suite());
  runner.addTest( mafExpirationDateTest::suite());
	runner.addTest( mafEventIOTest::suite());
	runner.addTest( mafDataPipeTest::suite());
	runner.addTest( mafAxesTest::suite());
  runner.addTest( mafAttributeTraceabilityTest::suite());
	runner.addTest( mafAttributeTest::suite());
	runner.addTest( mafViewPlotTest::suite());
	runner.addTest( mafProgressBarHelperTest::suite());
	
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

