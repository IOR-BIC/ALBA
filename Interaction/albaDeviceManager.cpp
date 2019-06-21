/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceManager
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// base includes
#include "albaDeviceManager.h"

// MFL events
#include "albaEventBase.h"

// local 
#include "albaDeviceSet.h"
#include "mmuIdFactory.h"
#include "albaInteractionFactory.h"

// serialization
#include "albaStorageElement.h"
#include "albaStorage.h"

// VTK local

#include <assert.h>

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
ALBA_ID_IMP(albaDeviceManager::DISPATCH_START);
ALBA_ID_IMP(albaDeviceManager::DISPATCH_END);

//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaDeviceManager)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaDeviceManager::albaDeviceManager()
//------------------------------------------------------------------------------
{
  m_RestoringFlag   = false;
  m_DeviceIdCounter = albaDevice::MIN_DEVICE_ID;
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
albaDeviceManager::~albaDeviceManager()
//------------------------------------------------------------------------------
{
  m_Listener = NULL;
  albaDEL(m_DeviceSet);
}

//------------------------------------------------------------------------------
int albaDeviceManager::InternalInitialize()
//------------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize())
    return ALBA_ERROR;

  // startup all devices
  return m_DeviceSet->StartUp();
}

//------------------------------------------------------------------------------
void albaDeviceManager::InternalShutdown()
//------------------------------------------------------------------------------
{
  m_DeviceSet->Stop();
  Superclass::InternalShutdown();
  albaYield();
}

//------------------------------------------------------------------------------
albaID albaDeviceManager::AddDevice(albaDevice *device)
//------------------------------------------------------------------------------
{
  m_DeviceSet->AddDevice(device);
  return device->GetID();
}

//------------------------------------------------------------------------------
albaDevice *albaDeviceManager::AddDevice(const char *type, bool persistent)
//------------------------------------------------------------------------------
{
  albaDevice *device = albaInteractionFactory::CreateDeviceInstance(type);
  
  if (device)
  {
    device->SetPersistentFlag(persistent); // set persistent flag

    
    albaInteractionFactory *iFactory = albaInteractionFactory::GetInstance();
    const char *dev_name=iFactory->GetDeviceTypeName(type);
    assert(dev_name);

    albaString base_name=dev_name;
    albaString instance_name = base_name;

    for (int id=1;true;id++)
    {
      
      // if no device with the same name exists break!
      if (m_DeviceSet->GetDevice(instance_name)==NULL)
        break;

      // else append an numeric postfix
      instance_name=base_name+" ("+albaString(id)+")";
    }
    
    device->SetName(instance_name);
    
    albaDevice *ret = (AddDevice(device)>0)?device:NULL;

    //device->Delete();

    return ret;
  }

  return device;
}

//------------------------------------------------------------------------------
int albaDeviceManager::RemoveDevice(albaDevice *device, bool force)
//------------------------------------------------------------------------------
{
  return m_DeviceSet->RemoveDevice(device,force);
}

//------------------------------------------------------------------------------
int albaDeviceManager::RemoveDevice(const char *name, bool force)
//------------------------------------------------------------------------------
{
  return m_DeviceSet->RemoveDevice(name,force);
}

//------------------------------------------------------------------------------
albaDevice *albaDeviceManager::GetDevice(const char *name)
//------------------------------------------------------------------------------
{
  return m_DeviceSet->GetDevice(name);
}

//------------------------------------------------------------------------------
albaDevice *albaDeviceManager::GetDevice(albaID id)
//------------------------------------------------------------------------------
{
  if (id==m_DeviceSet->GetID())
    return m_DeviceSet;
  
  return m_DeviceSet->GetDevice(id);
}
//------------------------------------------------------------------------------
std::list<albaDevice *> *albaDeviceManager::GetDevices()
//------------------------------------------------------------------------------
{
  return m_DeviceSet->GetDevices();
}

//------------------------------------------------------------------------------
int albaDeviceManager::GetNumberOfDevices()
{
  return m_DeviceSet->GetNumberOfDevices();
}

//------------------------------------------------------------------------------
void albaDeviceManager::RemoveAllDevices(bool force)
//------------------------------------------------------------------------------
{
  m_DeviceSet->RemoveAllDevices(force);
}

//----------------------------------------------------------------------------
int albaDeviceManager::InternalStore(albaStorageElement *node)
//----------------------------------------------------------------------------
{
  assert(node);
  node->SetAttribute("DeviceIdCounter",m_DeviceIdCounter);
  
  if (node->StoreObject("DeviceSet",m_DeviceSet)==NULL)
    return ALBA_ERROR;

  return ALBA_OK;
}

//----------------------------------------------------------------------------
int albaDeviceManager::InternalRestore(albaStorageElement *node)
//----------------------------------------------------------------------------
{
  assert(node);
  m_RestoringFlag=true; // used to avoid DeviceManager set device ID when restoring
  
  if (!node->GetAttributeAsInteger("DeviceIdCounter",m_DeviceIdCounter))
  {
    assert(true);
    albaErrorMacro("Cannot find \"DeviceIdCounter\" attribute, possible subsequent restoring problems!");
    m_DeviceIdCounter = albaDevice::MIN_DEVICE_ID;
  }
  
  int fail = node->RestoreObject("DeviceSet",m_DeviceSet);
  
  m_RestoringFlag=false;

  return fail;
}

//------------------------------------------------------------------------------
bool albaDeviceManager::DispatchEvents()
//------------------------------------------------------------------------------
{
  if (m_Dispatched)
    return false;

  InvokeEvent(this,DISPATCH_START); // Advise dispatching is started 
  bool ret=Superclass::DispatchEvents();
  InvokeEvent(this,DISPATCH_END); // Advise dispatching is finished 

  return ret;
}

//------------------------------------------------------------------------------
void albaDeviceManager::OnEvent(albaEventBase *event)
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
  else if (id==albaDeviceSet::DEVICE_ADDED )
  {
    albaDevice *device=(albaDevice *)event->GetData();
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
