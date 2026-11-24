/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionFactoryTest
 Authors: Eleonora Mambrini
 
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
#include "albaInteractionFactoryTest.h"

#include "albaInteractionFactory.h"
#include "albaDevice.h"
#include <iostream>
#include <iterator>



//-------------------------------------------------------------------------
/** a simple VME created just for testing purposes. */
class albaDeviceCustom : public albaDevice
{
public:
  albaTypeMacro(albaDeviceCustom,albaDevice);

protected:
  albaDeviceCustom();
  virtual ~albaDeviceCustom();

private:
  albaDeviceCustom(const albaDeviceCustom&); // Not implemented
  void operator=(const albaDeviceCustom&); // Not implemented
};

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaDeviceCustom);

//-------------------------------------------------------------------------
albaDeviceCustom::albaDeviceCustom()
{

}
//-------------------------------------------------------------------------
albaDeviceCustom::~albaDeviceCustom()
{

}

//------------------------------------------------------------------------------
// a custom plugger for the new factory
template <class T> class albaPlugTestDevice
{
public:
  albaPlugTestDevice(const char *description);
};

//------------------------------------------------------------------------------
template <class T>
albaPlugTestDevice<T>::albaPlugTestDevice(const char *description)
{ 
  albaInteractionFactory *factory=albaInteractionFactory::GetInstance();
  if (factory)
  {
    factory->RegisterNewDevice(T::GetStaticTypeName(), description, T::NewObject);

  }
}



//-------------------------------------------------------------------------
void albaInteractionFactoryTest::TestGetDescription()
{
  albaInteractionFactory *interaction_factory = albaInteractionFactory::GetInstance();
  CPPUNIT_ASSERT(interaction_factory!=NULL);
  
  std::string value1 = interaction_factory->GetDescription();
  std::string value2 = std::string("Factory for ALBA Devices");
  
  CPPUNIT_ASSERT( value1 == value2 );
}

//-------------------------------------------------------------------------
void albaInteractionFactoryTest::TestGetInstance()
{
  albaInteractionFactory *interaction_factory = albaInteractionFactory::GetInstance();
  CPPUNIT_ASSERT(interaction_factory!=NULL);
}

//-------------------------------------------------------------------------
void albaInteractionFactoryTest::TestInitialize()
{
  albaInteractionFactory *interaction_factory = albaInteractionFactory::GetInstance();

  CPPUNIT_ASSERT(albaInteractionFactory::Initialize()==ALBA_OK);

  std::list<albaObjectFactory *> list=albaObjectFactory::GetRegisteredFactories();
  CPPUNIT_ASSERT(list.size()==1);
}

//-------------------------------------------------------------------------
void albaInteractionFactoryTest::TestRegisterNewDevice_CreateDeviceInstance()
//-------------------------------------------------------------------------
{
  albaInteractionFactory *interaction_factory = albaInteractionFactory::GetInstance();
  albaInteractionFactory::Initialize();

  albaPlugTestDevice<albaDeviceCustom>("a custom device"); // plug a pipe in the main node factory

  albaDevice *device0 = albaInteractionFactory::CreateDeviceInstance("albaDeviceNotExisting");
  albaDevice *device1 = albaInteractionFactory::CreateDeviceInstance("albaDeviceCustom");

  CPPUNIT_ASSERT(device0 == NULL);
  CPPUNIT_ASSERT(device1!=NULL);

  CPPUNIT_ASSERT(device1->IsALBAType(albaDeviceCustom));

  // cleanup factory products
  device1->Delete();

}

//-------------------------------------------------------------------------
void albaInteractionFactoryTest::TestGetNumberOfDevices()
//-------------------------------------------------------------------------
{
  albaInteractionFactory *interaction_factory = albaInteractionFactory::GetInstance();

  albaInteractionFactory::Initialize();

  albaPlugTestDevice<albaDeviceCustom>("a custom device");

  albaDevice *device0 = albaInteractionFactory::CreateDeviceInstance("albaDeviceNotExisting");
  albaDevice *device1 = albaInteractionFactory::CreateDeviceInstance("albaDeviceCustom");

  // test factory contents
  //poor proof to register again an already registered device
  albaPlugTestDevice<albaDeviceCustom>("a custom device"); 

  int numberOfDevices = interaction_factory->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 1);

  // cleanup factory products
  device1->Delete();
}

//-------------------------------------------------------------------------
void albaInteractionFactoryTest::TestGetDeviceName()
//-------------------------------------------------------------------------
{

  albaInteractionFactory *interaction_factory = albaInteractionFactory::GetInstance();

  albaInteractionFactory::Initialize();

  albaPlugTestDevice<albaDeviceCustom>("a custom device"); // plug a device in the main node factory

  albaDevice *device = albaInteractionFactory::CreateDeviceInstance("albaDeviceCustom");

  const char *name = interaction_factory->GetDeviceName(0);
  CPPUNIT_ASSERT(stricmp(name, "albaDeviceCustom") == 0);

  // cleanup factory products
  device->Delete();

}

//-------------------------------------------------------------------------
void albaInteractionFactoryTest::TestGetDeviceNames()
//-------------------------------------------------------------------------
{
  albaInteractionFactory *interaction_factory = albaInteractionFactory::GetInstance();

  albaInteractionFactory::Initialize();

  albaPlugTestDevice<albaDeviceCustom>("a custom device"); // plug a device in the main node factory

  albaDevice *device0 = albaInteractionFactory::CreateDeviceInstance("albaDeviceNotExisting");
  albaDevice *device1 = albaInteractionFactory::CreateDeviceInstance("albaDeviceCustom");

  const std::set<std::string> *devices = interaction_factory->GetDeviceNames();
  int s = devices->size();
  CPPUNIT_ASSERT(s == 1);

  bool found=false;
  if(devices->count("albaDeviceCustom") == 1)
    found = true;
  CPPUNIT_ASSERT(found);

  // cleanup factory products
  device1->Delete();
}

//-------------------------------------------------------------------------
void albaInteractionFactoryTest::TestGetDeviceDescription()
//-------------------------------------------------------------------------
{
  albaInteractionFactory *interaction_factory = albaInteractionFactory::GetInstance();

  albaInteractionFactory::Initialize();

  albaPlugTestDevice<albaDeviceCustom>("a custom device"); // plug a device in the main node factory

  albaDevice *device = albaInteractionFactory::CreateDeviceInstance("albaDeviceCustom");

  const char *description = interaction_factory->GetDeviceTypeName("albaDeviceCustom");
  CPPUNIT_ASSERT(stricmp(description, "a custom device") == 0);

  // cleanup factory products
  device->Delete();

}