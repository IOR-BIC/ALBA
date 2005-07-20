/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDeviceManager.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-20 15:49:48 $
  Version:   $Revision: 1.8 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

// base includes
#include "mafDeviceManager.h"

// MFL events
#include "mafEventBase.h"

// local 
#include "mafDeviceSet.h"
#include "mmuIdFactory.h"
#include "mafInteractionFactory.h"

// serialization
#include "mafStorageElement.h"
#include "mafStorage.h"

// VTK local

#include <assert.h>

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafDeviceManager::DISPATCH_START);
MAF_ID_IMP(mafDeviceManager::DISPATCH_END);

//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafDeviceManager)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafDeviceManager::mafDeviceManager()
//------------------------------------------------------------------------------
{
  m_RestoringFlag   = false;
  m_DeviceIdCounter = mafDevice::MIN_DEVICE_ID;
  m_PersistentDeviceIdCounter = 0;
  vtkNEW(m_DeviceSet);
  m_DeviceSet->SetPersistentFlag(false);
  m_DeviceSet->SetThreaded(false);
  m_DeviceSet->SetName("DeviceSet");
  m_DeviceSet->AutoStartOn();
  m_DeviceSet->SetListener(this);
  m_DeviceSet->SetID(++m_DeviceIdCounter); // ID of the root device
}

//------------------------------------------------------------------------------
mafDeviceManager::~mafDeviceManager()
//------------------------------------------------------------------------------
{
  mafDEL(m_DeviceSet)
}

//------------------------------------------------------------------------------
int mafDeviceManager::InternalInitialize()
//------------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize())
    return MAF_ERROR;

  // startup all devices
  return m_DeviceSet->StartUp();
}

//------------------------------------------------------------------------------
void mafDeviceManager::InternalShutdown()
//------------------------------------------------------------------------------
{
  m_DeviceSet->Stop();
  Superclass::InternalShutdown();
  mafYield();
}

//------------------------------------------------------------------------------
mafID mafDeviceManager::AddDevice(mafDevice *device)
//------------------------------------------------------------------------------
{
  m_DeviceSet->AddDevice(device);
  return device->GetID();
}

//------------------------------------------------------------------------------
mafDevice *mafDeviceManager::AddDevice(const char *type, bool persistent)
//------------------------------------------------------------------------------
{
  mafDevice *device = mafInteractionFactory::CreateDeviceInstance(type);
  
  if (device)
  {
    device->SetPersistentFlag(persistent); // set persistent flag

    
    mafInteractionFactory *iFactory = mafInteractionFactory::GetInstance();
    const char *dev_name=iFactory->GetDeviceDescription(type);
    assert(dev_name);

    mafString base_name=dev_name;
    mafString instance_name = base_name;

    for (int id=1;true;id++)
    {
      
      // if no device with the same name exists break!
      if (m_DeviceSet->GetDevice(instance_name)==NULL)
        break;

      // else append an numeric postfix
      instance_name=base_name+" ("+mafString(id)+")";
    }
    
    device->SetName(instance_name);
    
    mafDevice *ret = (AddDevice(device)>0)?device:NULL;

    //device->Delete();

    return ret;
  }

  return device;
}

//------------------------------------------------------------------------------
int mafDeviceManager::RemoveDevice(mafDevice *device, bool force)
//------------------------------------------------------------------------------
{
  return m_DeviceSet->RemoveDevice(device,force);
}

//------------------------------------------------------------------------------
int mafDeviceManager::RemoveDevice(const char *name, bool force)
//------------------------------------------------------------------------------
{
  return m_DeviceSet->RemoveDevice(name,force);
}

//------------------------------------------------------------------------------
mafDevice *mafDeviceManager::GetDevice(const char *name)
//------------------------------------------------------------------------------
{
  return m_DeviceSet->GetDevice(name);
}

//------------------------------------------------------------------------------
mafDevice *mafDeviceManager::GetDevice(mafID id)
//------------------------------------------------------------------------------
{
  return m_DeviceSet->GetDevice(id);
}
//------------------------------------------------------------------------------
std::list<mafDevice *> *mafDeviceManager::GetDevices()
//------------------------------------------------------------------------------
{
  return m_DeviceSet->GetDevices();
}

//------------------------------------------------------------------------------
int mafDeviceManager::GetNumberOfDevices()
{
  return m_DeviceSet->GetNumberOfDevices();
}

//------------------------------------------------------------------------------
void mafDeviceManager::RemoveAllDevices(bool force)
//------------------------------------------------------------------------------
{
  m_DeviceSet->RemoveAllDevices(force);
}

//----------------------------------------------------------------------------
int mafDeviceManager::InternalStore(mafStorageElement *node)
//----------------------------------------------------------------------------
{
  assert(node);
  node->SetAttribute("DeviceIdCounter",m_DeviceIdCounter);
  
  if (node->StoreObject("DeviceSet",m_DeviceSet)==NULL)
    return MAF_ERROR;

  return MAF_OK;
}

//----------------------------------------------------------------------------
int mafDeviceManager::InternalRestore(mafStorageElement *node)
//----------------------------------------------------------------------------
{
  assert(node);
  m_RestoringFlag=true; // used to avoid DeviceManager set device ID when restoring
  
  if (!node->GetAttributeAsInteger("DeviceIdCounter",m_DeviceIdCounter))
  {
    assert(true);
    mafErrorMacro("Cannot find \"DeviceIdCounter\" attribute, possible subsequent restoring problems!");
    m_DeviceIdCounter = mafDevice::MIN_DEVICE_ID;
  }
  
  int fail = node->RestoreObject("DeviceSet",m_DeviceSet);
  
  m_RestoringFlag=false;

  return fail;
}

//------------------------------------------------------------------------------
bool mafDeviceManager::DispatchEvents()
//------------------------------------------------------------------------------
{
  if (m_Dispatched)
    return false;

  InvokeEvent(DISPATCH_START); // Advise dispatching is started 
  bool ret=Superclass::DispatchEvents();
  InvokeEvent(DISPATCH_END); // Advise dispatching is finished 

  return ret;
}

//------------------------------------------------------------------------------
void mafDeviceManager::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event&&event->GetSender());

  int id=event->GetId();

  if (id==EVENT_DISPATCH)
  {
    // Push the event to the queue. The DispatchEvent() of this
    // function will automatically call DispatchEvents() of the
    // sender. Note DispatchEvent() of this function is called in 
    // the wxWindows message pump thread. This way this object acts 
    // the synchronization of all the events coming from different sources.
    PushEvent(event);
    return;
  }
  else if (id==mafDeviceSet::DEVICE_ADDED )
  {
    mafDevice *device=(mafDevice *)event->GetData();
    if (!m_RestoringFlag)
    {
      if (device->IsPersistent())
      {
        device->SetID(++m_PersistentDeviceIdCounter);
      }
      else
      {
        device->SetID(++m_DeviceIdCounter);
      }
    }
    InvokeEvent(event); // send also to InteractionManager
    return;
  }
 
  Superclass::OnEvent(event);
}
