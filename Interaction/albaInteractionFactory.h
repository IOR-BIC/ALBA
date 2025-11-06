/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionFactory
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaInteractionFactory_h
#define __albaInteractionFactory_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaObjectFactory.h"
#include "albaConfigure.h"
#include <set>

/** to be used internally for plugging default devices --- calls a member function directly */
#define albaPlugDeviceMacro(node_type,descr) \
  RegisterNewDevice(node_type::GetStaticTypeName(), descr, node_type::NewObject);
  
/** to be used internally for plugging default avatars --- calls a member function directly */    
#define albaPlugAvatarMacro(node_type,descr) \
  RegisterNewAvatar(node_type::GetStaticTypeName(), descr, node_type::NewObject);

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------  
class albaAvatar;
class albaDevice;

/** Object factory for Devices and Avatars.
  To make a new Device or Avatar available in the ALBA it must be plugged inside a factory, in particular
  this factory must be of type albaInteractionFactory to be able to retrieve the list of devices and avatars
  plugged inside the factory. */
class ALBA_EXPORT albaInteractionFactory : public albaObjectFactory
{
public: 
  albaTypeMacro(albaInteractionFactory,albaObjectFactory);
  virtual const char* GetDescription() const;

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();
  
  /** return the instance pointer of the factory. return NULL if not initialized yet */
  static albaInteractionFactory *GetInstance();// {if (!m_Instance) Initialize(); return m_Instance;}

  /** create an instance of the node give its type name */
  static albaDevice *CreateDeviceInstance(const char *type_name);
   
  /**
    This function can be used by Application code to register new Objects's to the mflCoreFactory */
  void RegisterNewDevice(const char* node_name, const char* description, albaCreateObjectFunction createFunction);

  /** return list of names for nodes plugged into this factory */
  const static std::set<std::string> *GetDeviceNames();
  
  /** return device name */
  const char *GetDeviceName(int idx);

  /** return number of devices registered to this factory */
  int GetNumberOfDevices();
  
  /** return the comment field of the device object plugged in the factory */
  const char *GetDeviceTypeName(const char *device_name);
  
  /** create an instance of the node give its type name */
  static albaAvatar *CreateAvatarInstance(const char *type_name);
   
  /**
    This function can be used by Application code to register new Objects's to the mflCoreFactory */
  void RegisterNewAvatar(const char* node_name, const char* description, albaCreateObjectFunction createFunction);

  /** return list of names for nodes plugged into this factory */
  const static std::set<std::string> *GetAvatarNames();

  /** return number of devices registered to this factory */
  int GetNumberOfAvatars();

  /** return device name */
  const char *GetAvatarName(int idx);
  
  /** return the comment field of the avatar object plugged in the factory */
  const char *GetAvatarDescription(const char *device_name);

protected:
  albaInteractionFactory();
  ~albaInteractionFactory() { }

  static bool m_Initialized;
  //static albaInteractionFactory *m_Instance;
  static std::set<std::string> glo_DeviceNames; 
  static std::set<std::string> glo_AvatarNames; 
  
private:
  albaInteractionFactory(const albaInteractionFactory&);  // Not implemented.
  void operator=(const albaInteractionFactory&);  // Not implemented.
};

/** Plug  a node in the main ALBA Avatar factory.*/
template <class T>
class albaPlugAvatar
{
  public:
  albaPlugAvatar(const char *description);
  
};

//------------------------------------------------------------------------------
/** Plug a new Avatar class into the Avatar factory.*/
template <class T>
albaPlugAvatar<T>::albaPlugAvatar(const char *description)
//------------------------------------------------------------------------------
{ 
  albaInteractionFactory *factory=albaInteractionFactory::GetInstance();
  if (factory)
  {
    factory->RegisterNewAvatar(T::GetStaticTypeName(), description, T::NewObject);
  }
}

/** Plug  a node in the main ALBA Device factory.*/
template <class T>
class albaPlugDevice
{
  public:
  albaPlugDevice(const char *description);
  
};

//------------------------------------------------------------------------------
/** Plug a new Device class into the Device factory.*/
template <class T>
albaPlugDevice<T>::albaPlugDevice(const char *description)
//------------------------------------------------------------------------------
{ 
  albaInteractionFactory *factory=albaInteractionFactory::GetInstance();
  if (factory)
  {
    factory->RegisterNewDevice(T::GetStaticTypeName(), description, T::NewObject);
  }
}

#endif
