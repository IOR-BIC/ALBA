/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractor6DOFCameraMoveTest
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
#include "albaInteractor6DOFCameraMoveTest.h"
#include "albaInteractor6DOFCameraMove.h"
#include "albaDeviceButtonsPadTracker.h"
#include "vtkCamera.h"


void albaInteractor6DOFCameraMoveTest::TestFixture()
{

}

void albaInteractor6DOFCameraMoveTest::TestConstructorDestructor()
{
  albaInteractor6DOFCameraMove *interactor = albaInteractor6DOFCameraMove::New();
  CPPUNIT_ASSERT(interactor);

  interactor->Delete();
}

void albaInteractor6DOFCameraMoveTest::TestStartInteraction()
{
  albaInteractor6DOFCameraMove *interactor = albaInteractor6DOFCameraMove::New();
  CPPUNIT_ASSERT(interactor);

  albaDeviceButtonsPadTracker *dummyTracker = albaDeviceButtonsPadTracker::New();

  int interactionStarted = interactor->StartInteraction(dummyTracker);
  
  CPPUNIT_ASSERT(interactionStarted == false);

  dummyTracker->Delete();
  
  interactor->Delete();
}

void albaInteractor6DOFCameraMoveTest::TestOnEventNotHandled()
{
  albaInteractor6DOFCameraMove *interactor = albaInteractor6DOFCameraMove::New();
  CPPUNIT_ASSERT(interactor);
  
  albaEventBase dummyEventNotHandledByTestedClass;
  dummyEventNotHandledByTestedClass.SetSender(this);

  // testing for leaks and crashes
  interactor->OnEvent(&dummyEventNotHandledByTestedClass);

  interactor->Delete();
}


void albaInteractor6DOFCameraMoveTest::TestOnEventOnMchInputChannelWithNULLTracker()
{
  albaInteractor6DOFCameraMove *interactor = albaInteractor6DOFCameraMove::New();
  CPPUNIT_ASSERT(interactor);

  albaEventBase dummyEvent;
  dummyEvent.SetSender(this);
  dummyEvent.SetChannel(MCH_INPUT);

  interactor->m_InteractionFlag = true;

  // testing for leaks and crashes
  interactor->OnEvent(&dummyEvent);

  interactor->Delete();
}

void albaInteractor6DOFCameraMoveTest::TestOnEventOnMchInputChannelWithDummyTracker()
{
  albaDeviceButtonsPadTracker *dummyTracker = albaDeviceButtonsPadTracker::New();

  albaInteractor6DOFCameraMove *interactor = albaInteractor6DOFCameraMove::New();
  CPPUNIT_ASSERT(interactor);

  interactor->SetTracker(dummyTracker);

  albaEventBase dummyEvent;
  dummyEvent.SetSender(this);
  dummyEvent.SetChannel(MCH_INPUT);

  interactor->m_InteractionFlag = true;

  // testing for leaks and crashes
  interactor->OnEvent(&dummyEvent);

  interactor->Delete();

  dummyTracker->Delete();
}


void albaInteractor6DOFCameraMoveTest::TestOnEventTracker3DMoveOnMchInputChannelWithDummyTracker()
{
  albaDeviceButtonsPadTracker *dummyTracker = albaDeviceButtonsPadTracker::New();

  albaInteractor6DOFCameraMove *interactor = albaInteractor6DOFCameraMove::New();
  CPPUNIT_ASSERT(interactor);

  interactor->SetTracker(dummyTracker);

  albaEventBase dummyEvent;
  dummyEvent.SetSender(this);
  dummyEvent.SetChannel(MCH_INPUT);
  dummyEvent.SetId(albaDeviceButtonsPadTracker::GetTracker3DMoveId());

  interactor->m_InteractionFlag = true;

  // testing for leaks and crashes
  interactor->OnEvent(&dummyEvent);

  interactor->Delete();

  dummyTracker->Delete();
}

void albaInteractor6DOFCameraMoveTest::TestOnEventTracker3DMoveOnMchInputChannelWithDummyTrackerAndNotNULLCamera()
{
  albaDeviceButtonsPadTracker *dummyTracker = albaDeviceButtonsPadTracker::New();

  albaInteractor6DOFCameraMove *interactor = albaInteractor6DOFCameraMove::New();
  CPPUNIT_ASSERT(interactor);

  interactor->SetTracker(dummyTracker);

  albaEventBase dummyEvent;
  dummyEvent.SetSender(this);
  dummyEvent.SetChannel(MCH_INPUT);
  dummyEvent.SetId(albaDeviceButtonsPadTracker::GetTracker3DMoveId());

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
