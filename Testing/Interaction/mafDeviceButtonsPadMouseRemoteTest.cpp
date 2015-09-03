/*=========================================================================

 Program: MAF2
 Module: mafDeviceButtonsPadMouseRemoteTest
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
#include "mafDeviceButtonsPadMouseRemoteTest.h"
#include "mafDeviceButtonsPadMouseRemote.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafEventInteraction.h"
#include "mafEventBase.h"
#include "mafDecl.h"

//----------------------------------------------------------------------------
void mafDeviceButtonsPadMouseRemoteTest::TestFixture()
{

}

//----------------------------------------------------------------------------
void mafDeviceButtonsPadMouseRemoteTest::TestConstructorDestructor()
{
  mafDeviceButtonsPadMouseRemote *buttonsPadMouse = mafDeviceButtonsPadMouseRemote::New();
  CPPUNIT_ASSERT(buttonsPadMouse);
  buttonsPadMouse->Delete();
}

//----------------------------------------------------------------------------
void mafDeviceButtonsPadMouseRemoteTest::TestOnEventMouse2DMove()
{
  mafDeviceButtonsPadMouseRemote *buttonsPadMouse = mafDeviceButtonsPadMouseRemote::New();

  CPPUNIT_ASSERT(buttonsPadMouse);

  // dummy event sending in order to test for leaks and/or crashes
  mafEventInteraction mouse2DMoveEvent(this,mafDeviceButtonsPadMouse::GetMouse2DMoveId(),10,20);
  buttonsPadMouse->OnEvent(&mouse2DMoveEvent);
  
  buttonsPadMouse->Delete();
}

//----------------------------------------------------------------------------
void mafDeviceButtonsPadMouseRemoteTest::TestOnEventButtonDown()
{
  mafDeviceButtonsPadMouseRemote *buttonsPadMouse = mafDeviceButtonsPadMouseRemote::New();
  
  CPPUNIT_ASSERT(buttonsPadMouse);

  // dummy event sending in order to test for leaks and/or crashes
  
  mafEventInteraction buttonDownEvent(this,mafDeviceButtonsPadMouse::GetButtonDownId());
  buttonsPadMouse->OnEvent(&buttonDownEvent);

  buttonsPadMouse->Delete();  
}

//----------------------------------------------------------------------------
void mafDeviceButtonsPadMouseRemoteTest::TestOnEventMouseDClick()
{
  mafDeviceButtonsPadMouseRemote *buttonsPadMouse = mafDeviceButtonsPadMouseRemote::New();
  
  CPPUNIT_ASSERT(buttonsPadMouse);

  // dummy event sending in order to test for leaks and/or crashes
  mafEventInteraction mouseDClickEvent(this,mafDeviceButtonsPadMouse::GetMouseDClickId());
  buttonsPadMouse->OnEvent(&mouseDClickEvent);

  buttonsPadMouse->Delete();

}

//----------------------------------------------------------------------------
void mafDeviceButtonsPadMouseRemoteTest::TestOnEventButtonUp()
{
  mafDeviceButtonsPadMouseRemote *buttonsPadMouse = mafDeviceButtonsPadMouseRemote::New();

  CPPUNIT_ASSERT(buttonsPadMouse);

  // dummy event sending in order to test for leaks and/or crashes
  mafEventInteraction buttonUpEvent(this,mafDeviceButtonsPadMouse::GetButtonUpId());
  buttonsPadMouse->OnEvent(&buttonUpEvent);

  buttonsPadMouse->Delete();

}

//----------------------------------------------------------------------------
void mafDeviceButtonsPadMouseRemoteTest::TestOnEventViewSelect()
{
  mafDeviceButtonsPadMouseRemote *buttonsPadMouse = mafDeviceButtonsPadMouseRemote::New();
  
  CPPUNIT_ASSERT(buttonsPadMouse);

  // sending some dummy events and testing for leaks

  mafEventInteraction viewSelectEvent(this,VIEW_SELECT);
  buttonsPadMouse->OnEvent(&viewSelectEvent);

  buttonsPadMouse->Delete();
}

//----------------------------------------------------------------------------
void mafDeviceButtonsPadMouseRemoteTest::TestOnEventViewDelete()
{
  mafDeviceButtonsPadMouseRemote *buttonsPadMouse = mafDeviceButtonsPadMouseRemote::New();
  
  CPPUNIT_ASSERT(buttonsPadMouse);

  // sending some dummy events and testing for leaks

  mafEventInteraction viewDeleteEvent(this,VIEW_DELETE);
  buttonsPadMouse->OnEvent(&viewDeleteEvent);

  buttonsPadMouse->Delete();
}

//----------------------------------------------------------------------------
void mafDeviceButtonsPadMouseRemoteTest::TestOnEventMouseCharEvent()
{
  mafDeviceButtonsPadMouseRemote *buttonsPadMouse = mafDeviceButtonsPadMouseRemote::New();
  
  CPPUNIT_ASSERT(buttonsPadMouse);

  // dummy event sending in order to test for leaks and/or crashes
  mafEventInteraction mouseCharEvent(this,mafDeviceButtonsPadMouse::GetMouseCharEventId());
  buttonsPadMouse->OnEvent(&mouseCharEvent);
  
  buttonsPadMouse->Delete();
}

