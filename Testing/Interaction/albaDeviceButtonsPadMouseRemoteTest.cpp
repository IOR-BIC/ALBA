/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceButtonsPadMouseRemoteTest
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
#include "albaDeviceButtonsPadMouseRemoteTest.h"
#include "albaDeviceButtonsPadMouseRemote.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaEventInteraction.h"
#include "albaEventBase.h"
#include "albaDecl.h"

//----------------------------------------------------------------------------
void albaDeviceButtonsPadMouseRemoteTest::TestFixture()
{

}

//----------------------------------------------------------------------------
void albaDeviceButtonsPadMouseRemoteTest::TestConstructorDestructor()
{
  albaDeviceButtonsPadMouseRemote *buttonsPadMouse = albaDeviceButtonsPadMouseRemote::New();
  CPPUNIT_ASSERT(buttonsPadMouse);
  buttonsPadMouse->Delete();
}

//----------------------------------------------------------------------------
void albaDeviceButtonsPadMouseRemoteTest::TestOnEventMouse2DMove()
{
  albaDeviceButtonsPadMouseRemote *buttonsPadMouse = albaDeviceButtonsPadMouseRemote::New();

  CPPUNIT_ASSERT(buttonsPadMouse);

  // dummy event sending in order to test for leaks and/or crashes
  albaEventInteraction mouse2DMoveEvent(this,albaDeviceButtonsPadMouse::GetMouse2DMoveId(),10,20);
  buttonsPadMouse->OnEvent(&mouse2DMoveEvent);
  
  buttonsPadMouse->Delete();
}

//----------------------------------------------------------------------------
void albaDeviceButtonsPadMouseRemoteTest::TestOnEventButtonDown()
{
  albaDeviceButtonsPadMouseRemote *buttonsPadMouse = albaDeviceButtonsPadMouseRemote::New();
  
  CPPUNIT_ASSERT(buttonsPadMouse);

  // dummy event sending in order to test for leaks and/or crashes
  
  albaEventInteraction buttonDownEvent(this,albaDeviceButtonsPadMouse::GetButtonDownId());
  buttonsPadMouse->OnEvent(&buttonDownEvent);

  buttonsPadMouse->Delete();  
}

//----------------------------------------------------------------------------
void albaDeviceButtonsPadMouseRemoteTest::TestOnEventMouseDClick()
{
  albaDeviceButtonsPadMouseRemote *buttonsPadMouse = albaDeviceButtonsPadMouseRemote::New();
  
  CPPUNIT_ASSERT(buttonsPadMouse);

  // dummy event sending in order to test for leaks and/or crashes
  albaEventInteraction mouseDClickEvent(this,albaDeviceButtonsPadMouse::GetMouseDClickId());
  buttonsPadMouse->OnEvent(&mouseDClickEvent);

  buttonsPadMouse->Delete();

}

//----------------------------------------------------------------------------
void albaDeviceButtonsPadMouseRemoteTest::TestOnEventButtonUp()
{
  albaDeviceButtonsPadMouseRemote *buttonsPadMouse = albaDeviceButtonsPadMouseRemote::New();

  CPPUNIT_ASSERT(buttonsPadMouse);

  // dummy event sending in order to test for leaks and/or crashes
  albaEventInteraction buttonUpEvent(this,albaDeviceButtonsPadMouse::GetButtonUpId());
  buttonsPadMouse->OnEvent(&buttonUpEvent);

  buttonsPadMouse->Delete();

}

//----------------------------------------------------------------------------
void albaDeviceButtonsPadMouseRemoteTest::TestOnEventViewSelect()
{
  albaDeviceButtonsPadMouseRemote *buttonsPadMouse = albaDeviceButtonsPadMouseRemote::New();
  
  CPPUNIT_ASSERT(buttonsPadMouse);

  // sending some dummy events and testing for leaks

  albaEventInteraction viewSelectEvent(this,VIEW_SELECT);
  buttonsPadMouse->OnEvent(&viewSelectEvent);

  buttonsPadMouse->Delete();
}

//----------------------------------------------------------------------------
void albaDeviceButtonsPadMouseRemoteTest::TestOnEventViewDelete()
{
  albaDeviceButtonsPadMouseRemote *buttonsPadMouse = albaDeviceButtonsPadMouseRemote::New();
  
  CPPUNIT_ASSERT(buttonsPadMouse);

  // sending some dummy events and testing for leaks

  albaEventInteraction viewDeleteEvent(this,VIEW_DELETE);
  buttonsPadMouse->OnEvent(&viewDeleteEvent);

  buttonsPadMouse->Delete();
}

//----------------------------------------------------------------------------
void albaDeviceButtonsPadMouseRemoteTest::TestOnEventMouseCharEvent()
{
  albaDeviceButtonsPadMouseRemote *buttonsPadMouse = albaDeviceButtonsPadMouseRemote::New();
  
  CPPUNIT_ASSERT(buttonsPadMouse);

  // dummy event sending in order to test for leaks and/or crashes
  albaEventInteraction mouseCharEvent(this,albaDeviceButtonsPadMouse::GetMouseCharEventId());
  buttonsPadMouse->OnEvent(&mouseCharEvent);
  
  buttonsPadMouse->Delete();
}

