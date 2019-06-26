/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceManagerTest
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
#include "albaDeviceManagerTest.h"
#include "albaDeviceManager.h"
#include "albaDeviceSet.h"
#include "albaDevice.h"

//----------------------------------------------------------------------------
void albaDeviceManagerTest::TestFixture()
{

}
//----------------------------------------------------------------------------
void albaDeviceManagerTest::TestConstructorDestructor()
{
  albaDeviceManager *deviceManager = albaDeviceManager::New();
  CPPUNIT_ASSERT(deviceManager);
  deviceManager->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceManagerTest::TestAddDevice()
{
  albaDeviceManager *deviceManager = albaDeviceManager::New();

  int numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 0);

  albaDevice *dummyDevice = albaDevice::New();

  deviceManager->AddDevice(dummyDevice);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 1);

  deviceManager->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceManagerTest::TestRemoveDevice()
{
  albaDeviceManager *deviceManager = albaDeviceManager::New();

  int numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 0);

  albaDevice *dummyDevice = albaDevice::New();

  deviceManager->AddDevice(dummyDevice);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 1);

  deviceManager->RemoveDevice(dummyDevice);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 0);

  deviceManager->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceManagerTest::TestGetDevice()
{
  albaDeviceManager *deviceManager = albaDeviceManager::New();

  int numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 0);

  albaDevice *dummyDevice = albaDevice::New();

  std::string dummyDeviceName = "dummyDevice" ;
  dummyDevice->SetName(dummyDeviceName.c_str());

  deviceManager->AddDevice(dummyDevice);
  
  numberOfDevices = deviceManager->GetNumberOfDevices();
  
  CPPUNIT_ASSERT(numberOfDevices == 1);

  albaDevice *returnedDevice = deviceManager->GetDevice(dummyDeviceName.c_str());
  CPPUNIT_ASSERT(returnedDevice == dummyDevice);

  deviceManager->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceManagerTest::TestGetNumberOfDevices()
{
  albaDeviceManager *deviceManager = albaDeviceManager::New();

  int numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 0);

  albaDevice *dummyDevice1 = albaDevice::New();
  deviceManager->AddDevice(dummyDevice1);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 1);
  
  albaDevice *dummyDevice2 = albaDevice::New();
  deviceManager->AddDevice(dummyDevice2);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 2);

  deviceManager->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceManagerTest::TestRemoveAllDevices()
{

  albaDeviceManager *deviceManager = albaDeviceManager::New();

  int numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 0);

  albaDevice *dummyDevice1 = albaDevice::New();
  deviceManager->AddDevice(dummyDevice1);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 1);

  albaDevice *dummyDevice2 = albaDevice::New();
  deviceManager->AddDevice(dummyDevice2);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 2);
  
  deviceManager->RemoveAllDevices();
  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 0);

  deviceManager->Delete();
}
//----------------------------------------------------------------------------
void albaDeviceManagerTest::TestGetDevices()
{
  albaDeviceManager *deviceManager = albaDeviceManager::New();

  std::list<albaDevice *> *deviceList = deviceManager->GetDevices();
  int deviceListSize = deviceList->size();

  CPPUNIT_ASSERT(deviceListSize == 0);

  int numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 0);

  albaDevice *dummyDevice1 = albaDevice::New();
  deviceManager->AddDevice(dummyDevice1);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 1);
  
  deviceListSize = deviceList->size();
  CPPUNIT_ASSERT(deviceListSize == 1);

  albaDevice *dummyDevice2 = albaDevice::New();
  deviceManager->AddDevice(dummyDevice2);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 2);

  deviceListSize = deviceList->size();
  CPPUNIT_ASSERT(deviceListSize == 2);

  deviceManager->Delete();

}
//----------------------------------------------------------------------------
void albaDeviceManagerTest::TestGetDeviceSet()
{
  albaDeviceManager *deviceManager = albaDeviceManager::New();

  albaDeviceSet *deviceSet = deviceManager->GetDeviceSet();

  int numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 0);
  
  int deviceSetSize = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT(deviceSetSize == 0);

  albaDevice *dummyDevice1 = albaDevice::New();
  deviceManager->AddDevice(dummyDevice1);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 1);

  deviceSetSize = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT(deviceSetSize == 1);

  albaDevice *dummyDevice2 = albaDevice::New();
  deviceManager->AddDevice(dummyDevice2);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 2);

  deviceSetSize = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT(deviceSetSize == 2);

  deviceManager->Delete();
}
