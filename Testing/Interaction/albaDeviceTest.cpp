/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceTest
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
#include "albaDeviceTest.h"
#include "albaEventInteraction.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaDevice.h"
#include "albaGUI.h"

//----------------------------------------------------------------------------
void albaDeviceTest::TestFixture()
{

}
//----------------------------------------------------------------------------
void albaDeviceTest::TestConstructorDestructor()
{
  albaDevice *dummyDevice = albaDevice::New();
  CPPUNIT_ASSERT(dummyDevice);

  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceTest::TestSetGetID()
{
  albaDevice *dummyDevice = albaDevice::New();
  CPPUNIT_ASSERT(dummyDevice);
  unsigned long defaultID = dummyDevice->GetID();

  CPPUNIT_ASSERT(defaultID == 0);
  
  unsigned long newID = 55;
  dummyDevice->SetID(newID);

  CPPUNIT_ASSERT_EQUAL(dummyDevice->GetID(), newID);

  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceTest::TestSetName()
{
  albaDevice *dummyDevice = albaDevice::New();
  CPPUNIT_ASSERT(dummyDevice);
  albaString defaultName = dummyDevice->GetName();
  CPPUNIT_ASSERT(defaultName.Equals(""));

  const char *newName = "Dummy";
  dummyDevice->SetName(newName);

  CPPUNIT_ASSERT(albaString::Compare(dummyDevice->GetName(), newName) == 0);
  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceTest::TestSetGetAutoStart()
{
  albaDevice *dummyDevice = albaDevice::New();
  CPPUNIT_ASSERT(dummyDevice);
  
  bool autostartEnabled = dummyDevice->GetAutoStart();
  CPPUNIT_ASSERT(autostartEnabled == false);

  dummyDevice->SetAutoStart(true);
  autostartEnabled = dummyDevice->GetAutoStart();
  CPPUNIT_ASSERT(autostartEnabled == true);

  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceTest::TestAutoStartOnOff()
{
  albaDevice *dummyDevice = albaDevice::New();
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
//----------------------------------------------------------------------------
void albaDeviceTest::TestStart()
{
  albaDevice *dummyDevice = albaDevice::New();
  CPPUNIT_ASSERT(dummyDevice);
  
  int returnValue = dummyDevice->Start();

  CPPUNIT_ASSERT_EQUAL(returnValue,0);
  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceTest::TestStop()
{
  albaDevice *dummyDevice = albaDevice::New();
  CPPUNIT_ASSERT(dummyDevice);

  int returnValue = dummyDevice->Start();
  CPPUNIT_ASSERT_EQUAL(returnValue,0);

  dummyDevice->Stop();

  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceTest::TestStartUp()
{
  albaDevice *dummyDevice = albaDevice::New();
  CPPUNIT_ASSERT(dummyDevice);
  
  int startup = dummyDevice->StartUp();
  CPPUNIT_ASSERT_EQUAL(startup , 0);

  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceTest::TestSetGetPersistentFlag()
{
  albaDevice *dummyDevice = albaDevice::New();
  CPPUNIT_ASSERT(dummyDevice);
  
  bool persistent = dummyDevice->GetPersistentFlag();
  CPPUNIT_ASSERT_EQUAL(persistent , false);

  dummyDevice->SetPersistentFlag(true);
  persistent = dummyDevice->GetPersistentFlag();
  CPPUNIT_ASSERT_EQUAL(persistent , true);

  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceTest::TestIsPersistent()
{
  albaDevice *dummyDevice = albaDevice::New();
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
//----------------------------------------------------------------------------
void albaDeviceTest::TestGetGui()
{
  albaDevice *dummyDevice = albaDevice::New();
  CPPUNIT_ASSERT(dummyDevice);

  // This crash tha test since the GetGui method also creates the gui.
  // Cannot change this for backward compatibility and potential side 
  // effects in vertical applications
  // albaGUI *deviceGUI = dummyDevice->GetGui();
  // CPPUNIT_ASSERT(deviceGUI == NULL);
  std::cout << " (!) Cannot test by command line";

  dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceTest::TestUpdateGui()
{
  std::cout << " (!) Cannot test by command line";
}
//----------------------------------------------------------------------------
void albaDeviceTest::TestOnEvent()
{
  albaDevice *dummyDevice = albaDevice::New();
  CPPUNIT_ASSERT(dummyDevice);

  albaEventInteraction dummyMouse2DMoveEvent;
  dummyMouse2DMoveEvent.SetChannel(MCH_INPUT);
  dummyMouse2DMoveEvent.SetSender(this);
  dummyMouse2DMoveEvent.SetId(albaDeviceButtonsPadMouse::GetMouse2DMoveId());

  dummyDevice->OnEvent(&dummyMouse2DMoveEvent);
  dummyDevice->Delete();
}