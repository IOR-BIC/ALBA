/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafInteractor6DOFTest.cpp,v $
Language:  C++
Date:      $Date: 2011-05-25 11:58:32 $
Version:   $Revision: 1.1.2.2 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
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
class mafInteractorDummy : public mafInteractor6DOF // concrete class for test
//----------------------------------------------------------------------------
{
public:

  mafInteractorDummy(){};
  ~mafInteractorDummy(){};

  mafTypeMacro(mafInteractorDummy,mafInteractor6DOF);
};

mafCxxTypeMacro(mafInteractorDummy);

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
  mafInteractorDummy *interactor = mafInteractorDummy::New();
  cppDEL(interactor);
}

void mafInteractor6DOFTest::TestStartStopInteraction()
{
	mafInteractorDummy *interactor = mafInteractorDummy::New();

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
	mafInteractorDummy *interactor = mafInteractorDummy::New();

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
	mafInteractorDummy *interactor = mafInteractorDummy::New();

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
	mafInteractorDummy *interactor = mafInteractorDummy::New();
	
	// mostly a test for leaks...
	interactor->UpdateDeltaTransform();
	
	CPPUNIT_ASSERT(true);
	cppDEL(interactor);
}

void mafInteractor6DOFTest::TestSetGetTracker()
{
	mafInteractorDummy *interactor = mafInteractorDummy::New();
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
	mafInteractorDummy *interactor = mafInteractorDummy::New();

	CPPUNIT_ASSERT( interactor->GetIgnoreTriggerEvents() == false);
	interactor->SetIgnoreTriggerEvents(true);
	CPPUNIT_ASSERT( interactor->GetIgnoreTriggerEvents() == true);
	cppDEL(interactor);
}

void mafInteractor6DOFTest::TestIgnoreTriggerEventsOnOff()
{
	mafInteractorDummy *interactor = mafInteractorDummy::New();

	CPPUNIT_ASSERT( interactor->GetIgnoreTriggerEvents() == false);
	interactor->IgnoreTriggerEventsOn();
	
	CPPUNIT_ASSERT( interactor->GetIgnoreTriggerEvents() == true);

	interactor->IgnoreTriggerEventsOff();
	
	CPPUNIT_ASSERT( interactor->GetIgnoreTriggerEvents() == false);

	cppDEL(interactor);
}

void mafInteractor6DOFTest::TestSetRenderer()
{
	mafInteractorDummy *interactor = mafInteractorDummy::New();
	vtkRenderer *dummyRenderer = vtkRenderer::New();

	CPPUNIT_ASSERT(interactor->GetRenderer() == NULL);

	interactor->SetRenderer(dummyRenderer);
	CPPUNIT_ASSERT(interactor->GetRenderer() == dummyRenderer);

	dummyRenderer->Delete();
	cppDEL(interactor);
}

void mafInteractor6DOFTest::TestHideShowAvatar()
{
	mafInteractorDummy *interactor = mafInteractorDummy::New();
	
	// test for leaks and crashes...
	interactor->ShowAvatar();
	interactor->HideAvatar();

	cppDEL(interactor);
}


