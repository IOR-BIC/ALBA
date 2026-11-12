/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaStaticEventRouterTest
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
#include "albaStaticEventRouterTest.h"
#include "albaStaticEventRouter.h"
#include "albaSmartPointer.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaAction.h"

//----------------------------------------------------------------------------
void albaStaticEventRouterTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaStaticEventRouterTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  albaStaticEventRouter *interactor = albaStaticEventRouter::New();
  interactor->Delete();

  albaSmartPointer<albaStaticEventRouter> interactorSmartPointer;
}

void albaStaticEventRouterTest::TestBindDeviceToAction()
{
  albaStaticEventRouter *interactor = albaStaticEventRouter::New();

  albaDeviceButtonsPadMouse *dummyDevice = albaDeviceButtonsPadMouse::New();
  albaAction *dummyAction = albaAction::New();
  dummyAction->SetName("dummyAction");

  std::vector<albaAction *> actions;
  interactor->GetDeviceBindings(dummyDevice, actions);
  CPPUNIT_ASSERT(actions.size() == 0);

  interactor->BindDeviceToAction(dummyDevice , dummyAction);

  interactor->GetDeviceBindings(dummyDevice, actions);

  CPPUNIT_ASSERT(actions.size() == 1);

  dummyAction->Delete();
  
  // deleting the action will delete the bound device (?)
  // dummyDevice->Delete();
  
  interactor->Delete();
}

void albaStaticEventRouterTest::TestUnBindDeviceFromAction()
{
  albaStaticEventRouter *interactor = albaStaticEventRouter::New();

  albaDeviceButtonsPadMouse *dummyDevice = albaDeviceButtonsPadMouse::New();
  albaAction *dummyAction = albaAction::New();
  dummyAction->SetName("dummyAction");

  std::vector<albaAction *> actions;
  interactor->GetDeviceBindings(dummyDevice, actions);
  CPPUNIT_ASSERT(actions.size() == 0);

  interactor->BindDeviceToAction(dummyDevice , dummyAction);
  interactor->GetDeviceBindings(dummyDevice, actions);
  CPPUNIT_ASSERT(actions.size() == 1);

  interactor->UnBindDeviceFromAction(dummyDevice, dummyAction);
  interactor->GetDeviceBindings(dummyDevice, actions);
  CPPUNIT_ASSERT(actions.size() == 0);

  dummyAction->Delete();

  // deleting the action will delete the bound device (?)
  // dummyDevice->Delete();

  interactor->Delete();
}

void albaStaticEventRouterTest::TestUnBindDeviceFromAllActions()
{
  albaStaticEventRouter *interactor = albaStaticEventRouter::New();

  albaDeviceButtonsPadMouse *dummyDevice = albaDeviceButtonsPadMouse::New();
  albaAction *dummyAction1 = albaAction::New();
  dummyAction1->SetName("dummyAction1");

  albaAction *dummyAction2 = albaAction::New();
  dummyAction2->SetName("dummyAction2");

  std::vector<albaAction *> actions;
  interactor->GetDeviceBindings(dummyDevice, actions);
  CPPUNIT_ASSERT(actions.size() == 0);

  interactor->BindDeviceToAction(dummyDevice , dummyAction1);
  interactor->BindDeviceToAction(dummyDevice , dummyAction2);

  interactor->GetDeviceBindings(dummyDevice, actions);
  CPPUNIT_ASSERT(actions.size() == 2);

  interactor->UnBindDeviceFromAllActions(dummyDevice);
  interactor->GetDeviceBindings(dummyDevice, actions);
  CPPUNIT_ASSERT(actions.size() == 0);

  dummyAction1->Delete();
  dummyAction2->Delete();

  // deleting the action will delete the bound device (?)
  // dummyDevice->Delete();

  interactor->Delete();
}

void albaStaticEventRouterTest::TestGetDeviceBindings()
{
  albaStaticEventRouter *interactor = albaStaticEventRouter::New();

  albaDeviceButtonsPadMouse *dummyDevice = albaDeviceButtonsPadMouse::New();
  albaAction *dummyAction = albaAction::New();
  dummyAction->SetName("dummyAction");

  std::vector<albaAction *> actions;
  
  interactor->GetDeviceBindings(dummyDevice, actions);

  CPPUNIT_ASSERT(actions.size() == 0);

  interactor->BindDeviceToAction(dummyDevice , dummyAction);
  
  interactor->GetDeviceBindings(dummyDevice, actions);

  CPPUNIT_ASSERT(actions.size() == 1);

  dummyAction->Delete();

  // deleting the action will delete the bound device (?)
  // dummyDevice->Delete();

  interactor->Delete();
}


void albaStaticEventRouterTest::TestAddAction()
{
  albaStaticEventRouter *interactor = albaStaticEventRouter::New();

  albaDeviceButtonsPadMouse *dummyDevice = albaDeviceButtonsPadMouse::New();
  albaAction *dummyAction = albaAction::New();
  dummyAction->SetName("dummyAction");

  CPPUNIT_ASSERT(interactor->GetActions()->size() == 0);

  interactor->AddAction(dummyAction);

  CPPUNIT_ASSERT(interactor->GetActions()->size() == 1);
  dummyDevice->Delete();

  interactor->Delete();

  // deleting the interactor will delete the action (?)
  // dummyAction->Delete();
}


void albaStaticEventRouterTest::TestGetAction()
{
  albaStaticEventRouter *interactor = albaStaticEventRouter::New();

  albaDeviceButtonsPadMouse *dummyDevice = albaDeviceButtonsPadMouse::New();
  albaAction *dummyAction = albaAction::New();
  dummyAction->SetName("dummyAction");

  interactor->AddAction(dummyAction);

  albaAction *getAction = NULL;

  getAction = interactor->GetAction("dummyAction");
  CPPUNIT_ASSERT(getAction != NULL);
  CPPUNIT_ASSERT(albaString(getAction->GetName()).Equals("dummyAction"));

  dummyDevice->Delete();

  interactor->Delete();

  // deleting the interactor will delete the action (?)
  // dummyAction->Delete();
}

void albaStaticEventRouterTest::TestGetActions()
{
  albaStaticEventRouter *interactor = albaStaticEventRouter::New();

  albaDeviceButtonsPadMouse *dummyDevice = albaDeviceButtonsPadMouse::New();
  albaAction *dummyAction1 = albaAction::New();
  dummyAction1->SetName("dummyAction1");

  albaAction *dummyAction2 = albaAction::New();
  dummyAction2->SetName("dummyAction2");

  albaAction *dummyAction3 = albaAction::New();
  dummyAction3->SetName("dummyAction3");

  interactor->AddAction(dummyAction1);
  interactor->AddAction(dummyAction2);
  interactor->AddAction(dummyAction3);

  CPPUNIT_ASSERT(interactor->GetActions()->size() == 3);
  dummyDevice->Delete();

  interactor->Delete();

  // deleting the interactor will delete all actions (?)
  // dummyAction1->Delete();
  // dummyAction2->Delete();
  // dummyAction3->Delete();

}
