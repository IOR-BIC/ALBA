/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorTest
 Authors: Alberto Losi
 
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
#include "albaInteractorTest.h"
#include "albaObject.h"
#include "albaInteractor.h"
#include "vtkRenderer.h"
#include "albaDevice.h"
#include "albaEventInteraction.h"

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
class albaInteractorDummy : public albaInteractor // concrete class for test
//----------------------------------------------------------------------------
{
public:
  albaInteractorDummy(){Superclass;};
  ~albaInteractorDummy(){Superclass;};

  void OnButtonDown(albaEventInteraction *e){};
  void OnButtonUp(albaEventInteraction *e){};

  albaTypeMacro(albaInteractorDummy,albaInteractor);
};

albaCxxTypeMacro(albaInteractorDummy);

//----------------------------------------------------------------------------
void albaInteractorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaInteractorTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  albaInteractorDummy *interactor = albaInteractorDummy::New();

  cppDEL(interactor);
}
//----------------------------------------------------------------------------
void albaInteractorTest::TestSetGetRenderer()
//----------------------------------------------------------------------------
{
  vtkRenderer *renderer;
  vtkNEW(renderer);

  albaInteractorDummy *interactor;
  albaNEW(interactor);

  interactor->SetRendererAndView(renderer,NULL);
  CPPUNIT_ASSERT(interactor->GetRenderer() == renderer);

  vtkDEL(renderer);
  albaDEL(interactor);

}
//----------------------------------------------------------------------------
void albaInteractorTest::TestSetGetDevice()
//----------------------------------------------------------------------------
{
  albaDevice *device;
  albaNEW(device);

  albaInteractorDummy *interactor;
  albaNEW(interactor);

  interactor->SetDevice(device);
  CPPUNIT_ASSERT(interactor->GetDevice() == device);

  albaDEL(device);
  albaDEL(interactor);
}
//----------------------------------------------------------------------------
void albaInteractorTest::TestSetGetIgnoreTriggerEvents()
//----------------------------------------------------------------------------
{
  albaInteractorDummy *interactor;
  albaNEW(interactor);

  interactor->SetIgnoreTriggerEvents(true);
  CPPUNIT_ASSERT(interactor->GetIgnoreTriggerEvents(false) == 1);

  interactor->SetIgnoreTriggerEvents(false);
  CPPUNIT_ASSERT(interactor->GetIgnoreTriggerEvents(false) == 0);

  interactor->IgnoreTriggerEventsOn();
  CPPUNIT_ASSERT(interactor->GetIgnoreTriggerEvents(false) == 1);

  interactor->IgnoreTriggerEventsOff();
  CPPUNIT_ASSERT(interactor->GetIgnoreTriggerEvents(false) == 0);

  albaDEL(interactor);
}
//----------------------------------------------------------------------------
void albaInteractorTest::TestSetGetStartInteractionEvent()
//----------------------------------------------------------------------------
{
  albaInteractorDummy *interactor;
  albaNEW(interactor);

  albaID dummyInteractionEvent = START_INTERACTION_EVENT;
  interactor->SetStartInteractionEvent(dummyInteractionEvent);

  CPPUNIT_ASSERT(interactor->GetStartInteractionEvent() == dummyInteractionEvent);

  albaDEL(interactor);
}
//----------------------------------------------------------------------------
void albaInteractorTest::TestSetGetStopInteractionEvent()
//----------------------------------------------------------------------------
{
  albaInteractorDummy *interactor;
  albaNEW(interactor);

  albaID dummyInteractionEvent = STOP_INTERACTION_EVENT;
  interactor->SetStopInteractionEvent(dummyInteractionEvent);

  CPPUNIT_ASSERT(interactor->GetStopInteractionEvent() == dummyInteractionEvent);

  albaDEL(interactor);
}
//----------------------------------------------------------------------------
void albaInteractorTest::TestSetGetStartButton()
//----------------------------------------------------------------------------
{
  albaInteractorDummy *interactor;
  albaNEW(interactor);

  int button = 0;
  interactor->SetStartButton(button);

  CPPUNIT_ASSERT(interactor->GetStartButton() == button);

  albaDEL(interactor);
}
//----------------------------------------------------------------------------
void albaInteractorTest::TestSetGetModifiers()
//----------------------------------------------------------------------------
{
  albaInteractorDummy *interactor;
  albaNEW(interactor);

  int modifiers = 1;
  interactor->SetModifiers(modifiers);

  CPPUNIT_ASSERT(interactor->GetModifiers() == modifiers);

  albaDEL(interactor);
}
//----------------------------------------------------------------------------
void albaInteractorTest::TestGetCurrentButton()
//----------------------------------------------------------------------------
{
  albaInteractorDummy *interactor;
  albaNEW(interactor);

  albaDevice *device;
  albaNEW(device);

  interactor->SetDevice(device);

  // setup the interactor
  interactor->SetStartInteractionEvent(START_INTERACTION_EVENT);
  interactor->SetStopInteractionEvent(STOP_INTERACTION_EVENT);
  interactor->IgnoreTriggerEventsOff();
  int button = 1;
  interactor->SetStartButton(button);
  interactor->SetModifiers(0);

  albaEventInteraction e;
  e.SetSender(device);
  e.SetId(START_INTERACTION_EVENT);
  e.SetButton(button);
  e.SetChannel(MCH_INPUT);
  interactor->OnEvent(&e);

  CPPUNIT_ASSERT(interactor->GetCurrentButton() == button);

  albaDEL(interactor);
  albaDEL(device);
}
//----------------------------------------------------------------------------
void albaInteractorTest::TestGetCurrentModifier()
//----------------------------------------------------------------------------
{
  albaInteractorDummy *interactor;
  albaNEW(interactor);

  albaDevice *device;
  albaNEW(device);

  interactor->SetDevice(device);

  // setup the interactor
  interactor->SetStartInteractionEvent(START_INTERACTION_EVENT);
  interactor->SetStopInteractionEvent(STOP_INTERACTION_EVENT);
  interactor->IgnoreTriggerEventsOff();
  long modifier = 1;
  int button = 0;
  interactor->SetStartButton(button);
  interactor->SetModifiers(0);

  albaEventInteraction e;
  e.SetSender(device);
  e.SetId(START_INTERACTION_EVENT);
  e.SetModifiers(modifier);
  e.SetButton(button);
  e.SetChannel(MCH_INPUT);
  interactor->OnEvent(&e);

  CPPUNIT_ASSERT(interactor->GetCurrentModifier() == modifier);

  albaDEL(interactor);
  albaDEL(device);
}
//----------------------------------------------------------------------------
void albaInteractorTest::TestStartStopInteraction()
//----------------------------------------------------------------------------
{
  albaInteractorDummy *interactor;
  albaNEW(interactor);

  albaDevice *device;
  albaNEW(device);

  interactor->SetDevice(device);

  CPPUNIT_ASSERT(interactor->StartInteraction(device) == true);
  CPPUNIT_ASSERT(interactor->IsInteracting(device) == true);
  CPPUNIT_ASSERT(interactor->StartInteraction(device) == false);
  CPPUNIT_ASSERT(interactor->StopInteraction(device) == true);
  // CPPUNIT_ASSERT(interactor->StopInteraction(device) == false); raise a warning

  albaDEL(device);

  albaDEL(interactor);
}
//----------------------------------------------------------------------------
void albaInteractorTest::TestIsInteracting()
//----------------------------------------------------------------------------
{
  albaInteractorDummy *interactor;
  albaNEW(interactor);

  albaDevice *device;
  albaNEW(device);

  albaDevice *deviceDummy;
  albaNEW(deviceDummy);


  interactor->SetDevice(device);

  CPPUNIT_ASSERT(interactor->IsInteracting(device) == false);
  CPPUNIT_ASSERT(interactor->IsInteracting(deviceDummy) == false);

  interactor->StartInteraction(device) == true;

  CPPUNIT_ASSERT(interactor->IsInteracting(device) == true);
  CPPUNIT_ASSERT(interactor->IsInteracting(deviceDummy) == false);

  interactor->StopInteraction(device) == true;

  CPPUNIT_ASSERT(interactor->IsInteracting(device) == false);
  CPPUNIT_ASSERT(interactor->IsInteracting(deviceDummy) == false);

  albaDEL(device);

  albaDEL(deviceDummy);

  albaDEL(interactor);
}
//----------------------------------------------------------------------------
void albaInteractorTest::TestSetGetButtonMode()
//----------------------------------------------------------------------------
{
  albaInteractorDummy *interactor;
  albaNEW(interactor);

  interactor->SetButtonMode(SINGLE_BUTTON_MODE);

  CPPUNIT_ASSERT(interactor->GetButtonMode() == SINGLE_BUTTON_MODE);

  interactor->SetButtonMode(MULTI_BUTTON_MODE);

  CPPUNIT_ASSERT(interactor->GetButtonMode() == MULTI_BUTTON_MODE);

  interactor->SetButtonModeToSingleButton();

  CPPUNIT_ASSERT(interactor->GetButtonMode() == SINGLE_BUTTON_MODE);

  interactor->SetButtonModeToMultiButton();

  CPPUNIT_ASSERT(interactor->GetButtonMode() == MULTI_BUTTON_MODE);

  albaDEL(interactor);
}
