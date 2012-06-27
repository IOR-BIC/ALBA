/*=========================================================================

 Program: MAF2
 Module: mafDeviceSetTest
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
#include "mafDeviceSetTest.h"
#include "mafDeviceSet.h"

void mafDeviceSetTest::setUp()
{

}

void mafDeviceSetTest::tearDown()
{

}

void mafDeviceSetTest::TestConstructorDestructor()
{
  mafDeviceSet *deviceSet = mafDeviceSet::New();
  CPPUNIT_ASSERT(deviceSet);
  deviceSet->Delete();
}

void mafDeviceSetTest::TestGetDeviceByIndex()
{ 
  mafDeviceSet *deviceSet = mafDeviceSet::New();

  mafID dummyDeviceID = 10;
  std::string dummyDeviceName = "Dummy Device";

  mafDevice *dummyDevice = mafDevice::New();
  dummyDevice->SetName(dummyDeviceName.c_str());
  dummyDevice->SetID(dummyDeviceID);

  // index 0 since it's the first device added
  deviceSet->AddDevice(dummyDevice);

  mafDevice *getDevice = deviceSet->GetDeviceByIndex(0);

  CPPUNIT_ASSERT(getDevice);

  std::string getDeviceName = getDevice->GetName();

  CPPUNIT_ASSERT_EQUAL(getDeviceName, dummyDeviceName);

  deviceSet->Delete();
  
  // also destroy all devices so this is not needed
  // dummyDevice->Delete();
}

void mafDeviceSetTest::TestGetDeviceByName()
{
  mafDeviceSet *deviceSet = mafDeviceSet::New();

  mafID dummyDeviceID = 10;
  std::string dummyDeviceName = "Dummy Device";

  mafDevice *dummyDevice = mafDevice::New();
  dummyDevice->SetName(dummyDeviceName.c_str());
  dummyDevice->SetID(dummyDeviceID);

  deviceSet->AddDevice(dummyDevice);

  mafDevice *nullDevice = NULL;
  mafDevice *getDevice = NULL;
  
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

void mafDeviceSetTest::TestFixture()
{
  mafDeviceSet *deviceSet = mafDeviceSet::New();

  deviceSet->Delete();

}

void mafDeviceSetTest::TestGetNumberOfDevices()
{
  mafDeviceSet *deviceSet = mafDeviceSet::New();
  
  int numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  mafID dummyDeviceID = 10;
  std::string dummyDeviceName = "Dummy Device";

  mafDevice *dummyDevice = mafDevice::New();
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

void mafDeviceSetTest::TestGetDeviceByID()
{
  mafDeviceSet *deviceSet = mafDeviceSet::New();

  mafID dummyDeviceID = 10;
  std::string dummyDeviceName = "Dummy Device";

  mafDevice *dummyDevice = mafDevice::New();
  dummyDevice->SetName(dummyDeviceName.c_str());
  dummyDevice->SetID(dummyDeviceID);

  deviceSet->AddDevice(dummyDevice);

  mafDevice *getDevice = deviceSet->GetDevice(dummyDeviceID);

  CPPUNIT_ASSERT(getDevice);

  std::string getDeviceName = getDevice->GetName();

  CPPUNIT_ASSERT_EQUAL(getDeviceName, dummyDeviceName);

  deviceSet->Delete();

  // also destroy all devices so this is not needed
  // dummyDevice->Delete();

}

void mafDeviceSetTest::TestRemoveDeviceByIndex()
{
  mafDeviceSet *deviceSet = mafDeviceSet::New();

  int numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  mafID dummyDeviceID = 10;
  std::string dummyDeviceName = "Dummy Device";

  mafDevice *dummyDevice = mafDevice::New();
  dummyDevice->SetName(dummyDeviceName.c_str());
  dummyDevice->SetID(dummyDeviceID);

  deviceSet->AddDevice(dummyDevice);

  numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 1);

  int returnValue = deviceSet->RemoveDeviceByIndex(0);
  CPPUNIT_ASSERT_EQUAL(returnValue, (int)MAF_OK);

  numberOfDevices = deviceSet->GetNumberOfDevices();

//  mafID nonExistentDeviceID = 20;
//  returnValue = deviceSet->RemoveDeviceByIndex(nonExistentDeviceID);
//  CPPUNIT_ASSERT_EQUAL(returnValue, (int)MAF_ERROR);

  numberOfDevices = deviceSet->GetNumberOfDevices();

  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  deviceSet->Delete();

  // also destroy all devices so this is not needed
  // dummyDevice->Delete();
}

void mafDeviceSetTest::TestRemoveDeviceByID()
{
  mafDeviceSet *deviceSet = mafDeviceSet::New();

  int numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  mafID dummyDeviceID = 10;
  std::string dummyDeviceName = "Dummy Device";

  mafDevice *dummyDevice = mafDevice::New();
  dummyDevice->SetName(dummyDeviceName.c_str());
  dummyDevice->SetID(dummyDeviceID);

  deviceSet->AddDevice(dummyDevice);

  numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 1);

  int returnValue = deviceSet->RemoveDevice(dummyDeviceID);
  CPPUNIT_ASSERT_EQUAL(returnValue, (int)MAF_OK);

  numberOfDevices = deviceSet->GetNumberOfDevices();

  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  deviceSet->Delete();

  // also destroy all devices so this is not needed
  // dummyDevice->Delete();
}

void mafDeviceSetTest::TestRemoveDeviceByName()
{
  mafDeviceSet *deviceSet = mafDeviceSet::New();

  int numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  mafID dummyDeviceID = 10;
  std::string dummyDeviceName = "Dummy Device";

  mafDevice *dummyDevice = mafDevice::New();
  dummyDevice->SetName(dummyDeviceName.c_str());
  dummyDevice->SetID(dummyDeviceID);

  deviceSet->AddDevice(dummyDevice);

  numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 1);

  int returnValue = deviceSet->RemoveDevice(dummyDeviceName.c_str());
  CPPUNIT_ASSERT_EQUAL(returnValue, (int)MAF_OK);

  numberOfDevices = deviceSet->GetNumberOfDevices();

  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  deviceSet->Delete();

  // also destroy all devices so this is not needed
  // dummyDevice->Delete();

}

void mafDeviceSetTest::TestRemoveDeviceByPointer()
{
  mafDeviceSet *deviceSet = mafDeviceSet::New();

  int numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  mafID dummyDeviceID = 10;
  std::string dummyDeviceName = "Dummy Device";

  mafDevice *dummyDevice = mafDevice::New();
  dummyDevice->SetName(dummyDeviceName.c_str());
  dummyDevice->SetID(dummyDeviceID);

  deviceSet->AddDevice(dummyDevice);

  numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 1);

  int returnValue = deviceSet->RemoveDevice(dummyDevice);
  CPPUNIT_ASSERT_EQUAL(returnValue, (int)MAF_OK);

  numberOfDevices = deviceSet->GetNumberOfDevices();

  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  deviceSet->Delete();

  // also destroy all devices so this is not needed
  // deviceOne->Delete();
}

void mafDeviceSetTest::TestRemoveAllDevices()
{
  mafDeviceSet *deviceSet = mafDeviceSet::New();

  int numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  mafID deviceOneID = 10;
  std::string deviceOneName = "Device One";

  mafDevice *deviceOne = mafDevice::New();
  deviceOne->SetName(deviceOneName.c_str());
  deviceOne->SetID(deviceOneID);

  deviceSet->AddDevice(deviceOne);

  numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 1);
  
  mafID deviceTwoID = 10;
  std::string deviceTwoName = "Device Two";

  mafDevice *deviceTwo = mafDevice::New();
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

void mafDeviceSetTest::TestGetDevices()
{
  mafDeviceSet *deviceSet = mafDeviceSet::New();

  int numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 0);

  mafID deviceOneID = 10;
  std::string deviceOneName = "Device One";

  mafDevice *deviceOne = mafDevice::New();
  deviceOne->SetName(deviceOneName.c_str());
  deviceOne->SetID(deviceOneID);

  deviceSet->AddDevice(deviceOne);

  numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 1);

  mafID deviceTwoID = 10;
  std::string deviceTwoName = "Device Two";

  mafDevice *deviceTwo = mafDevice::New();
  deviceTwo->SetName(deviceTwoName.c_str());
  deviceTwo->SetID(deviceTwoID);

  deviceSet->AddDevice(deviceTwo);

  numberOfDevices = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT_EQUAL(numberOfDevices, 2);

  std::list<mafDevice *> *deviceList;

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