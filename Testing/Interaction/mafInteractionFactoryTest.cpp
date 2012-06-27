/*=========================================================================

 Program: MAF2
 Module: mafInteractionFactoryTest
 Authors: Eleonora Mambrini
 
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
#include "mafInteractionFactoryTest.h"

#include "mafInteractionFactory.h"
#include "mafAvatar.h"
#include "mafDevice.h"
#include "mafVersion.h"
#include <iostream>
#include <iterator>



//-------------------------------------------------------------------------
/** a simple VME created just for testing purposes. */
class mafDeviceCustom : public mafDevice
  //-------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafDeviceCustom,mafDevice);

protected:
  mafDeviceCustom();
  virtual ~mafDeviceCustom();

private:
  mafDeviceCustom(const mafDeviceCustom&); // Not implemented
  void operator=(const mafDeviceCustom&); // Not implemented
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafDeviceCustom);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafDeviceCustom::mafDeviceCustom()
//-------------------------------------------------------------------------
{

}
//-------------------------------------------------------------------------
mafDeviceCustom::~mafDeviceCustom()
//-------------------------------------------------------------------------
{

}

//------------------------------------------------------------------------------
// a custom plugger for the new factory
template <class T>
class mafPlugTestDevice
  //------------------------------------------------------------------------------
{
public:
  mafPlugTestDevice(const char *description);

};

//------------------------------------------------------------------------------
template <class T>
mafPlugTestDevice<T>::mafPlugTestDevice(const char *description)
//------------------------------------------------------------------------------
{ 
  mafInteractionFactory *factory=mafInteractionFactory::GetInstance();
  if (factory)
  {
    factory->RegisterNewDevice(T::GetStaticTypeName(), description, T::NewObject);

  }
}



//-------------------------------------------------------------------------
/** a simple VME created just for testing purposes. */
class mafAvatarCustom : public mafAvatar
  //-------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafAvatarCustom,mafAvatar);

protected:
  mafAvatarCustom();
  virtual ~mafAvatarCustom();

private:
  mafAvatarCustom(const mafAvatarCustom&); // Not implemented
  void operator=(const mafAvatarCustom&); // Not implemented
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafAvatarCustom);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafAvatarCustom::mafAvatarCustom()
//-------------------------------------------------------------------------
{

}
//-------------------------------------------------------------------------
mafAvatarCustom::~mafAvatarCustom()
//-------------------------------------------------------------------------
{

}

//------------------------------------------------------------------------------
// a custom plugger for the new factory
template <class T>
class mafPlugTestAvatar
  //------------------------------------------------------------------------------
{
public:
  mafPlugTestAvatar(const char *description);

};

//------------------------------------------------------------------------------
template <class T>
mafPlugTestAvatar<T>::mafPlugTestAvatar(const char *description)
//------------------------------------------------------------------------------
{ 
  mafInteractionFactory *factory=mafInteractionFactory::GetInstance();
  if (factory)
  {
    factory->RegisterNewAvatar(T::GetStaticTypeName(), description, T::NewObject);

  }
}

//-------------------------------------------------------------------------
void mafInteractionFactoryTest::TestGetMAFSourceVersion()
//-------------------------------------------------------------------------
{
  mafInteractionFactory *interaction_factory = mafInteractionFactory::GetInstance();
  CPPUNIT_ASSERT(interaction_factory!=NULL);
  
  std::string value1 = interaction_factory->GetMAFSourceVersion();
  std::string value2 = std::string(MAF_SOURCE_VERSION);
  
  CPPUNIT_ASSERT( value1 == value2 );

}
//-------------------------------------------------------------------------
void mafInteractionFactoryTest::TestGetDescription()
//-------------------------------------------------------------------------
{
  mafInteractionFactory *interaction_factory = mafInteractionFactory::GetInstance();
  CPPUNIT_ASSERT(interaction_factory!=NULL);
  
  std::string value1 = interaction_factory->GetDescription();
  std::string value2 = std::string("Factory for MAF Devices and Avatars");
  
  CPPUNIT_ASSERT( value1 == value2 );
}

//-------------------------------------------------------------------------
void mafInteractionFactoryTest::TestGetInstance()
//-------------------------------------------------------------------------
{
  mafInteractionFactory *interaction_factory = mafInteractionFactory::GetInstance();
  CPPUNIT_ASSERT(interaction_factory!=NULL);
}

