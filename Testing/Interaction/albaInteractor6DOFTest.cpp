/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractor6DOFTest
 Authors: Stefano Perticoni
 
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

#include <cppunit/config/SourcePrefix.h>
#include "albaObject.h"
#include "vtkRenderer.h"
#include "albaDevice.h"
#include "albaEventInteraction.h"
#include "albaInteractor6DOF.h"
#include "albaInteractor6DOFTest.h"
#include "vtkALBASmartPointer.h"
#include "albaDeviceButtonsPadTracker.h"

//----------------------------------------------------------------------------
class albaInteractor6DOFDummy : public albaInteractor6DOF // concrete class for test
//----------------------------------------------------------------------------
{
public:

  albaInteractor6DOFDummy(){};
  ~albaInteractor6DOFDummy(){};

  albaTypeMacro(albaInteractor6DOFDummy,albaInteractor6DOF);
};

albaCxxTypeMacro(albaInteractor6DOFDummy);

//----------------------------------------------------------------------------
void albaInteractor6DOFTest::TestFixture()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void albaInteractor6DOFTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  albaInteractor6DOFDummy *interactor = albaInteractor6DOFDummy::New();
  cppDEL(interactor);
}

void albaInteractor6DOFTest::TestStartStopInteraction()
{
	albaInteractor6DOFDummy *interactor = albaInteractor6DOFDummy::New();

	albaDeviceButtonsPadTracker *tracker;
	albaNEW(tracker);

	interactor->SetDevice(tracker);
	CPPUNIT_ASSERT( interactor->IsInteracting(tracker) == false );

	interactor->StartInteraction(tracker);
	CPPUNIT_ASSERT( interactor->IsInteracting(tracker) == false );

	// 1 leak in StopInteraction call 
	interactor->StopInteraction(tracker);
	CPPUNIT_ASSERT( interactor->IsInteracting(tracker) == false );

	albaDEL(tracker);
	albaDEL(interactor);

}

void albaInteractor6DOFTest::TestSetGetTrackerPoseMatrix()
{
	albaInteractor6DOFDummy *interactor = albaInteractor6DOFDummy::New();

	albaMatrix *matrix = NULL;

	albaMatrix *identity = albaMatrix::New();

	matrix = interactor->GetTrackerPoseMatrix();
	CPPUNIT_ASSERT(matrix != NULL);
	CPPUNIT_ASSERT(matrix->Equals(identity));

	albaMatrix *dummy = albaMatrix::New();
	dummy->SetElement(0,3, 15);
	
	interactor->SetTrackerPoseMatrix(dummy);
	CPPUNIT_ASSERT(interactor->GetTrackerPoseMatrix()->Equals(dummy));
	
	cppDEL(interactor);
	albaDEL(dummy);
	albaDEL(identity);
}

void albaInteractor6DOFTest::TestTrackerSnapshot()
{
	albaInteractor6DOFDummy *interactor = albaInteractor6DOFDummy::New();

	albaMatrix *dummyMatrix = albaMatrix::New();
	dummyMatrix->SetElement(0,3, 15);

	albaMatrix *dummyMatrixDeepCopy = NULL;
	dummyMatrixDeepCopy = albaMatrix::New();
	dummyMatrixDeepCopy->DeepCopy(dummyMatrix);

	interactor->TrackerSnapshot(dummyMatrix);

	// test that the given matrix is not changed by client
	CPPUNIT_ASSERT(dummyMatrix->Equals(dummyMatrixDeepCopy) == true);
	cppDEL(interactor);

	albaDEL(dummyMatrix);
	albaDEL(dummyMatrixDeepCopy);

}

void albaInteractor6DOFTest::TestUpdateDeltaTransform()
{
	albaInteractor6DOFDummy *interactor = albaInteractor6DOFDummy::New();
	
	// mostly a test for leaks...
	interactor->UpdateDeltaTransform();
	
	cppDEL(interactor);
}

void albaInteractor6DOFTest::TestSetGetTracker()
{
	albaInteractor6DOFDummy *interactor = albaInteractor6DOFDummy::New();
	CPPUNIT_ASSERT(interactor->GetTracker() == NULL);

	albaDeviceButtonsPadTracker *dummyTracker;
	albaNEW(dummyTracker);
	
	interactor->SetTracker(dummyTracker);
	CPPUNIT_ASSERT(interactor->GetTracker() == dummyTracker);

	albaDEL(dummyTracker);
	cppDEL(interactor);
}

void albaInteractor6DOFTest::TestSetGetIgnoreTriggerEvents()
{
	albaInteractor6DOFDummy *interactor = albaInteractor6DOFDummy::New();

	CPPUNIT_ASSERT( interactor->GetIgnoreTriggerEvents() == false);
	interactor->SetIgnoreTriggerEvents(true);
	CPPUNIT_ASSERT( interactor->GetIgnoreTriggerEvents() == true);
	cppDEL(interactor);
}

void albaInteractor6DOFTest::TestIgnoreTriggerEventsOnOff()
{
	albaInteractor6DOFDummy *interactor = albaInteractor6DOFDummy::New();

	CPPUNIT_ASSERT( interactor->GetIgnoreTriggerEvents() == false);
	interactor->IgnoreTriggerEventsOn();
	
	CPPUNIT_ASSERT( interactor->GetIgnoreTriggerEvents() == true);

	interactor->IgnoreTriggerEventsOff();
	
	CPPUNIT_ASSERT( interactor->GetIgnoreTriggerEvents() == false);

	cppDEL(interactor);
}

void albaInteractor6DOFTest::TestSetRenderer()
{
	albaInteractor6DOFDummy *interactor = albaInteractor6DOFDummy::New();
	vtkRenderer *dummyRenderer = vtkRenderer::New();

	CPPUNIT_ASSERT(interactor->GetRenderer() == NULL);

	interactor->SetRendererAndView(dummyRenderer,NULL);
	CPPUNIT_ASSERT(interactor->GetRenderer() == dummyRenderer);

	dummyRenderer->Delete();
	cppDEL(interactor);
}

void albaInteractor6DOFTest::TestHideShowAvatar()
{
	albaInteractor6DOFDummy *interactor = albaInteractor6DOFDummy::New();
	
	// test for leaks and crashes...
	interactor->ShowAvatar();
	interactor->HideAvatar();

	cppDEL(interactor);
}


