/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceSetTest
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
#include "albaDeviceSetTest.h"
#include "albaDeviceSet.h"

//----------------------------------------------------------------------------
void albaDeviceSetTest::TestConstructorDestructor()
{
  albaDeviceSet *deviceSet = albaDeviceSet::New();
  CPPUNIT_ASSERT(deviceSet);
  deviceSet->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceSetTest::TestGetDeviceByIndex()
{ 
  albaDeviceSet *deviceSet = albaDeviceSet::New();

  albaID dummyDeviceID = 10;
  std::string dummyDeviceName = "Dummy Device";

  albaDevice *dummyDevice = albaDevice::New();
  dummyDevice->SetName(dummyDeviceName.c_str());
  dummyDevice->SetID(dummyDeviceID);

  // index 0 since it's the first device added
  deviceSet->AddDevice(dummyDevice);

  albaDevice *getDevice = deviceSet->GetDeviceByIndex(0);

  CPPUNIT_ASSERT(getDevice);

  std::string getDeviceName = getDevice->GetName();

  CPPUNIT_ASSERT_EQUAL(getDeviceName, dummyDeviceName);

  deviceSet->Delete();
  
  // also destroy all devices so this is not needed
  // dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceSetTest::TestGetDeviceByName()
{
  albaDeviceSet *deviceSet = albaDeviceSet::New();

  albaID dummyDeviceID = 10;
  std::string dummyDeviceName = "Dummy Device";

  albaDevice *dummyDevice = albaDevice::New();
  dummyDevice->SetName(dummyDeviceName.c_str());
  dummyDevice->SetID(dummyDeviceID);

  deviceSet->AddDevice(dummyDevice);

  albaDevice *nullDevice = NULL;
  albaDevice *getDevice = NULL;
  
  std::string nonExistentName = "IDoNotExist";
  getDevice = deviceSet->GetDevice(nonExistentName.c_str());
  CPPUNIT_ASSERT_EQUAL(getDevice, nullDevice);

  getDevice = deviceSet->GetDevice(dummyDeviceName.c_str());
  
  std::string getDeviceName = getDevice->GetName();

  CPPUNIT_ASSERT_EQUAL(getDeviceName, dummyDeviceName);

  deviceSet->Delete();

  // also destroy all devices so this is not needed
  // dummyDevice->Delete();

}
//----------------------------------------------------------------------------
void albaDeviceSetTest::TestFixture()
{
  albaDeviceSet *deviceSet = albaDeviceSet::New();

  deviceSet->Delete();

}
//----------------------------------------------------------------------------
void albaDeviceSetTest::TestGetNumberOfDevices()
{
  albaDeviceSet *deviceSet = albaDeviceSet::New();
  
  int numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  albaID dummyDeviceID = 10;
  std::string dummyDeviceName = "Dummy Device";

  albaDevice *dummyDevice = albaDevice::New();
  dummyDevice->SetName(dummyDeviceName.c_str());
  dummyDevice->SetID(dummyDeviceID);

  deviceSet->AddDevice(dummyDevice);

  numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 1);

  deviceSet->RemoveDeviceByIndex(0);
  numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  deviceSet->Delete();

  // also destroy all devices so this is not needed
  // dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceSetTest::TestGetDeviceByID()
{
  albaDeviceSet *deviceSet = albaDeviceSet::New();

  albaID dummyDeviceID = 10;
  std::string dummyDeviceName = "Dummy Device";

  albaDevice *dummyDevice = albaDevice::New();
  dummyDevice->SetName(dummyDeviceName.c_str());
  dummyDevice->SetID(dummyDeviceID);

  deviceSet->AddDevice(dummyDevice);

  albaDevice *getDevice = deviceSet->GetDevice(dummyDeviceID);

  CPPUNIT_ASSERT(getDevice);

  std::string getDeviceName = getDevice->GetName();

  CPPUNIT_ASSERT_EQUAL(getDeviceName, dummyDeviceName);

  deviceSet->Delete();

  // also destroy all devices so this is not needed
  // dummyDevice->Delete();

}
//----------------------------------------------------------------------------
void albaDeviceSetTest::TestRemoveDeviceByIndex()
{
  albaDeviceSet *deviceSet = albaDeviceSet::New();

  int numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  albaID dummyDeviceID = 10;
  std::string dummyDeviceName = "Dummy Device";

  albaDevice *dummyDevice = albaDevice::New();
  dummyDevice->SetName(dummyDeviceName.c_str());
  dummyDevice->SetID(dummyDeviceID);

  deviceSet->AddDevice(dummyDevice);

  numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 1);

  int returnValue = deviceSet->RemoveDeviceByIndex(0);
  CPPUNIT_ASSERT_EQUAL(returnValue, (int)ALBA_OK);

  numberOfDevices = deviceSet->GetNumberOfDevices();

//  albaID nonExistentDeviceID = 20;
//  returnValue = deviceSet->RemoveDeviceByIndex(nonExistentDeviceID);
//  CPPUNIT_ASSERT_EQUAL(returnValue, (int)ALBA_ERROR);

  numberOfDevices = deviceSet->GetNumberOfDevices();

  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  deviceSet->Delete();

  // also destroy all devices so this is not needed
  // dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceSetTest::TestRemoveDeviceByID()
{
  albaDeviceSet *deviceSet = albaDeviceSet::New();

  int numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  albaID dummyDeviceID = 10;
  std::string dummyDeviceName = "Dummy Device";

  albaDevice *dummyDevice = albaDevice::New();
  dummyDevice->SetName(dummyDeviceName.c_str());
  dummyDevice->SetID(dummyDeviceID);

  deviceSet->AddDevice(dummyDevice);

  numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 1);

  int returnValue = deviceSet->RemoveDevice(dummyDeviceID);
  CPPUNIT_ASSERT_EQUAL(returnValue, (int)ALBA_OK);

  numberOfDevices = deviceSet->GetNumberOfDevices();

  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  deviceSet->Delete();

  // also destroy all devices so this is not needed
  // dummyDevice->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceSetTest::TestRemoveDeviceByName()
{
  albaDeviceSet *deviceSet = albaDeviceSet::New();

  int numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  albaID dummyDeviceID = 10;
  std::string dummyDeviceName = "Dummy Device";

  albaDevice *dummyDevice = albaDevice::New();
  dummyDevice->SetName(dummyDeviceName.c_str());
  dummyDevice->SetID(dummyDeviceID);

  deviceSet->AddDevice(dummyDevice);

  numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 1);

  int returnValue = deviceSet->RemoveDevice(dummyDeviceName.c_str());
  CPPUNIT_ASSERT_EQUAL(returnValue, (int)ALBA_OK);

  numberOfDevices = deviceSet->GetNumberOfDevices();

  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  deviceSet->Delete();

  // also destroy all devices so this is not needed
  // dummyDevice->Delete();

}
//----------------------------------------------------------------------------
void albaDeviceSetTest::TestRemoveDeviceByPointer()
{
  albaDeviceSet *deviceSet = albaDeviceSet::New();

  int numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  albaID dummyDeviceID = 10;
  std::string dummyDeviceName = "Dummy Device";

  albaDevice *dummyDevice = albaDevice::New();
  dummyDevice->SetName(dummyDeviceName.c_str());
  dummyDevice->SetID(dummyDeviceID);

  deviceSet->AddDevice(dummyDevice);

  numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 1);

  int returnValue = deviceSet->RemoveDevice(dummyDevice);
  CPPUNIT_ASSERT_EQUAL(returnValue, (int)ALBA_OK);

  numberOfDevices = deviceSet->GetNumberOfDevices();

  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  deviceSet->Delete();

  // also destroy all devices so this is not needed
  // deviceOne->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceSetTest::TestRemoveAllDevices()
{
  albaDeviceSet *deviceSet = albaDeviceSet::New();

  int numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  albaID deviceOneID = 10;
  std::string deviceOneName = "Device One";

  albaDevice *deviceOne = albaDevice::New();
  deviceOne->SetName(deviceOneName.c_str());
  deviceOne->SetID(deviceOneID);

  deviceSet->AddDevice(deviceOne);

  numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 1);
  
  albaID deviceTwoID = 10;
  std::string deviceTwoName = "Device Two";

  albaDevice *deviceTwo = albaDevice::New();
  deviceTwo->SetName(deviceTwoName.c_str());
  deviceTwo->SetID(deviceTwoID);

  deviceSet->AddDevice(deviceTwo);

  numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 2);

  deviceSet->RemoveAllDevices();

  numberOfDevices = deviceSet->GetNumberOfDevices();

  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  deviceSet->Delete();

  // also destroy all devices so this is not needed
  // deviceOne->Delete();

}
//----------------------------------------------------------------------------
void albaDeviceSetTest::TestGetDevices()
{
  albaDeviceSet *deviceSet = albaDeviceSet::New();

  int numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  albaID deviceOneID = 10;
  std::string deviceOneName = "Device One";

  albaDevice *deviceOne = albaDevice::New();
  deviceOne->SetName(deviceOneName.c_str());
  deviceOne->SetID(deviceOneID);

  deviceSet->AddDevice(deviceOne);

  numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 1);

  albaID deviceTwoID = 10;
  std::string deviceTwoName = "Device Two";

  albaDevice *deviceTwo = albaDevice::New();
  deviceTwo->SetName(deviceTwoName.c_str());
  deviceTwo->SetID(deviceTwoID);

  deviceSet->AddDevice(deviceTwo);

  numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 2);

  std::list<albaDevice *> *deviceList;

  deviceList = deviceSet->GetDevices();
  
  CPPUNIT_ASSERT(deviceList->size() == 2);

  deviceSet->RemoveAllDevices();

  CPPUNIT_ASSERT(deviceList->size() == 0);

  numberOfDevices = deviceSet->GetNumberOfDevices();

  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  deviceSet->Delete();

  // also destroy all devices so this is not needed
  // deviceOne->Delete();
}