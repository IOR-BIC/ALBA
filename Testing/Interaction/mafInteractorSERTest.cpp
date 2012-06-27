/*=========================================================================

 Program: MAF2
 Module: mafInteractorSERTest
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
#include "mafInteractorSERTest.h"
#include "mafInteractorSER.h"
#include "mafSmartPointer.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafAction.h"

//----------------------------------------------------------------------------
void mafInteractorSERTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafInteractorSERTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafInteractorSERTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafInteractorSERTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  mafInteractorSER *interactor = mafInteractorSER::New();
  interactor->Delete();

  mafSmartPointer<mafInteractorSER> interactorSmartPointer;
}

void mafInteractorSERTest::TestBindDeviceToAction()
{
  mafInteractorSER *interactor = mafInteractorSER::New();

  mafDeviceButtonsPadMouse *dummyDevice = mafDeviceButtonsPadMouse::New();
  mafAction *dummyAction = mafAction::New();
  dummyAction->SetName("dummyAction");

  std::vector<mafAction *> actions;
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

void mafInteractorSERTest::TestUnBindDeviceFromAction()
{
  mafInteractorSER *interactor = mafInteractorSER::New();

  mafDeviceButtonsPadMouse *dummyDevice = mafDeviceButtonsPadMouse::New();
  mafAction *dummyAction = mafAction::New();
  dummyAction->SetName("dummyAction");

  std::vector<mafAction *> actions;
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

void mafInteractorSERTest::TestUnBindDeviceFromAllActions()
{
  mafInteractorSER *interactor = mafInteractorSER::New();

  mafDeviceButtonsPadMouse *dummyDevice = mafDeviceButtonsPadMouse::New();
  mafAction *dummyAction1 = mafAction::New();
  dummyAction1->SetName("dummyAction1");

  mafAction *dummyAction2 = mafAction::New();
  dummyAction2->SetName("dummyAction2");

  std::vector<mafAction *> actions;
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

void mafInteractorSERTest::TestGetDeviceBindings()
{
  mafInteractorSER *interactor = mafInteractorSER::New();

  mafDeviceButtonsPadMouse *dummyDevice = mafDeviceButtonsPadMouse::New();
  mafAction *dummyAction = mafAction::New();
  dummyAction->SetName("dummyAction");

  std::vector<mafAction *> actions;
  
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

void mafInteractorSERTest::TestBindAction()
{
  mafInteractorSER *interactor = mafInteractorSER::New();

  mafDeviceButtonsPadMouse *dummyDevice = mafDeviceButtonsPadMouse::New();
  mafAction *dummyAction = mafAction::New();
  dummyAction->SetName("dummyAction");

  interactor->AddAction(dummyAction);
  interactor->BindAction("dummyAction", interactor);

  mafAction *getAction = NULL;

  getAction =interactor->GetAction("dummyAction");
  CPPUNIT_ASSERT(getAction != NULL);
  CPPUNIT_ASSERT(mafString(getAction->GetName()).Equals("dummyAction"));
  
  dummyDevice->Delete();

  interactor->Delete();

  // deleting the interactor will delete the action (?)
  // dummyAction->Delete();
}

void mafInteractorSERTest::TestUnBindAction()
{
  mafInteractorSER *interactor = mafInteractorSER::New();

  mafDeviceButtonsPadMouse *dummyDevice = mafDeviceButtonsPadMouse::New();
  mafAction *dummyAction = mafAction::New();
  dummyAction->SetName("dummyAction");

  std::vector<mafAction *> bindings;

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

void mafInteractorSERTest::TestAddAction()
{
  mafInteractorSER *interactor = mafInteractorSER::New();

  mafDeviceButtonsPadMouse *dummyDevice = mafDeviceButtonsPadMouse::New();
  mafAction *dummyAction = mafAction::New();
  dummyAction->SetName("dummyAction");

  CPPUNIT_ASSERT(interactor->GetActions()->size() == 0);

  interactor->AddAction(dummyAction);

  CPPUNIT_ASSERT(interactor->GetActions()->size() == 1);
  dummyDevice->Delete();

  interactor->Delete();

  // deleting the interactor will delete the action (?)
  // dummyAction->Delete();
}


void mafInteractorSERTest::TestGetAction()
{
  mafInteractorSER *interactor = mafInteractorSER::New();

  mafDeviceButtonsPadMouse *dummyDevice = mafDeviceButtonsPadMouse::New();
  mafAction *dummyAction = mafAction::New();
  dummyAction->SetName("dummyAction");

  interactor->AddAction(dummyAction);

  mafAction *getAction = NULL;

  getAction = interactor->GetAction("dummyAction");
  CPPUNIT_ASSERT(getAction != NULL);
  CPPUNIT_ASSERT(mafString(getAction->GetName()).Equals("dummyAction"));

  dummyDevice->Delete();

  interactor->Delete();

  // deleting the interactor will delete the action (?)
  // dummyAction->Delete();
}

void mafInteractorSERTest::TestGetActions()
{
  mafInteractorSER *interactor = mafInteractorSER::New();

  mafDeviceButtonsPadMouse *dummyDevice = mafDeviceButtonsPadMouse::New();
  mafAction *dummyAction1 = mafAction::New();
  dummyAction1->SetName("dummyAction1");

  mafAction *dummyAction2 = mafAction::New();
  dummyAction2->SetName("dummyAction2");

  mafAction *dummyAction3 = mafAction::New();
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
