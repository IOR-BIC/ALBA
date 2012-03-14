/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafDeviceTest.cpp,v $
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
#include "mafDeviceTest.h"
#include "mafEventInteraction.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafDevice.h"
#include "mafGUI.h"

void mafDeviceTest::setUp()
{

}

void mafDeviceTest::tearDown()
{

}

void mafDeviceTest::TestFixture()
{

}

void mafDeviceTest::TestConstructorDestructor()
{
  mafDevice *dummyDevice = mafDevice::New();
  CPPUNIT_ASSERT(dummyDevice);

  dummyDevice->Delete();
}

void mafDeviceTest::TestSetGetID()
{
  mafDevice *dummyDevice = mafDevice::New();
  CPPUNIT_ASSERT(dummyDevice);
  unsigned long defaultID = dummyDevice->GetID();

  CPPUNIT_ASSERT(defaultID == 0);
  
  unsigned long newID = 55;
  dummyDevice->SetID(newID);

  CPPUNIT_ASSERT_EQUAL(dummyDevice->GetID(), newID);

  dummyDevice->Delete();
}

void mafDeviceTest::TestSetName()
{
  mafDevice *dummyDevice = mafDevice::New();
  CPPUNIT_ASSERT(dummyDevice);
  mafString defaultName = dummyDevice->GetName();
  CPPUNIT_ASSERT(defaultName.Equals(""));

  const char *newName = "Dummy";
  dummyDevice->SetName(newName);

  CPPUNIT_ASSERT(mafString::Compare(dummyDevice->GetName(), newName) == 0);
  dummyDevice->Delete();
}

void mafDeviceTest::TestSetGetAutoStart()
{
  mafDevice *dummyDevice = mafDevice::New();
  CPPUNIT_ASSERT(dummyDevice);
  
  bool autostartEnabled = dummyDevice->GetAutoStart();
  CPPUNIT_ASSERT(autostartEnabled == false);

  dummyDevice->SetAutoStart(true);
  autostartEnabled = dummyDevice->GetAutoStart();
  CPPUNIT_ASSERT(autostartEnabled == true);

  dummyDevice->Delete();
}

void mafDeviceTest::TestAutoStartOnOff()
{
  mafDevice *dummyDevice = mafDevice::New();
  CPPUNIT_ASSERT(dummyDevice);
  
  bool autostartEnabled = dummyDevice->GetAutoStart();
  CPPUNIT_ASSERT(autostartEnabled == false);

  dummyDevice->AutoStartOn();
  autostartEnabled = dummyDevice->GetAutoStart();
  CPPUNIT_ASSERT(autostartEnabled == true);

  dummyDevice->AutoStartOff();
  autostartEnabled = dummyDevice->GetAutoStart();
  CPPUNIT_ASSERT(autostartEnabled == false);

  dummyDevice->Delete();
}

void mafDeviceTest::TestStart()
{
  mafDevice *dummyDevice = mafDevice::New();
  CPPUNIT_ASSERT(dummyDevice);
  
  int returnValue = dummyDevice->Start();

  CPPUNIT_ASSERT_EQUAL(returnValue,0);
  dummyDevice->Delete();
}

void mafDeviceTest::TestStop()
{
  mafDevice *dummyDevice = mafDevice::New();
  CPPUNIT_ASSERT(dummyDevice);

  int returnValue = dummyDevice->Start();
  CPPUNIT_ASSERT_EQUAL(returnValue,0);

  dummyDevice->Stop();

  CPPUNIT_ASSERT(true);

  dummyDevice->Delete();
}

void mafDeviceTest::TestStartUp()
{
  mafDevice *dummyDevice = mafDevice::New();
  CPPUNIT_ASSERT(dummyDevice);
  
  int startup = dummyDevice->StartUp();
  CPPUNIT_ASSERT_EQUAL(startup , 0);

  dummyDevice->Delete();
}

