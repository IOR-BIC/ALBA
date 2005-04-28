/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: mafInteractionFactory.h,v $
  Language:  C++
  Date:      $Date: 2005-04-28 16:10:12 $
  Version:   $Revision: 1.1 $

=========================================================================*/
// .NAME mafInteractionFactory - 
// .SECTION Description

#ifndef __mafInteractionFactory_h
#define __mafInteractionFactory_h

#ifdef __GNUG__
    #pragma interface "mafInteractionFactory.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mflCoreFactory.h"
#include "mflDefines.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafAvatar;
class mafDevice;

/**
  This class is an object factory specialized for interaction related objects,
  in particular APIs are provided for registering and creating instances of 
  Devices and Avatars. A couple of spiecialized classes are defined to allow
  easier registration mafPlugDevice and mafPlugAvatar
*/
class mafInteractionFactory : public mflCoreFactory
{
public:
  class PIMPL;

// Methods from vtkObject
  vtkTypeMacro(mafInteractionFactory,mflCoreFactory);
  static mafInteractionFactory *New();
  void PrintSelf(ostream& os, vtkIndent indent);
  virtual const char* GetVTKSourceVersion();
  virtual const char* GetDescription();
  
  /** return pointer to mafInteractionFactory instance */
  static mafInteractionFactory *GetInstance();

  /** create a new instance of the specified avatar */
  static mafAvatar *CreateAvatarInstance(const char *name);

  /** create a new instance of the specified device */
  static mafDevice *CreateDeviceInstance(const char *name);

  /**
  This function can be used by Application code to register new Devices to the InteractioFactory */
  mafID RegisterNewDevice(const char* type, const char* name, CreateFunction createFunction);
  
  /**
  This function can be used by Application code to register new Avatars to the InteractioFactory */
  mafID RegisterNewAvatar(const char* type, const char* name, CreateFunction createFunction);
  
  /**  Return number of devices in the factory */
  int GetNumberOfDevices();

  /**  Return number of avatars in the factory */
  int GetNumberOfAvatars();

  /**  Return the name assigned to a device in the list of available devices */
  const char *GetDeviceName(mafID idx);

  /**  Return the name assigned to a device in the list of available devices */
  const char *GetDeviceName(const char *type);

  /**  Return the class type of a device in the list of available devices */
  const char *GetDeviceType(mafID idx);

  /**  Return the name assigned to an avatar in the list of available avatars */
  const char *GetAvatarName(mafID idx);

  /**  Return the name assigned to an avatar in the list of available avatars */
  const char *GetAvatarName(const char *type);

  /**  Return the class type of an avatar in the list of available avatars */
  const char *GetAvatarType(mafID idx);

  /**
    This is used to register the factory when linking as statically */
  static void Initialize();

protected:
  mafInteractionFactory();
  ~mafInteractionFactory();

  static PIMPL *Internals;

  /**
  This is used to access the interaction factory instance */
  static mafInteractionFactory * InteractionFactory;

private:
  mafInteractionFactory(const mafInteractionFactory&);  // Not implemented.
  void operator=(const mafInteractionFactory&);  // Not implemented.
};


/**
  this templated class has the only objective to register a class to the interaction factory.
  Registration occurs by calling the static member function "Plug", and the class type to be registred 
  is specified as template argument of the class, while a comment can be 
  provided as argument of the Plug function. Typical usage is mafPlugger<mafDeviceFoo>::Plug("test") */
template <class T>
class mafPlugDevice
{
public:
  mafPlugDevice(const char *name="") { \
  assert(T::IsTypeOf("mafDevice")); \
  mafInteractionFactory *iFactory=mafInteractionFactory::GetInstance(); \
  if (iFactory) \
    iFactory->RegisterNewDevice(T::GetTypeName(), name, T::NewObjectInstance); \
  }
};

/** Plug an Avatar */
template <class T>
class mafPlugAvatar
{
  public: \
  mafPlugAvatar(const char *name) \
  { \
    assert(T::IsTypeOf("mafAvatar3D"));
    mafInteractionFactory *iFactory=mafInteractionFactory::GetInstance(); \
    if (iFactory) \
      iFactory->RegisterNewAvatar(T::GetTypeName(), name, T::NewObjectInstance); \
  }
};


/** Plug generic object. To be moved to CoreFactory */
template <class T>
class mafPlugObject
{
  public: \
  mafPlugObject(const char *name) \
  { \
    mafInteractionFactory *iFactory=mafInteractionFactory::GetInstance(); \
    if (iFactory) \
      iFactory->RegisterNewObject(T::GetTypeName(), name, T::NewObjectInstance); \
  }
};

#endif
