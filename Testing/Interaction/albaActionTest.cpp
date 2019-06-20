/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaActionTest
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
#include "albaActionTest.h"
#include "albaAction.h"
#include "albaDevice.h"
#include "albaInteractorCompositorMouse.h"

typedef std::list<albaAutoPointer<albaDevice> > mmuDeviceList;

void albaActionTest::TestFixture()
{

}

void albaActionTest::TestConstructorDestructor()
{
  albaAction *action = NULL;
  action = albaAction::New();
  CPPUNIT_ASSERT(action);
  action->Delete();
  
  // a sample smart pointer constructor to check for leaks 
  albaSmartPointer<albaAction> actionSmartPointer;

}

void albaActionTest::TestSetGetType()
{
  albaAction *action = NULL;
  action = albaAction::New();
  CPPUNIT_ASSERT(action);

  int defaultActionType = action->GetType();
  
  action->SetType(albaAction::EXCLUSIVE_ACTION);
  CPPUNIT_ASSERT(action->GetType() == albaAction::EXCLUSIVE_ACTION);

  action->Delete(); 
}

void albaActionTest::TestSetTypeToShared()
{
  albaAction *action = NULL;
  action = albaAction::New();
  CPPUNIT_ASSERT(action);

  action->SetTypeToShared();
  CPPUNIT_ASSERT(action->GetType() == albaAction::SHARED_ACTION);

  action->Delete();
}

void albaActionTest::TestSetTypeToExclusive()
{
  albaAction *action = NULL;
  action = albaAction::New();
  CPPUNIT_ASSERT(action);

  action->SetTypeToExclusive();
  CPPUNIT_ASSERT(action->GetType() == albaAction::EXCLUSIVE_ACTION);

  action->Delete();
}

void albaActionTest::TestBindDevice()
{
  albaDevice *dummyDevice = NULL;
  dummyDevice =  albaDevice::New();
  dummyDevice->SetName("dummy");

  albaAction *action = NULL;
  action = albaAction::New();
  CPPUNIT_ASSERT(action);
  
  const mmuDeviceList *devicesBound = NULL;
  devicesBound = action->GetDevices();
  CPPUNIT_ASSERT(devicesBound->size() == 0);

  action->BindDevice(dummyDevice);
  CPPUNIT_ASSERT(devicesBound->size() == 1);
  CPPUNIT_ASSERT(devicesBound->front().GetPointer() == dummyDevice);
  
  //  dummyDevice is destroyed by action destructor
  action->Delete();
  
}

void albaActionTest::TestUnBindDevice()
{
  albaDevice *dummyDevice = NULL;
  dummyDevice =  albaDevice::New();
  dummyDevice->SetName("dummy device");

  albaAction *action = NULL;
  action = albaAction::New();
  CPPUNIT_ASSERT(action);

  const mmuDeviceList *devicesBound = NULL;
  devicesBound = action->GetDevices();

  action->BindDevice(dummyDevice);
  CPPUNIT_ASSERT(devicesBound->size() == 1);
  CPPUNIT_ASSERT(devicesBound->front().GetPointer() == dummyDevice);

  action->UnBindDevice(dummyDevice);
  CPPUNIT_ASSERT(devicesBound->size() == 0);
 
  // dummyDevice is destroyed by action destructor
  action->Delete();
}

void albaActionTest::TestBindInteractor()
{
  albaInteractorCompositorMouse *dummyInteractor = NULL;
  dummyInteractor =  albaInteractorCompositorMouse::New();
  dummyInteractor->SetName("dummy interactor");

  albaAction *action = NULL;
  action = albaAction::New();
  CPPUNIT_ASSERT(action);
  
  std::vector<albaObserver *> actionChannelObservers;
  action->GetObservers(MCH_INPUT, actionChannelObservers);
  CPPUNIT_ASSERT(actionChannelObservers.size() == 0);

  action->BindInteractor(dummyInteractor);
  
  action->GetObservers(MCH_INPUT, actionChannelObservers);
  CPPUNIT_ASSERT(actionChannelObservers.size() == 1);

  action->Delete();

  // dummyInteractor is not destroyed by action destructor
  dummyInteractor->Delete();
}

void albaActionTest::TestUnBindInteractor()
{
  albaInteractorCompositorMouse *dummyInteractor = NULL;
  dummyInteractor =  albaInteractorCompositorMouse::New();
  dummyInteractor->SetName("dummy interactor");

  albaAction *action = NULL;
  action = albaAction::New();
  CPPUNIT_ASSERT(action);

  std::vector<albaObserver *> actionChannelObservers;
  action->GetObservers(MCH_INPUT, actionChannelObservers);
  CPPUNIT_ASSERT(actionChannelObservers.size() == 0);

  action->BindInteractor(dummyInteractor);

  action->GetObservers(MCH_INPUT, actionChannelObservers);
  CPPUNIT_ASSERT(actionChannelObservers.size() == 1);
  
  action->UnBindInteractor(dummyInteractor);
  action->GetObservers(MCH_INPUT, actionChannelObservers);
  CPPUNIT_ASSERT(actionChannelObservers.size() == 0);

  action->Delete();

  // dummyInteractor is not destroyed by action destructor
  dummyInteractor->Delete();

}

void albaActionTest::TestGetDevices()
{
  albaDevice *dummyDevice1 = NULL;
  dummyDevice1 =  albaDevice::New();
  dummyDevice1->SetName("dummy device 1");

  albaAction *action = NULL;
  action = albaAction::New();
  CPPUNIT_ASSERT(action);

  albaDevice *dummyDevice2 = NULL;
  dummyDevice2 =  albaDevice::New();
  dummyDevice2->SetName("dummy device 2");

  const mmuDeviceList *devicesBound = NULL;
  devicesBound = action->GetDevices();
  CPPUNIT_ASSERT(devicesBound->size() == 0);

  action->BindDevice(dummyDevice1);
  CPPUNIT_ASSERT(devicesBound->size() == 1);
  CPPUNIT_ASSERT(devicesBound->front().GetPointer() == dummyDevice1);

  action->BindDevice(dummyDevice2);
  CPPUNIT_ASSERT(devicesBound->size() == 2);
  CPPUNIT_ASSERT(devicesBound->back().GetPointer() == dummyDevice2);
  
  action->UnBindDevice(dummyDevice1);
  CPPUNIT_ASSERT(devicesBound->size() == 1);
  CPPUNIT_ASSERT(devicesBound->front().GetPointer() == dummyDevice2);

  action->UnBindDevice(dummyDevice2);
  CPPUNIT_ASSERT(devicesBound->size() == 0);

  // dummyDevice1 and dummyDevice2 are destroyed by action destructor
  action->Delete();

}
