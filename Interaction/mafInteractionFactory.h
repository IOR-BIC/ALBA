/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafInteractionFactory.h,v $
  Language:  C++
  Date:      $Date: 2005-07-13 13:53:00 $
  Version:   $Revision: 1.5 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafInteractionFactory_h
#define __mafInteractionFactory_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafObjectFactory.h"
#include "mafInteractionConfigure.h"
#include <set>

/** to be used internally for plugging default devices --- calls a member function directly */
#define mafPlugDeviceMacro(node_type,descr) \
  RegisterNewDevice(node_type::GetStaticTypeName(), descr, node_type::NewObject);
  
/** to be used internally for plugging default avatars --- calls a member function directly */    
#define mafPlugAvatarMacro(node_type,descr) \
  RegisterNewAvatar(node_type::GetStaticTypeName(), descr, node_type::NewObject);

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------  
class mafAvatar;
class mafDevice;

/** Object factory for Devices and Avatars.
  To make a new Device or Avatar available in the MAF it must be plugged inside a factory, in particular
  this factory must be of type mafInteractionFactory to be able to retrieve the list of devices and avatars
  plugged inside the factory. */
class MAF_EXPORT mafInteractionFactory : public mafObjectFactory
{
public: 
  mafTypeMacro(mafInteractionFactory,mafObjectFactory);
  virtual const char* GetMAFSourceVersion() const;
  virtual const char* GetDescription() const;

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();
  
  /** return the instance pointer of the factory. return NULL if not initialized yet */
  static mafInteractionFactory *GetInstance() {if (!m_Instance) Initialize(); return m_Instance;}

  /** create an instance of the node give its type name */
  static mafDevice *CreateDeviceInstance(const char *type_name);
   
  /**
    This function can be used by Application code to register new Objects's to the mflCoreFactory */
  void RegisterNewDevice(const char* node_name, const char* description, mafCreateObjectFunction createFunction);

  /** return list of names for nodes plugged into this factory */
  const static std::set<std::string> *GetDeviceNames();
  
  /** return device name */
  const char *GetDeviceName(int idx);

  /** return number of devices registered to this factory */
  int GetNumberOfDevices();
  
  /** return the comment field of the device object plugged in the factory */
  const char *GetDeviceDescription(const char *device_name);
  
  /** create an instance of the node give its type name */
  static mafAvatar *CreateAvatarInstance(const char *type_name);
   
  /**
    This function can be used by Application code to register new Objects's to the mflCoreFactory */
  void RegisterNewAvatar(const char* node_name, const char* description, mafCreateObjectFunction createFunction);

  /** return list of names for nodes plugged into this factory */
  const static std::set<std::string> *GetAvatarNames();

  /** return number of devices registered to this factory */
  int GetNumberOfAvatars();

  /** return device name */
  const char *GetAvatarName(int idx);
  
  /** return the comment field of the avatar object plugged in the factory */
  const char *GetAvatarDescription(const char *device_name);

protected:
  mafInteractionFactory();
  ~mafInteractionFactory() { }

  static mafInteractionFactory *m_Instance;
  static std::set<std::string> m_DeviceNames; 
  static std::set<std::string> m_AvatarNames; 
  
private:
  mafInteractionFactory(const mafInteractionFactory&);  // Not implemented.
  void operator=(const mafInteractionFactory&);  // Not implemented.
};

/** Plug  a node in the main MAF Avatar factory.*/
template <class T>
class MAF_EXPORT mafPlugAvatar
{
  public:
  mafPlugAvatar(const char *description);
  
};

//------------------------------------------------------------------------------
/** Plug a new Avatar class into the Avatar factory.*/
template <class T>
mafPlugAvatar<T>::mafPlugAvatar(const char *description)
//------------------------------------------------------------------------------
{ 
  mafInteractionFactory *factory=mafInteractionFactory::GetInstance();
  if (factory)
  {
    factory->RegisterNewAvatar(T::GetStaticTypeName(), description, T::NewObject);
  }
}

/** Plug  a node in the main MAF Device factory.*/
template <class T>
class MAF_EXPORT mafPlugDevice
{
  public:
  mafPlugDevice(const char *description);
  
};

//------------------------------------------------------------------------------
/** Plug a new Device class into the Device factory.*/
template <class T>
mafPlugDevice<T>::mafPlugDevice(const char *description)
//------------------------------------------------------------------------------
{ 
  mafInteractionFactory *factory=mafInteractionFactory::GetInstance();
  if (factory)
  {
    factory->RegisterNewDevice(T::GetStaticTypeName(), description, T::NewObject);
  }
}

#endif
