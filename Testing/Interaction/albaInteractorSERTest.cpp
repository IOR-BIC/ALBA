/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorSERTest
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
#include "albaInteractorSERTest.h"
#include "albaInteractorSER.h"
#include "albaSmartPointer.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaAction.h"

//----------------------------------------------------------------------------
void albaInteractorSERTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaInteractorSERTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  albaInteractorSER *interactor = albaInteractorSER::New();
  interactor->Delete();

  albaSmartPointer<albaInteractorSER> interactorSmartPointer;
}

void albaInteractorSERTest::TestBindDeviceToAction()
{
  albaInteractorSER *interactor = albaInteractorSER::New();

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

void albaInteractorSERTest::TestUnBindDeviceFromAction()
{
  albaInteractorSER *interactor = albaInteractorSER::New();

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

void albaInteractorSERTest::TestUnBindDeviceFromAllActions()
{
  albaInteractorSER *interactor = albaInteractorSER::New();

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

void albaInteractorSERTest::TestGetDeviceBindings()
{
  albaInteractorSER *interactor = albaInteractorSER::New();

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

void albaInteractorSERTest::TestBindAction()
{
  albaInteractorSER *interactor = albaInteractorSER::New();

  albaDeviceButtonsPadMouse *dummyDevice = albaDeviceButtonsPadMouse::New();
  albaAction *dummyAction = albaAction::New();
  dummyAction->SetName("dummyAction");

  interactor->AddAction(dummyAction);
  interactor->BindAction("dummyAction", interactor);

  albaAction *getAction = NULL;

  getAction =interactor->GetAction("dummyAction");
  CPPUNIT_ASSERT(getAction != NULL);
  CPPUNIT_ASSERT(albaString(getAction->GetName()).Equals("dummyAction"));
  
  dummyDevice->Delete();

  interactor->Delete();

  // deleting the interactor will delete the action (?)
  // dummyAction->Delete();
}

void albaInteractorSERTest::TestUnBindAction()
{
  albaInteractorSER *interactor = albaInteractorSER::New();

  albaDeviceButtonsPadMouse *dummyDevice = albaDeviceButtonsPadMouse::New();
  albaAction *dummyAction = albaAction::New();
  dummyAction->SetName("dummyAction");

  std::vector<albaAction *> bindings;

  interactor->AddAction(dummyAction);
  interactor->GetDeviceBindings(dummyDevice, bindings);
  CPPUNIT_ASSERT(bindings.size() == 0);
    
  interactor->BindAction("dummyAction", interactor);
  dummyAction->BindDevice(dummyDevice);

  interactor->GetDeviceBindings(dummyDevice, bindings);

  CPPUNIT_ASSERT(bindings.size() == 1);

  interactor->UnBindAction("dummyAction", interactor);
  interactor->GetDeviceBindings(dummyDevice, bindings);
  CPPUNIT_ASSERT(bindings.size() == 1);

  interactor->Delete();

  // deleting the interactor will delete the action and the device (?)
  // dummyAction->Delete();
  // dummyDevice->Delete();

}

void albaInteractorSERTest::TestAddAction()
{
  albaInteractorSER *interactor = albaInteractorSER::New();

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


void albaInteractorSERTest::TestGetAction()
{
  albaInteractorSER *interactor = albaInteractorSER::New();

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

void albaInteractorSERTest::TestGetActions()
{
  albaInteractorSER *interactor = albaInteractorSER::New();

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