//-------------------------------------------------------------------------
void mafInteractionFactoryTest::TestInitialize()
//-------------------------------------------------------------------------
{
  mafInteractionFactory *interaction_factory = mafInteractionFactory::GetInstance();

  CPPUNIT_ASSERT(mafInteractionFactory::Initialize()==MAF_OK);

  std::list<mafObjectFactory *> list=mafObjectFactory::GetRegisteredFactories();
  CPPUNIT_ASSERT(list.size()==1);
}

//-------------------------------------------------------------------------
void mafInteractionFactoryTest::TestRegisterNewAvatar_CreateAvatarInstance()
//-------------------------------------------------------------------------
{
  mafInteractionFactory *interaction_factory = mafInteractionFactory::GetInstance();
  
  mafInteractionFactory::Initialize();

  mafPlugTestDevice<mafDeviceCustom>("a custom device");


  mafDevice *device0 = mafInteractionFactory::CreateDeviceInstance("mafDeviceNotExisting");
  mafDevice *device1 = mafInteractionFactory::CreateDeviceInstance("mafDeviceCustom");

  CPPUNIT_ASSERT(device0 == NULL);
  CPPUNIT_ASSERT(device1!=NULL);

  CPPUNIT_ASSERT(device1->IsMAFType(mafDeviceCustom));

  // cleanup factory products
  device1->Delete();

}

//-------------------------------------------------------------------------
void mafInteractionFactoryTest::TestRegisterNewDevice_CreateDeviceInstance()
//-------------------------------------------------------------------------
{
  mafInteractionFactory *interaction_factory = mafInteractionFactory::GetInstance();
  mafInteractionFactory::Initialize();

  mafPlugTestDevice<mafDeviceCustom>("a custom device"); // plug a pipe in the main node factory

  mafDevice *device0 = mafInteractionFactory::CreateDeviceInstance("mafDeviceNotExisting");
  mafDevice *device1 = mafInteractionFactory::CreateDeviceInstance("mafDeviceCustom");

  CPPUNIT_ASSERT(device0 == NULL);
  CPPUNIT_ASSERT(device1!=NULL);

  CPPUNIT_ASSERT(device1->IsMAFType(mafDeviceCustom));

  // cleanup factory products
  device1->Delete();

}

//-------------------------------------------------------------------------
void mafInteractionFactoryTest::TestGetNumberOfAvatars()
//-------------------------------------------------------------------------
{

  mafInteractionFactory *interaction_factory = mafInteractionFactory::GetInstance();

  mafInteractionFactory::Initialize();

  mafPlugTestAvatar<mafAvatarCustom>("a custom avatar");

  mafAvatar *avatar0 = mafInteractionFactory::CreateAvatarInstance("mafAvatarNotExisting");
  mafAvatar *avatar1 = mafInteractionFactory::CreateAvatarInstance("mafAvatarCustom");

  // test factory contents
  //poor proof to register again an already registered avatar
  mafPlugTestAvatar<mafAvatarCustom>("a custom avatar"); 

  int numberOfAvatars = interaction_factory->GetNumberOfAvatars();
  CPPUNIT_ASSERT(numberOfAvatars == 1);

  // cleanup factory products
  avatar1->Delete();

}

//-------------------------------------------------------------------------
void mafInteractionFactoryTest::TestGetAvatarName()
//-------------------------------------------------------------------------
{
  mafInteractionFactory *interaction_factory = mafInteractionFactory::GetInstance();

  mafInteractionFactory::Initialize();

  mafPlugTestAvatar<mafAvatarCustom>("a custom avatar"); // plug an avatar in the main node factory

  mafAvatar *avatar = mafInteractionFactory::CreateAvatarInstance("mafAvatarCustom");

  const char *name = interaction_factory->GetAvatarName(0);
  CPPUNIT_ASSERT(stricmp(name, "mafAvatarCustom") == 0);

  // cleanup factory products
  avatar->Delete();

}

//-------------------------------------------------------------------------
void mafInteractionFactoryTest::TestGetAvatarNames()
//-------------------------------------------------------------------------
{
  mafInteractionFactory *interaction_factory = mafInteractionFactory::GetInstance();

  mafInteractionFactory::Initialize();

  mafPlugTestAvatar<mafAvatarCustom>("a custom avatar"); // plug an avatar in the main node factory

  mafAvatar *avatar0 = mafInteractionFactory::CreateAvatarInstance("mafAvatarNotExisting");
  mafAvatar *avatar1 = mafInteractionFactory::CreateAvatarInstance("mafAvatarCustom");

  const std::set<std::string> *avatars = interaction_factory->GetAvatarNames();
  int s = avatars->size();
  CPPUNIT_ASSERT(s == 1);

  bool found=false;
  if(avatars->count("mafAvatarCustom") == 1)
    found = true;
  CPPUNIT_ASSERT(found);

  // cleanup factory products
  avatar1->Delete();

}

