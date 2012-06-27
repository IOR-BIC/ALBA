/*=========================================================================

 Program: MAF2
 Module: mafInteractorTest
 Authors: Alberto Losi
 
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
#include "mafInteractorTest.h"
#include "mafObject.h"
#include "mafInteractor.h"
#include "vtkRenderer.h"
#include "mafDevice.h"
#include "mafEventInteraction.h"

enum EVENTS_ID
{
  START_INTERACTION_EVENT = 0,
  STOP_INTERACTION_EVENT,
};
enum MODALITIES_ID
{
  SINGLE_BUTTON_MODE=0,
  MULTI_BUTTON_MODE,
};
//----------------------------------------------------------------------------
class mafInteractorDummy : public mafInteractor // concrete class for test
//----------------------------------------------------------------------------
{
public:
  mafInteractorDummy(){Superclass;};
  ~mafInteractorDummy(){Superclass;};

  void OnButtonDown(mafEventInteraction *e){};
  void OnButtonUp(mafEventInteraction *e){};

  mafTypeMacro(mafInteractorDummy,mafInteractor);
};

mafCxxTypeMacro(mafInteractorDummy);

//----------------------------------------------------------------------------
void mafInteractorTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafInteractorTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafInteractorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafInteractorTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  mafInteractorDummy *interactor = mafInteractorDummy::New();

  cppDEL(interactor);
}
//----------------------------------------------------------------------------
void mafInteractorTest::TestSetGetRenderer()
//----------------------------------------------------------------------------
{
  vtkRenderer *renderer;
  vtkNEW(renderer);

  mafInteractorDummy *interactor;
  mafNEW(interactor);

  interactor->SetRenderer(renderer);
  CPPUNIT_ASSERT(interactor->GetRenderer() == renderer);

  vtkDEL(renderer);
  mafDEL(interactor);

}
//----------------------------------------------------------------------------
void mafInteractorTest::TestSetGetDevice()
//----------------------------------------------------------------------------
{
  mafDevice *device;
  mafNEW(device);

  mafInteractorDummy *interactor;
  mafNEW(interactor);

  interactor->SetDevice(device);
  CPPUNIT_ASSERT(interactor->GetDevice() == device);

  mafDEL(device);
  mafDEL(interactor);
}
//----------------------------------------------------------------------------
void mafInteractorTest::TestSetGetIgnoreTriggerEvents()
//----------------------------------------------------------------------------
{
  mafInteractorDummy *interactor;
  mafNEW(interactor);

  interactor->SetIgnoreTriggerEvents(true);
  CPPUNIT_ASSERT(interactor->GetIgnoreTriggerEvents(false) == 1);

  interactor->SetIgnoreTriggerEvents(false);
  CPPUNIT_ASSERT(interactor->GetIgnoreTriggerEvents(false) == 0);

  interactor->IgnoreTriggerEventsOn();
  CPPUNIT_ASSERT(interactor->GetIgnoreTriggerEvents(false) == 1);

  interactor->IgnoreTriggerEventsOff();
  CPPUNIT_ASSERT(interactor->GetIgnoreTriggerEvents(false) == 0);

  mafDEL(interactor);
}
//----------------------------------------------------------------------------
void mafInteractorTest::TestSetGetStartInteractionEvent()
//----------------------------------------------------------------------------
{
  mafInteractorDummy *interactor;
  mafNEW(interactor);

  mafID dummyInteractionEvent = START_INTERACTION_EVENT;
  interactor->SetStartInteractionEvent(dummyInteractionEvent);

  CPPUNIT_ASSERT(interactor->GetStartInteractionEvent() == dummyInteractionEvent);

  mafDEL(interactor);
}
//----------------------------------------------------------------------------
void mafInteractorTest::TestSetGetStopInteractionEvent()
//----------------------------------------------------------------------------
{
  mafInteractorDummy *interactor;
  mafNEW(interactor);

  mafID dummyInteractionEvent = STOP_INTERACTION_EVENT;
  interactor->SetStopInteractionEvent(dummyInteractionEvent);

  CPPUNIT_ASSERT(interactor->GetStopInteractionEvent() == dummyInteractionEvent);

  mafDEL(interactor);
}
//----------------------------------------------------------------------------
void mafInteractorTest::TestSetGetStartButton()
//----------------------------------------------------------------------------
{
  mafInteractorDummy *interactor;
  mafNEW(interactor);

  int button = 0;
  interactor->SetStartButton(button);

  CPPUNIT_ASSERT(interactor->GetStartButton() == button);

  mafDEL(interactor);
}
//----------------------------------------------------------------------------
void mafInteractorTest::TestSetGetModifiers()
//----------------------------------------------------------------------------
{
  mafInteractorDummy *interactor;
  mafNEW(interactor);

  int modifiers = 1;
  interactor->SetModifiers(modifiers);

  CPPUNIT_ASSERT(interactor->GetModifiers() == modifiers);

  mafDEL(interactor);
}
//----------------------------------------------------------------------------
void mafInteractorTest::TestGetCurrentButton()
//----------------------------------------------------------------------------
{
  mafInteractorDummy *interactor;
  mafNEW(interactor);

  mafDevice *device;
  mafNEW(device);

  interactor->SetDevice(device);

  // setup the interactor
  interactor->SetStartInteractionEvent(START_INTERACTION_EVENT);
  interactor->SetStopInteractionEvent(STOP_INTERACTION_EVENT);
  interactor->IgnoreTriggerEventsOff();
  int button = 1;
  interactor->SetStartButton(button);
  interactor->SetModifiers(0);

  mafEventInteraction e;
  e.SetSender(device);
  e.SetId(START_INTERACTION_EVENT);
  e.SetButton(button);
  e.SetChannel(MCH_INPUT);
  interactor->OnEvent(&e);

  CPPUNIT_ASSERT(interactor->GetCurrentButton() == button);

  mafDEL(interactor);
  mafDEL(device);
}
//----------------------------------------------------------------------------
void mafInteractorTest::TestGetCurrentModifier()
//----------------------------------------------------------------------------
{
  mafInteractorDummy *interactor;
  mafNEW(interactor);

  mafDevice *device;
  mafNEW(device);

  interactor->SetDevice(device);

  // setup the interactor
  interactor->SetStartInteractionEvent(START_INTERACTION_EVENT);
  interactor->SetStopInteractionEvent(STOP_INTERACTION_EVENT);
  interactor->IgnoreTriggerEventsOff();
  long modifier = 1;
  int button = 0;
  interactor->SetStartButton(button);
  interactor->SetModifiers(0);

  mafEventInteraction e;
  e.SetSender(device);
  e.SetId(START_INTERACTION_EVENT);
  e.SetModifiers(modifier);
  e.SetButton(button);
  e.SetChannel(MCH_INPUT);
  interactor->OnEvent(&e);

  CPPUNIT_ASSERT(interactor->GetCurrentModifier() == modifier);

  mafDEL(interactor);
  mafDEL(device);
}
//----------------------------------------------------------------------------
void mafInteractorTest::TestStartStopInteraction()
//----------------------------------------------------------------------------
{
  mafInteractorDummy *interactor;
  mafNEW(interactor);

  mafDevice *device;
  mafNEW(device);

  interactor->SetDevice(device);

  CPPUNIT_ASSERT(interactor->StartInteraction(device) == true);
  CPPUNIT_ASSERT(interactor->IsInteracting(device) == true);
  CPPUNIT_ASSERT(interactor->StartInteraction(device) == false);
  CPPUNIT_ASSERT(interactor->StopInteraction(device) == true);
  // CPPUNIT_ASSERT(interactor->StopInteraction(device) == false); raise a warning

  mafDEL(device);

  mafDEL(interactor);
}
//----------------------------------------------------------------------------
void mafInteractorTest::TestIsInteracting()
//----------------------------------------------------------------------------
{
  mafInteractorDummy *interactor;
  mafNEW(interactor);

  mafDevice *device;
  mafNEW(device);

  mafDevice *deviceDummy;
  mafNEW(deviceDummy);


  interactor->SetDevice(device);

  CPPUNIT_ASSERT(interactor->IsInteracting(device) == false);
  CPPUNIT_ASSERT(interactor->IsInteracting(deviceDummy) == false);

  interactor->StartInteraction(device) == true;

  CPPUNIT_ASSERT(interactor->IsInteracting(device) == true);
  CPPUNIT_ASSERT(interactor->IsInteracting(deviceDummy) == false);

  interactor->StopInteraction(device) == true;

  CPPUNIT_ASSERT(interactor->IsInteracting(device) == false);
  CPPUNIT_ASSERT(interactor->IsInteracting(deviceDummy) == false);

  mafDEL(device);

  mafDEL(deviceDummy);

  mafDEL(interactor);
}
//----------------------------------------------------------------------------
void mafInteractorTest::TestSetGetButtonMode()
//----------------------------------------------------------------------------
{
  mafInteractorDummy *interactor;
  mafNEW(interactor);

  interactor->SetButtonMode(SINGLE_BUTTON_MODE);

  CPPUNIT_ASSERT(interactor->GetButtonMode() == SINGLE_BUTTON_MODE);

  interactor->SetButtonMode(MULTI_BUTTON_MODE);

  CPPUNIT_ASSERT(interactor->GetButtonMode() == MULTI_BUTTON_MODE);

  interactor->SetButtonModeToSingleButton();

  CPPUNIT_ASSERT(interactor->GetButtonMode() == SINGLE_BUTTON_MODE);

  interactor->SetButtonModeToMultiButton();

  CPPUNIT_ASSERT(interactor->GetButtonMode() == MULTI_BUTTON_MODE);

  mafDEL(interactor);
}
//----------------------------------------------------------------------------
void mafInteractorTest::TestSetGetLockDevice()
//----------------------------------------------------------------------------
{
  mafInteractorDummy *interactor;
  mafNEW(interactor);

  interactor->SetLockDevice(true);

  CPPUNIT_ASSERT(interactor->GetLockDevice() == true);

  interactor->SetLockDevice(false);

  CPPUNIT_ASSERT(interactor->GetLockDevice() == false);

  mafDEL(interactor);
}