void mafDeviceTest::TestLock()
{
  mafDevice *dummyDevice = mafDevice::New();
  CPPUNIT_ASSERT(dummyDevice);
  
  bool lock = dummyDevice->Lock();
  CPPUNIT_ASSERT_EQUAL(lock, true);


  dummyDevice->Delete();
}

void mafDeviceTest::TestUnlock()
{
  mafDevice *dummyDevice = mafDevice::New();
  CPPUNIT_ASSERT(dummyDevice);
  
  bool lock = dummyDevice->Lock();
  CPPUNIT_ASSERT_EQUAL(lock, true);

  bool isLocked = dummyDevice->IsLocked();
  CPPUNIT_ASSERT_EQUAL(isLocked, true);

  dummyDevice->Unlock();

  isLocked = dummyDevice->IsLocked();
  CPPUNIT_ASSERT_EQUAL(isLocked, false);

  dummyDevice->Delete();
}

void mafDeviceTest::TestIsLocked()
{
  mafDevice *dummyDevice = mafDevice::New();
  CPPUNIT_ASSERT(dummyDevice);
  
  bool defaultLock = dummyDevice->IsLocked();
  CPPUNIT_ASSERT_EQUAL(defaultLock, false);

  bool returnValue = dummyDevice->Lock();
  CPPUNIT_ASSERT_EQUAL(returnValue, true);
  
  dummyDevice->Unlock();
  bool isLocked = dummyDevice->IsLocked();
  CPPUNIT_ASSERT_EQUAL(defaultLock, false);

  dummyDevice->Delete();
}

void mafDeviceTest::TestSetGetPersistentFlag()
{
  mafDevice *dummyDevice = mafDevice::New();
  CPPUNIT_ASSERT(dummyDevice);
  
  bool persistent = dummyDevice->GetPersistentFlag();
  CPPUNIT_ASSERT_EQUAL(persistent , false);

  dummyDevice->SetPersistentFlag(true);
  persistent = dummyDevice->GetPersistentFlag();
  CPPUNIT_ASSERT_EQUAL(persistent , true);

  dummyDevice->Delete();
}

void mafDeviceTest::TestIsPersistent()
{
  mafDevice *dummyDevice = mafDevice::New();
  CPPUNIT_ASSERT(dummyDevice);
  
  bool persistent = dummyDevice->GetPersistentFlag();
  CPPUNIT_ASSERT_EQUAL(persistent , false);

  dummyDevice->SetPersistentFlag(true);
  persistent = dummyDevice->GetPersistentFlag();
  CPPUNIT_ASSERT_EQUAL(persistent , true);
  
  dummyDevice->SetPersistentFlag(false);
  persistent = dummyDevice->GetPersistentFlag();
  CPPUNIT_ASSERT_EQUAL(persistent , false);

  dummyDevice->Delete();
}

void mafDeviceTest::TestGetGui()
{
  mafDevice *dummyDevice = mafDevice::New();
  CPPUNIT_ASSERT(dummyDevice);

  // This crash tha test since the GetGui method also creates the gui.
  // Cannot change this for backward compatibility and potential side 
  // effects in vertical applications
  // mafGUI *deviceGUI = dummyDevice->GetGui();
  // CPPUNIT_ASSERT(deviceGUI == NULL);
  std::cout << " (!) Cannot test by command line";

  dummyDevice->Delete();
}

void mafDeviceTest::TestUpdateGui()
{
  std::cout << " (!) Cannot test by command line";
}

void mafDeviceTest::TestOnEvent()
{
  mafDevice *dummyDevice = mafDevice::New();
  CPPUNIT_ASSERT(dummyDevice);

  mafEventInteraction dummyMouse2DMoveEvent;
  dummyMouse2DMoveEvent.SetChannel(MCH_INPUT);
  dummyMouse2DMoveEvent.SetSender(this);
  dummyMouse2DMoveEvent.SetId(mafDeviceButtonsPadMouse::GetMouse2DMoveId());

  dummyDevice->OnEvent(&dummyMouse2DMoveEvent);
  CPPUNIT_ASSERT(true); 

  dummyDevice->Delete();
}