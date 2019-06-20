/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorCompositorMouseTest
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
#include "albaInteractorCompositorMouseTest.h"
#include "albaInteractorCompositorMouse.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaEventInteraction.h"


void albaInteractorCompositorMouseTest::TestFixture()
{

}

void albaInteractorCompositorMouseTest::TestConstructorDestructor()
{
  albaInteractorCompositorMouse *compositorMouse = NULL;
  compositorMouse = albaInteractorCompositorMouse::New();
  CPPUNIT_ASSERT(compositorMouse != NULL);
  compositorMouse->Delete(); 
}

void albaInteractorCompositorMouseTest::TestStartInteraction()
{
  albaInteractorCompositorMouse *compositorMouse = albaInteractorCompositorMouse::New();
  CPPUNIT_ASSERT(compositorMouse != NULL);
  
  albaDeviceButtonsPadMouse *dummyMouse = albaDeviceButtonsPadMouse::New();
  CPPUNIT_ASSERT(dummyMouse->GetRenderer() == NULL);

  // cannot start interaction since there is no renderer: should return false
  int returnValue = compositorMouse->StartInteraction(dummyMouse);
  CPPUNIT_ASSERT(returnValue == false);

  compositorMouse->Delete(); 
  dummyMouse->Delete();
}

void albaInteractorCompositorMouseTest::TestOnEvent()
{
  albaInteractorCompositorMouse *compositorMouse = NULL;
  compositorMouse = albaInteractorCompositorMouse::New();
  CPPUNIT_ASSERT(compositorMouse != NULL);
  
  CPPUNIT_ASSERT(compositorMouse->m_MousePose[0] == 0  && 
  compositorMouse->m_MousePose[1] == 0);

  CPPUNIT_ASSERT(compositorMouse->m_LastMousePose[0] == 0  && 
  compositorMouse->m_LastMousePose[1] == 0);

  CPPUNIT_ASSERT(compositorMouse->m_InteractionFlag == false);

  // trying to create a fake mouse device to fool the compositor OnEvent...
  compositorMouse->m_InteractionFlag = true;
  
  albaDeviceButtonsPadMouse *dummyMouse = albaDeviceButtonsPadMouse::New();
  CPPUNIT_ASSERT(dummyMouse !=  NULL);
  compositorMouse->m_Device = dummyMouse;

  albaEventInteraction dummyMouse2DMoveEvent;
  dummyMouse2DMoveEvent.SetChannel(MCH_INPUT);
  dummyMouse2DMoveEvent.SetSender(this);
  dummyMouse2DMoveEvent.SetId(albaDeviceButtonsPadMouse::GetMouse2DMoveId());

  double dummyPosition[2] = {10,20};
  dummyMouse2DMoveEvent.Set2DPosition(dummyPosition);

  compositorMouse->OnEvent(&dummyMouse2DMoveEvent);
  
  // ...and then check that the interactor is not fooled 
  CPPUNIT_ASSERT(compositorMouse->m_MousePose[0] == 0  && 
  compositorMouse->m_MousePose[1] == 0);

  CPPUNIT_ASSERT(compositorMouse->m_LastMousePose[0] == 0  && 
  compositorMouse->m_LastMousePose[1] == 0);
  
  dummyMouse->Delete();
  compositorMouse->Delete(); 
}

void albaInteractorCompositorMouseTest::TestCreateBehavior()
{
  albaInteractorCompositorMouse *compositorMouse = NULL;
  compositorMouse = albaInteractorCompositorMouse::New();
  CPPUNIT_ASSERT(compositorMouse != NULL);

  albaInteractorGenericMouse *mouseBehavior = NULL;
  mouseBehavior = compositorMouse->GetBehavior(MOUSE_LEFT);
  CPPUNIT_ASSERT(mouseBehavior == NULL);
    
  mouseBehavior = compositorMouse->CreateBehavior(MOUSE_LEFT);
  CPPUNIT_ASSERT(mouseBehavior != NULL);

  mouseBehavior = compositorMouse->GetBehavior(MOUSE_LEFT);
  CPPUNIT_ASSERT(mouseBehavior != NULL);

  compositorMouse->Delete(); 
}

void albaInteractorCompositorMouseTest::TestGetBehavior()
{
  albaInteractorCompositorMouse *compositorMouse = NULL;
  compositorMouse = albaInteractorCompositorMouse::New();
  CPPUNIT_ASSERT(compositorMouse != NULL);

  albaInteractorGenericMouse *getNotYetCreatedMouseBehavior = NULL;
  getNotYetCreatedMouseBehavior = compositorMouse->GetBehavior(MOUSE_MIDDLE);
  CPPUNIT_ASSERT(getNotYetCreatedMouseBehavior == NULL);

  albaInteractorGenericMouse *createdMouseMiddleBehavior = NULL;
  createdMouseMiddleBehavior = compositorMouse->CreateBehavior(MOUSE_MIDDLE);
  CPPUNIT_ASSERT(createdMouseMiddleBehavior != NULL);
  
  albaInteractorGenericMouse *getMouseMiddleBehavior = NULL;
  getMouseMiddleBehavior = compositorMouse->GetBehavior(MOUSE_MIDDLE);
  CPPUNIT_ASSERT(getMouseMiddleBehavior != NULL);
  CPPUNIT_ASSERT(getMouseMiddleBehavior == createdMouseMiddleBehavior);
  
  albaInteractorGenericMouse *getMouseMiddleCtrlBehavior = NULL;
  getMouseMiddleCtrlBehavior = compositorMouse->GetBehavior(MOUSE_MIDDLE_CONTROL);
  CPPUNIT_ASSERT(getMouseMiddleCtrlBehavior == NULL);

  compositorMouse->Delete(); 
}
