/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorGeneric6DOFTest
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
#include "albaDevice.h"
#include "albaEventInteraction.h"
#include "albaInteractorGeneric6DOFTest.h"
#include "albaInteractorGeneric6DOF.h"
#include "albaDeviceButtonsPadTracker.h"

//----------------------------------------------------------------------------
void albaInteractorGeneric6DOFTest::TestFixture()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void albaInteractorGeneric6DOFTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  albaInteractorGeneric6DOF *interactor6DOF = albaInteractorGeneric6DOF::New();

  interactor6DOF->Delete();
}

void albaInteractorGeneric6DOFTest::TestOnEvent()
{
	// Sending several different events to perform full OnEvent coverage: mostly a leak/crash test
	albaInteractorGeneric6DOF *interactor6DOF = albaInteractorGeneric6DOF::New();

	albaEventInteraction channelEvent;
	channelEvent.SetChannel(MCH_INPUT);
	channelEvent.SetSender(this);
	interactor6DOF->OnEvent(&channelEvent);

	albaDeviceButtonsPadTracker *dummyTracker = albaDeviceButtonsPadTracker::New();

	albaEventInteraction channelEventWithTrackerNotSendingEvents;
	channelEventWithTrackerNotSendingEvents.SetChannel(MCH_INPUT);
	channelEventWithTrackerNotSendingEvents.SetId(albaDeviceButtonsPadTracker::GetTracker3DMoveId());
	channelEventWithTrackerNotSendingEvents.SetSender(this);

	interactor6DOF->SetTracker(dummyTracker);

	interactor6DOF->OnEvent(&channelEventWithTrackerNotSendingEvents);

	albaEventInteraction channelEventWithTrackerSendingEvents;
	channelEventWithTrackerSendingEvents.SetChannel(MCH_INPUT);
	channelEventWithTrackerSendingEvents.SetId(albaDeviceButtonsPadTracker::GetTracker3DMoveId());
	channelEventWithTrackerSendingEvents.SetSender(dummyTracker);

	interactor6DOF->OnEvent(&channelEventWithTrackerSendingEvents);

	dummyTracker->Delete();

	interactor6DOF->Delete();

	assert(true);
}

void albaInteractorGeneric6DOFTest::TestOnMove()
{
	// mostly a leaks/crash test
	albaInteractorGeneric6DOF *interactor6DOF = albaInteractorGeneric6DOF::New();

	albaEventInteraction channelEventWithNoAvatarAvailable;
	channelEventWithNoAvatarAvailable.SetChannel(MCH_INPUT);
	channelEventWithNoAvatarAvailable.SetSender(this);
	interactor6DOF->OnMove(&channelEventWithNoAvatarAvailable);

	interactor6DOF->Delete();

	assert(true);

}

void albaInteractorGeneric6DOFTest::TestSetGetDifferentialMoving()
{
	albaInteractorGeneric6DOF *interactor6DOF = albaInteractorGeneric6DOF::New();

	bool differentialMoving = interactor6DOF->GetDifferentialMoving();
	CPPUNIT_ASSERT(differentialMoving == true);

	interactor6DOF->SetDifferentialMoving(true);
	differentialMoving = interactor6DOF->GetDifferentialMoving();
	CPPUNIT_ASSERT(differentialMoving == true);
	
	interactor6DOF->SetDifferentialMoving(false);
	differentialMoving = interactor6DOF->GetDifferentialMoving();
	CPPUNIT_ASSERT(differentialMoving == false);

	interactor6DOF->Delete();
}

void albaInteractorGeneric6DOFTest::TestDifferentialMovingOnOff()
{
	albaInteractorGeneric6DOF *interactor6DOF = albaInteractorGeneric6DOF::New();

	interactor6DOF->DifferentialMovingOn();
	bool differentialMoving = interactor6DOF->GetDifferentialMoving();
	CPPUNIT_ASSERT(differentialMoving == true);

	interactor6DOF->DifferentialMovingOff();
	differentialMoving = interactor6DOF->GetDifferentialMoving();
	CPPUNIT_ASSERT(differentialMoving == false);

	interactor6DOF->Delete();
}