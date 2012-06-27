/*=========================================================================

 Program: MAF2
 Module: mafActionTest
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
#include "mafActionTest.h"
#include "mafAction.h"
#include "mafDevice.h"
#include "mafInteractorCompositorMouse.h"

typedef std::list<mafAutoPointer<mafDevice> > mmuDeviceList;

void mafActionTest::setUp()
{

}

void mafActionTest::tearDown()
{

}

void mafActionTest::TestFixture()
{

}

void mafActionTest::TestConstructorDestructor()
{
  mafAction *action = NULL;
  action = mafAction::New();
  CPPUNIT_ASSERT(action);
  action->Delete();
  
  // a sample smart pointer constructor to check for leaks 
  mafSmartPointer<mafAction> actionSmartPointer;

}

void mafActionTest::TestSetGetType()
{
  mafAction *action = NULL;
  action = mafAction::New();
  CPPUNIT_ASSERT(action);

  int defaultActionType = action->GetType();
  
  action->SetType(mafAction::EXCLUSIVE_ACTION);
  CPPUNIT_ASSERT(action->GetType() == mafAction::EXCLUSIVE_ACTION);

  action->Delete(); 
}

void mafActionTest::TestSetTypeToShared()
{
  mafAction *action = NULL;
  action = mafAction::New();
  CPPUNIT_ASSERT(action);

  action->SetTypeToShared();
  CPPUNIT_ASSERT(action->GetType() == mafAction::SHARED_ACTION);

  action->Delete();
}

void mafActionTest::TestSetTypeToExclusive()
{
  mafAction *action = NULL;
  action = mafAction::New();
  CPPUNIT_ASSERT(action);

  action->SetTypeToExclusive();
  CPPUNIT_ASSERT(action->GetType() == mafAction::EXCLUSIVE_ACTION);

  action->Delete();
}

void mafActionTest::TestBindDevice()
{
  mafDevice *dummyDevice = NULL;
  dummyDevice =  mafDevice::New();
  dummyDevice->SetName("dummy");

  mafAction *action = NULL;
  action = mafAction::New();
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

void mafActionTest::TestUnBindDevice()
{
  mafDevice *dummyDevice = NULL;
  dummyDevice =  mafDevice::New();
  dummyDevice->SetName("dummy device");

  mafAction *action = NULL;
  action = mafAction::New();
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

void mafActionTest::TestBindInteractor()
{
  mafInteractorCompositorMouse *dummyInteractor = NULL;
  dummyInteractor =  mafInteractorCompositorMouse::New();
  dummyInteractor->SetName("dummy interactor");

  mafAction *action = NULL;
  action = mafAction::New();
  CPPUNIT_ASSERT(action);
  
  std::vector<mafObserver *> actionChannelObservers;
  action->GetObservers(MCH_INPUT, actionChannelObservers);
  CPPUNIT_ASSERT(actionChannelObservers.size() == 0);

  action->BindInteractor(dummyInteractor);
  
  action->GetObservers(MCH_INPUT, actionChannelObservers);
  CPPUNIT_ASSERT(actionChannelObservers.size() == 1);

  action->Delete();

  // dummyInteractor is not destroyed by action destructor
  dummyInteractor->Delete();
}

void mafActionTest::TestUnBindInteractor()
{
  mafInteractorCompositorMouse *dummyInteractor = NULL;
  dummyInteractor =  mafInteractorCompositorMouse::New();
  dummyInteractor->SetName("dummy interactor");

  mafAction *action = NULL;
  action = mafAction::New();
  CPPUNIT_ASSERT(action);

  std::vector<mafObserver *> actionChannelObservers;
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

void mafActionTest::TestGetDevices()
{
  mafDevice *dummyDevice1 = NULL;
  dummyDevice1 =  mafDevice::New();
  dummyDevice1->SetName("dummy device 1");

  mafAction *action = NULL;
  action = mafAction::New();
  CPPUNIT_ASSERT(action);

  mafDevice *dummyDevice2 = NULL;
  dummyDevice2 =  mafDevice::New();
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
