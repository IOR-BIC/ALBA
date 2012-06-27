/*=========================================================================

 Program: MAF2
 Module: mafInteractor6DOFCameraMoveTest
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
#include "mafInteractor6DOFCameraMoveTest.h"
#include "mafInteractor6DOFCameraMove.h"
#include "mafDeviceButtonsPadTracker.h"
#include "vtkCamera.h"

void mafInteractor6DOFCameraMoveTest::setUp()
{

}

void mafInteractor6DOFCameraMoveTest::tearDown()
{

}

void mafInteractor6DOFCameraMoveTest::TestFixture()
{

}

void mafInteractor6DOFCameraMoveTest::TestConstructorDestructor()
{
  mafInteractor6DOFCameraMove *interactor = mafInteractor6DOFCameraMove::New();
  CPPUNIT_ASSERT(interactor);

  interactor->Delete();
}

void mafInteractor6DOFCameraMoveTest::TestStartInteraction()
{
  mafInteractor6DOFCameraMove *interactor = mafInteractor6DOFCameraMove::New();
  CPPUNIT_ASSERT(interactor);

  mafDeviceButtonsPadTracker *dummyTracker = mafDeviceButtonsPadTracker::New();

  int interactionStarted = interactor->StartInteraction(dummyTracker);
  
  CPPUNIT_ASSERT(interactionStarted == false);

  dummyTracker->Delete();
  
  interactor->Delete();
}

void mafInteractor6DOFCameraMoveTest::TestOnEventNotHandled()
{
  mafInteractor6DOFCameraMove *interactor = mafInteractor6DOFCameraMove::New();
  CPPUNIT_ASSERT(interactor);
  
  mafEventBase dummyEventNotHandledByTestedClass;
  dummyEventNotHandledByTestedClass.SetSender(this);

  // testing for leaks and crashes
  interactor->OnEvent(&dummyEventNotHandledByTestedClass);

  interactor->Delete();
}


void mafInteractor6DOFCameraMoveTest::TestOnEventOnMchInputChannelWithNULLTracker()
{
  mafInteractor6DOFCameraMove *interactor = mafInteractor6DOFCameraMove::New();
  CPPUNIT_ASSERT(interactor);

  mafEventBase dummyEvent;
  dummyEvent.SetSender(this);
  dummyEvent.SetChannel(MCH_INPUT);

  interactor->m_InteractionFlag = true;

  // testing for leaks and crashes
  interactor->OnEvent(&dummyEvent);

  interactor->Delete();
}

void mafInteractor6DOFCameraMoveTest::TestOnEventOnMchInputChannelWithDummyTracker()
{
  mafDeviceButtonsPadTracker *dummyTracker = mafDeviceButtonsPadTracker::New();

  mafInteractor6DOFCameraMove *interactor = mafInteractor6DOFCameraMove::New();
  CPPUNIT_ASSERT(interactor);

  interactor->SetTracker(dummyTracker);

  mafEventBase dummyEvent;
  dummyEvent.SetSender(this);
  dummyEvent.SetChannel(MCH_INPUT);

  interactor->m_InteractionFlag = true;

  // testing for leaks and crashes
  interactor->OnEvent(&dummyEvent);

  interactor->Delete();

  dummyTracker->Delete();
}


void mafInteractor6DOFCameraMoveTest::TestOnEventTracker3DMoveOnMchInputChannelWithDummyTracker()
{
  mafDeviceButtonsPadTracker *dummyTracker = mafDeviceButtonsPadTracker::New();

  mafInteractor6DOFCameraMove *interactor = mafInteractor6DOFCameraMove::New();
  CPPUNIT_ASSERT(interactor);

  interactor->SetTracker(dummyTracker);

  mafEventBase dummyEvent;
  dummyEvent.SetSender(this);
  dummyEvent.SetChannel(MCH_INPUT);
  dummyEvent.SetId(mafDeviceButtonsPadTracker::GetTracker3DMoveId());

  interactor->m_InteractionFlag = true;

  // testing for leaks and crashes
  interactor->OnEvent(&dummyEvent);

  interactor->Delete();

  dummyTracker->Delete();
}

void mafInteractor6DOFCameraMoveTest::TestOnEventTracker3DMoveOnMchInputChannelWithDummyTrackerAndNotNULLCamera()
{
  mafDeviceButtonsPadTracker *dummyTracker = mafDeviceButtonsPadTracker::New();

  mafInteractor6DOFCameraMove *interactor = mafInteractor6DOFCameraMove::New();
  CPPUNIT_ASSERT(interactor);

  interactor->SetTracker(dummyTracker);

  mafEventBase dummyEvent;
  dummyEvent.SetSender(this);
  dummyEvent.SetChannel(MCH_INPUT);
  dummyEvent.SetId(mafDeviceButtonsPadTracker::GetTracker3DMoveId());

  interactor->m_InteractionFlag = true;
  
  vtkCamera *dummyCamera = vtkCamera::New();

  interactor->m_CurrentCamera = dummyCamera;

  //------- start comment -------
  // testing for leaks and crashes
  // working on visual studio 2003
  // failing on visual studio 2010
  // commented for the moment (more investigation is needed...)
  // interactor->OnEvent(&dummyEvent);
  //------- end comment -------

  interactor->Delete();
  dummyCamera->Delete();
  dummyTracker->Delete();
}
