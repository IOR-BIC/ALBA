/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafInteractorCompositorMouseTest.cpp,v $
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
#include "mafInteractorCompositorMouseTest.h"
#include "mafInteractorCompositorMouse.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafEventInteraction.h"

void mafInteractorCompositorMouseTest::setUp()
{

}

void mafInteractorCompositorMouseTest::tearDown()
{

}

void mafInteractorCompositorMouseTest::TestFixture()
{

}

void mafInteractorCompositorMouseTest::TestConstructorDestructor()
{
  mafInteractorCompositorMouse *compositorMouse = NULL;
  compositorMouse = mafInteractorCompositorMouse::New();
  CPPUNIT_ASSERT(compositorMouse != NULL);
  compositorMouse->Delete(); 
}

void mafInteractorCompositorMouseTest::TestStartInteraction()
{
  mafInteractorCompositorMouse *compositorMouse = mafInteractorCompositorMouse::New();
  CPPUNIT_ASSERT(compositorMouse != NULL);
  
  mafDeviceButtonsPadMouse *dummyMouse = mafDeviceButtonsPadMouse::New();
  CPPUNIT_ASSERT(dummyMouse->GetRenderer() == NULL);

  // cannot start interaction since there is no renderer: should return false
  int returnValue = compositorMouse->StartInteraction(dummyMouse);
  CPPUNIT_ASSERT(returnValue == false);

  compositorMouse->Delete(); 
  dummyMouse->Delete();
}

void mafInteractorCompositorMouseTest::TestOnEvent()
{
  mafInteractorCompositorMouse *compositorMouse = NULL;
  compositorMouse = mafInteractorCompositorMouse::New();
  CPPUNIT_ASSERT(compositorMouse != NULL);
  
  CPPUNIT_ASSERT(compositorMouse->m_MousePose[0] == 0  && 
  compositorMouse->m_MousePose[1] == 0);

  CPPUNIT_ASSERT(compositorMouse->m_LastMousePose[0] == 0  && 
  compositorMouse->m_LastMousePose[1] == 0);

  CPPUNIT_ASSERT(compositorMouse->m_InteractionFlag == false);

  // trying to create a fake mouse device to fool the compositor OnEvent...
  compositorMouse->m_InteractionFlag = true;
  
  mafDeviceButtonsPadMouse *dummyMouse = mafDeviceButtonsPadMouse::New();
  CPPUNIT_ASSERT(dummyMouse !=  NULL);
  compositorMouse->m_Device = dummyMouse;

  mafEventInteraction dummyMouse2DMoveEvent;
  dummyMouse2DMoveEvent.SetChannel(MCH_INPUT);
  dummyMouse2DMoveEvent.SetSender(this);
  dummyMouse2DMoveEvent.SetId(mafDeviceButtonsPadMouse::GetMouse2DMoveId());

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

void mafInteractorCompositorMouseTest::TestCreateBehavior()
{
  mafInteractorCompositorMouse *compositorMouse = NULL;
  compositorMouse = mafInteractorCompositorMouse::New();
  CPPUNIT_ASSERT(compositorMouse != NULL);

  mafInteractorGenericMouse *mouseBehavior = NULL;
  mouseBehavior = compositorMouse->GetBehavior(MOUSE_LEFT);
  CPPUNIT_ASSERT(mouseBehavior == NULL);
    
  mouseBehavior = compositorMouse->CreateBehavior(MOUSE_LEFT);
  CPPUNIT_ASSERT(mouseBehavior != NULL);

  mouseBehavior = compositorMouse->GetBehavior(MOUSE_LEFT);
  CPPUNIT_ASSERT(mouseBehavior != NULL);

  compositorMouse->Delete(); 
}

void mafInteractorCompositorMouseTest::TestGetBehavior()
{
  mafInteractorCompositorMouse *compositorMouse = NULL;
  compositorMouse = mafInteractorCompositorMouse::New();
  CPPUNIT_ASSERT(compositorMouse != NULL);

  mafInteractorGenericMouse *getNotYetCreatedMouseBehavior = NULL;
  getNotYetCreatedMouseBehavior = compositorMouse->GetBehavior(MOUSE_MIDDLE);
  CPPUNIT_ASSERT(getNotYetCreatedMouseBehavior == NULL);

  mafInteractorGenericMouse *createdMouseMiddleBehavior = NULL;
  createdMouseMiddleBehavior = compositorMouse->CreateBehavior(MOUSE_MIDDLE);
  CPPUNIT_ASSERT(createdMouseMiddleBehavior != NULL);
  
  mafInteractorGenericMouse *getMouseMiddleBehavior = NULL;
  getMouseMiddleBehavior = compositorMouse->GetBehavior(MOUSE_MIDDLE);
  CPPUNIT_ASSERT(getMouseMiddleBehavior != NULL);
  CPPUNIT_ASSERT(getMouseMiddleBehavior == createdMouseMiddleBehavior);
  
  mafInteractorGenericMouse *getMouseMiddleCtrlBehavior = NULL;
  getMouseMiddleCtrlBehavior = compositorMouse->GetBehavior(MOUSE_MIDDLE_CONTROL);
  CPPUNIT_ASSERT(getMouseMiddleCtrlBehavior == NULL);

  compositorMouse->Delete(); 
}
