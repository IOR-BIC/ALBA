/*=========================================================================

 Program: MAF2
 Module: mafInteractorGeneric6DOFTest
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
#include "mafDevice.h"
#include "mafEventInteraction.h"
#include "mafInteractorGeneric6DOFTest.h"
#include "mafInteractorGeneric6DOF.h"
#include "mafDeviceButtonsPadTracker.h"

//----------------------------------------------------------------------------
void mafInteractorGeneric6DOFTest::TestFixture()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void mafInteractorGeneric6DOFTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  mafInteractorGeneric6DOF *interactor6DOF = mafInteractorGeneric6DOF::New();

  interactor6DOF->Delete();
}

void mafInteractorGeneric6DOFTest::TestOnEvent()
{
	// Sending several different events to perform full OnEvent coverage: mostly a leak/crash test
	mafInteractorGeneric6DOF *interactor6DOF = mafInteractorGeneric6DOF::New();

	mafEventInteraction channelEvent;
	channelEvent.SetChannel(MCH_INPUT);
	channelEvent.SetSender(this);
	interactor6DOF->OnEvent(&channelEvent);

	mafDeviceButtonsPadTracker *dummyTracker = mafDeviceButtonsPadTracker::New();

	mafEventInteraction channelEventWithTrackerNotSendingEvents;
	channelEventWithTrackerNotSendingEvents.SetChannel(MCH_INPUT);
	channelEventWithTrackerNotSendingEvents.SetId(mafDeviceButtonsPadTracker::GetTracker3DMoveId());
	channelEventWithTrackerNotSendingEvents.SetSender(this);

	interactor6DOF->SetTracker(dummyTracker);

	interactor6DOF->OnEvent(&channelEventWithTrackerNotSendingEvents);

	mafEventInteraction channelEventWithTrackerSendingEvents;
	channelEventWithTrackerSendingEvents.SetChannel(MCH_INPUT);
	channelEventWithTrackerSendingEvents.SetId(mafDeviceButtonsPadTracker::GetTracker3DMoveId());
	channelEventWithTrackerSendingEvents.SetSender(dummyTracker);

	interactor6DOF->OnEvent(&channelEventWithTrackerSendingEvents);

	dummyTracker->Delete();

	interactor6DOF->Delete();

	assert(true);
}

void mafInteractorGeneric6DOFTest::TestOnMove()
{
	// mostly a leaks/crash test
	mafInteractorGeneric6DOF *interactor6DOF = mafInteractorGeneric6DOF::New();

	mafEventInteraction channelEventWithNoAvatarAvailable;
	channelEventWithNoAvatarAvailable.SetChannel(MCH_INPUT);
	channelEventWithNoAvatarAvailable.SetSender(this);
	interactor6DOF->OnMove(&channelEventWithNoAvatarAvailable);

	interactor6DOF->Delete();

	assert(true);

}

void mafInteractorGeneric6DOFTest::TestSetGetDifferentialMoving()
{
	mafInteractorGeneric6DOF *interactor6DOF = mafInteractorGeneric6DOF::New();

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

void mafInteractorGeneric6DOFTest::TestDifferentialMovingOnOff()
{
	mafInteractorGeneric6DOF *interactor6DOF = mafInteractorGeneric6DOF::New();

	interactor6DOF->DifferentialMovingOn();
	bool differentialMoving = interactor6DOF->GetDifferentialMoving();
	CPPUNIT_ASSERT(differentialMoving == true);

	interactor6DOF->DifferentialMovingOff();
	differentialMoving = interactor6DOF->GetDifferentialMoving();
	CPPUNIT_ASSERT(differentialMoving == false);

	interactor6DOF->Delete();
}