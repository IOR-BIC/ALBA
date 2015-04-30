/*=========================================================================

 Program: MAF2
 Module: mafInteractor6DOFTest
 Authors: Stefano Perticoni
 
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

#include <cppunit/config/SourcePrefix.h>
#include "mafObject.h"
#include "vtkRenderer.h"
#include "mafDevice.h"
#include "mafEventInteraction.h"
#include "mafInteractor6DOF.h"
#include "mafInteractor6DOFTest.h"
#include "vtkMAFSmartPointer.h"
#include "mafDeviceButtonsPadTracker.h"

//----------------------------------------------------------------------------
class mafInteractor6DOFDummy : public mafInteractor6DOF // concrete class for test
//----------------------------------------------------------------------------
{
public:

  mafInteractor6DOFDummy(){};
  ~mafInteractor6DOFDummy(){};

  mafTypeMacro(mafInteractor6DOFDummy,mafInteractor6DOF);
};

mafCxxTypeMacro(mafInteractor6DOFDummy);

//----------------------------------------------------------------------------
void mafInteractor6DOFTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafInteractor6DOFTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafInteractor6DOFTest::TestFixture()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void mafInteractor6DOFTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  mafInteractor6DOFDummy *interactor = mafInteractor6DOFDummy::New();
  cppDEL(interactor);
}

void mafInteractor6DOFTest::TestStartStopInteraction()
{
	mafInteractor6DOFDummy *interactor = mafInteractor6DOFDummy::New();

	mafDeviceButtonsPadTracker *tracker;
	mafNEW(tracker);

	interactor->SetDevice(tracker);
	CPPUNIT_ASSERT( interactor->IsInteracting(tracker) == false );

	interactor->StartInteraction(tracker);
	CPPUNIT_ASSERT( interactor->IsInteracting(tracker) == false );

	// 1 leak in StopInteraction call 
	interactor->StopInteraction(tracker);
	CPPUNIT_ASSERT( interactor->IsInteracting(tracker) == false );

	mafDEL(tracker);
	mafDEL(interactor);

}

void mafInteractor6DOFTest::TestSetGetTrackerPoseMatrix()
{
	mafInteractor6DOFDummy *interactor = mafInteractor6DOFDummy::New();

	mafMatrix *matrix = NULL;

	mafMatrix *identity = mafMatrix::New();

	matrix = interactor->GetTrackerPoseMatrix();
	CPPUNIT_ASSERT(matrix != NULL);
	CPPUNIT_ASSERT(matrix->Equals(identity));

	mafMatrix *dummy = mafMatrix::New();
	dummy->SetElement(0,3, 15);
	
	interactor->SetTrackerPoseMatrix(dummy);
	CPPUNIT_ASSERT(interactor->GetTrackerPoseMatrix()->Equals(dummy));
	
	cppDEL(interactor);
	mafDEL(dummy);
	mafDEL(identity);
}

void mafInteractor6DOFTest::TestTrackerSnapshot()
{
	mafInteractor6DOFDummy *interactor = mafInteractor6DOFDummy::New();

	mafMatrix *dummyMatrix = mafMatrix::New();
	dummyMatrix->SetElement(0,3, 15);

	mafMatrix *dummyMatrixDeepCopy = NULL;
	dummyMatrixDeepCopy = mafMatrix::New();
	dummyMatrixDeepCopy->DeepCopy(dummyMatrix);

	interactor->TrackerSnapshot(dummyMatrix);

	// test that the given matrix is not changed by client
	CPPUNIT_ASSERT(dummyMatrix->Equals(dummyMatrixDeepCopy) == true);
	cppDEL(interactor);

	mafDEL(dummyMatrix);
	mafDEL(dummyMatrixDeepCopy);

}

void mafInteractor6DOFTest::TestUpdateDeltaTransform()
{
	mafInteractor6DOFDummy *interactor = mafInteractor6DOFDummy::New();
	
	// mostly a test for leaks...
	interactor->UpdateDeltaTransform();
	
	CPPUNIT_ASSERT(true);
	cppDEL(interactor);
}

void mafInteractor6DOFTest::TestSetGetTracker()
{
	mafInteractor6DOFDummy *interactor = mafInteractor6DOFDummy::New();
	CPPUNIT_ASSERT(interactor->GetTracker() == NULL);

	mafDeviceButtonsPadTracker *dummyTracker;
	mafNEW(dummyTracker);
	
	interactor->SetTracker(dummyTracker);
	CPPUNIT_ASSERT(interactor->GetTracker() == dummyTracker);

	mafDEL(dummyTracker);
	cppDEL(interactor);
}

void mafInteractor6DOFTest::TestSetGetIgnoreTriggerEvents()
{
	mafInteractor6DOFDummy *interactor = mafInteractor6DOFDummy::New();

	CPPUNIT_ASSERT( interactor->GetIgnoreTriggerEvents() == false);
	interactor->SetIgnoreTriggerEvents(true);
	CPPUNIT_ASSERT( interactor->GetIgnoreTriggerEvents() == true);
	cppDEL(interactor);
}

void mafInteractor6DOFTest::TestIgnoreTriggerEventsOnOff()
{
	mafInteractor6DOFDummy *interactor = mafInteractor6DOFDummy::New();

	CPPUNIT_ASSERT( interactor->GetIgnoreTriggerEvents() == false);
	interactor->IgnoreTriggerEventsOn();
	
	CPPUNIT_ASSERT( interactor->GetIgnoreTriggerEvents() == true);

	interactor->IgnoreTriggerEventsOff();
	
	CPPUNIT_ASSERT( interactor->GetIgnoreTriggerEvents() == false);

	cppDEL(interactor);
}

void mafInteractor6DOFTest::TestSetRenderer()
{
	mafInteractor6DOFDummy *interactor = mafInteractor6DOFDummy::New();
	vtkRenderer *dummyRenderer = vtkRenderer::New();

	CPPUNIT_ASSERT(interactor->GetRenderer() == NULL);

	interactor->SetRenderer(dummyRenderer);
	CPPUNIT_ASSERT(interactor->GetRenderer() == dummyRenderer);

	dummyRenderer->Delete();
	cppDEL(interactor);
}

void mafInteractor6DOFTest::TestHideShowAvatar()
{
	mafInteractor6DOFDummy *interactor = mafInteractor6DOFDummy::New();
	
	// test for leaks and crashes...
	interactor->ShowAvatar();
	interactor->HideAvatar();

	cppDEL(interactor);
}