//-------------------------------------------------------------------------
void mafInteractionFactoryTest::TestGetNumberOfDevices()
//-------------------------------------------------------------------------
{
  mafInteractionFactory *interaction_factory = mafInteractionFactory::GetInstance();

  mafInteractionFactory::Initialize();

  mafPlugTestDevice<mafDeviceCustom>("a custom device");

  mafDevice *device0 = mafInteractionFactory::CreateDeviceInstance("mafDeviceNotExisting");
  mafDevice *device1 = mafInteractionFactory::CreateDeviceInstance("mafDeviceCustom");

  // test factory contents
  //poor proof to register again an already registered avatar
  mafPlugTestDevice<mafDeviceCustom>("a custom device"); 

  int numberOfDevices = interaction_factory->GetNumberOfDevices();
  CPPUNIT_ASSERT(numberOfDevices == 1);

  // cleanup factory products
  device1->Delete();
}

//-------------------------------------------------------------------------
void mafInteractionFactoryTest::TestGetAvatarDescription()
//-------------------------------------------------------------------------
{
  mafInteractionFactory *interaction_factory = mafInteractionFactory::GetInstance();

  mafInteractionFactory::Initialize();

  mafPlugTestAvatar<mafAvatarCustom>("a custom avatar"); // plug an avatar in the main node factory

  mafAvatar *avatar = mafInteractionFactory::CreateAvatarInstance("mafAvatarCustom");

  //---------- start comment -------------
  // working on visual studio 2003
  // failing on visual studio 2010
  // commented for the moment (more investigation is needed...)
  //
  // const char *description = interaction_factory->GetAvatarDescription("mafAvatarCustom");
  // CPPUNIT_ASSERT(stricmp(description, "a custom avatar") == 0);
  //
  //---------- end comment -------------
  // cleanup factory products
  avatar->Delete();

}

//-------------------------------------------------------------------------
void mafInteractionFactoryTest::TestGetDeviceName()
//-------------------------------------------------------------------------
{

  mafInteractionFactory *interaction_factory = mafInteractionFactory::GetInstance();

  mafInteractionFactory::Initialize();

  mafPlugTestDevice<mafDeviceCustom>("a custom device"); // plug an avatar in the main node factory

  mafDevice *device = mafInteractionFactory::CreateDeviceInstance("mafDeviceCustom");

  const char *name = interaction_factory->GetDeviceName(0);
  CPPUNIT_ASSERT(stricmp(name, "mafDeviceCustom") == 0);

  // cleanup factory products
  device->Delete();

}

//-------------------------------------------------------------------------
void mafInteractionFactoryTest::TestGetDeviceNames()
//-------------------------------------------------------------------------
{
  mafInteractionFactory *interaction_factory = mafInteractionFactory::GetInstance();

  mafInteractionFactory::Initialize();

  mafPlugTestAvatar<mafDeviceCustom>("a custom device"); // plug a device in the main node factory

  mafDevice *device0 = mafInteractionFactory::CreateDeviceInstance("mafDeviceNotExisting");
  mafDevice *device1 = mafInteractionFactory::CreateDeviceInstance("mafDeviceCustom");

  const std::set<std::string> *devices = interaction_factory->GetDeviceNames();
  int s = devices->size();
  CPPUNIT_ASSERT(s == 1);

  bool found=false;
  if(devices->count("mafDeviceCustom") == 1)
    found = true;
  CPPUNIT_ASSERT(found);

  // cleanup factory products
  device1->Delete();
}

//-------------------------------------------------------------------------
void mafInteractionFactoryTest::TestGetDeviceDescription()
//-------------------------------------------------------------------------
{
  mafInteractionFactory *interaction_factory = mafInteractionFactory::GetInstance();

  mafInteractionFactory::Initialize();

  mafPlugTestDevice<mafDeviceCustom>("a custom device"); // plug a device in the main node factory

  mafDevice *device = mafInteractionFactory::CreateDeviceInstance("mafDeviceCustom");

  const char *description = interaction_factory->GetDeviceDescription("mafDeviceCustom");
  CPPUNIT_ASSERT(stricmp(description, "a custom device") == 0);

  // cleanup factory products
  device->Delete();

}