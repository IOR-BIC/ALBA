/*=========================================================================

 Program: MAF2
 Module: mafDeviceManagerTest
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
#include "mafDeviceManagerTest.h"
#include "mafDeviceManager.h"
#include "mafDeviceSet.h"
#include "mafDevice.h"

void mafDeviceManagerTest::setUp()
{

}

void mafDeviceManagerTest::tearDown()
{

}

void mafDeviceManagerTest::TestFixture()
{

}

void mafDeviceManagerTest::TestConstructorDestructor()
{
  mafDeviceManager *deviceManager = mafDeviceManager::New();
  CPPUNIT_ASSERT(deviceManager);
  deviceManager->Delete();
}

void mafDeviceManagerTest::TestAddDevice()
{
  mafDeviceManager *deviceManager = mafDeviceManager::New();

  int numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 0);

  mafDevice *dummyDevice = mafDevice::New();

  deviceManager->AddDevice(dummyDevice);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 1);

  deviceManager->Delete();
}

void mafDeviceManagerTest::TestRemoveDevice()
{
  mafDeviceManager *deviceManager = mafDeviceManager::New();

  int numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 0);

  mafDevice *dummyDevice = mafDevice::New();

  deviceManager->AddDevice(dummyDevice);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 1);

  deviceManager->RemoveDevice(dummyDevice);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 0);

  deviceManager->Delete();
}

void mafDeviceManagerTest::TestGetDevice()
{
  mafDeviceManager *deviceManager = mafDeviceManager::New();

  int numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 0);

  mafDevice *dummyDevice = mafDevice::New();

  std::string dummyDeviceName = "dummyDevice" ;
  dummyDevice->SetName(dummyDeviceName.c_str());

  deviceManager->AddDevice(dummyDevice);
  
  numberOfDevices = deviceManager->GetNumberOfDevices();
  
  CPPUNIT_ASSERT(numberOfDevices == 1);

  mafDevice *returnedDevice = deviceManager->GetDevice(dummyDeviceName.c_str());
  CPPUNIT_ASSERT(returnedDevice == dummyDevice);

  deviceManager->Delete();
}

void mafDeviceManagerTest::TestGetNumberOfDevices()
{
  mafDeviceManager *deviceManager = mafDeviceManager::New();

  int numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 0);

  mafDevice *dummyDevice1 = mafDevice::New();
  deviceManager->AddDevice(dummyDevice1);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 1);
  
  mafDevice *dummyDevice2 = mafDevice::New();
  deviceManager->AddDevice(dummyDevice2);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 2);

  deviceManager->Delete();
}

void mafDeviceManagerTest::TestRemoveAllDevices()
{

  mafDeviceManager *deviceManager = mafDeviceManager::New();

  int numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 0);

  mafDevice *dummyDevice1 = mafDevice::New();
  deviceManager->AddDevice(dummyDevice1);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 1);

  mafDevice *dummyDevice2 = mafDevice::New();
  deviceManager->AddDevice(dummyDevice2);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 2);
  
  deviceManager->RemoveAllDevices();
  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 0);

  deviceManager->Delete();
}

void mafDeviceManagerTest::TestGetDevices()
{
  mafDeviceManager *deviceManager = mafDeviceManager::New();

  std::list<mafDevice *> *deviceList = deviceManager->GetDevices();
  int deviceListSize = deviceList->size();

  CPPUNIT_ASSERT(deviceListSize == 0);

  int numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 0);

  mafDevice *dummyDevice1 = mafDevice::New();
  deviceManager->AddDevice(dummyDevice1);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 1);
  
  deviceListSize = deviceList->size();
  CPPUNIT_ASSERT(deviceListSize == 1);

  mafDevice *dummyDevice2 = mafDevice::New();
  deviceManager->AddDevice(dummyDevice2);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 2);

  deviceListSize = deviceList->size();
  CPPUNIT_ASSERT(deviceListSize == 2);

  deviceManager->Delete();

}

void mafDeviceManagerTest::TestGetDeviceSet()
{
  mafDeviceManager *deviceManager = mafDeviceManager::New();

  mafDeviceSet *deviceSet = deviceManager->GetDeviceSet();

  int numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 0);
  
  int deviceSetSize = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT(deviceSetSize == 0);

  mafDevice *dummyDevice1 = mafDevice::New();
  deviceManager->AddDevice(dummyDevice1);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 1);

  deviceSetSize = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT(deviceSetSize == 1);

  mafDevice *dummyDevice2 = mafDevice::New();
  deviceManager->AddDevice(dummyDevice2);

  numberOfDevices = deviceManager->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 2);

  deviceSetSize = deviceSet->GetNumberOfDevices();
  CPPUNIT_ASSERT(deviceSetSize == 2);

  deviceManager->Delete();
}
