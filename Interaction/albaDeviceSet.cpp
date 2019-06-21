/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceSet
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// To be included first because of wxWindows

// base
#include "albaDeviceSet.h"

// events
#include "albaEventBase.h"

// serialization
#include "albaStorageElement.h"

#include "albaMutexLock.h"
#include "mmuIdFactory.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
ALBA_ID_IMP(albaDeviceSet::DEVICE_ADD);
ALBA_ID_IMP(albaDeviceSet::DEVICE_REMOVE);
ALBA_ID_IMP(albaDeviceSet::DEVICE_ADDED);
ALBA_ID_IMP(albaDeviceSet::DEVICE_REMOVING);
//ALBA_ID_IMP(albaDeviceSet::MCH_DEVICE_SETUP);
//ALBA_ID_IMP(albaDeviceSet::MCH_DEVICE_SETDOWN);

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaDeviceSet)

//------------------------------------------------------------------------------
albaDeviceSet::albaDeviceSet()
//------------------------------------------------------------------------------
{
  m_DevicesMutex = new albaMutexLock;
}

//------------------------------------------------------------------------------
albaDeviceSet::~albaDeviceSet()
//------------------------------------------------------------------------------
{
  RemoveAllDevices(true);
  cppDEL(m_DevicesMutex);
}

//------------------------------------------------------------------------------
int albaDeviceSet::InternalInitialize()
//------------------------------------------------------------------------------
{
  Superclass::InternalInitialize();
  m_DevicesMutex->Lock();
  for (std::list<albaDevice*>::iterator it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    albaDevice *device=*it;
    assert(device);
    if (device->StartUp())
    {
      albaErrorMacro("Cannot Initilized Device: "<<device->GetName());
		  return ALBA_ERROR;
    }
  }
  m_DevicesMutex->Unlock();

  return ALBA_OK;
}

//------------------------------------------------------------------------------
void albaDeviceSet::InternalShutdown()
//------------------------------------------------------------------------------
{
  m_DevicesMutex->Lock();
  for (std::list<albaDevice*>::iterator it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    albaDevice *device=*it;
    assert(device);
    device->Stop();
  }
  m_DevicesMutex->Unlock();
  Superclass::InternalShutdown();
}

//------------------------------------------------------------------------------
int albaDeviceSet::InternalStore(albaStorageElement *node)
//------------------------------------------------------------------------------
{
  if (Superclass::InternalStore(node))
    return -1;

  m_DevicesMutex->Lock();
  for (std::list<albaDevice*>::iterator it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    albaDevice *device=*it;
    if (device->IsPersistent()) // do not store persistent devices
      continue;

    if (node->StoreObject("Device",device)==NULL)
    {
      albaErrorMacro("Error Writing "<<device->GetName()<<" device");
      m_DevicesMutex->Unlock();
		  return ALBA_ERROR;;
    }
  }
  m_DevicesMutex->Unlock();
  return ALBA_OK;
}

//------------------------------------------------------------------------------
int albaDeviceSet::InternalRestore(albaStorageElement *node)
//------------------------------------------------------------------------------
{
  assert(node);
  
  int fail=ALBA_OK;
  int old_state=IsInitialized();
  Stop();
  RemoveAllDevices();

  Superclass::InternalRestore(node);

  std::vector<albaStorageElement *> devices=node->GetChildren();
  for (int i=0;i<devices.size();i++)
  {
    // Must create the object before restoring since
    // the device must be already connected to the
    // device manager
    albaStorageElement *device_node=devices[i];  
    if (albaCString(device_node->GetName())=="Device")
    {
      if (albaObject *obj=device_node->RestoreObject())
      {
        if (albaDevice *device=albaDevice::SafeDownCast(obj)) // check the restored object is really a albaDevice
        {
          AddDevice(device);
        } 
        else
        {
          albaErrorMacro("Wrong object type, expect \"albaDevice\" found \""<<obj->GetTypeName()<<"\".");
          fail=ALBA_ERROR;
          obj->Delete(); // release memory
        }
      }
      else
      {
        albaErrorMacro("Unknown Device type, I/O parse error.");
        fail=ALBA_ERROR;
      }
      
    }
  }

  if (old_state)
    return Start();

  return fail;
}

//------------------------------------------------------------------------------
int albaDeviceSet::GetNumberOfDevices()
//------------------------------------------------------------------------------
{
  int num=m_Devices.size();
  return num;
}

//------------------------------------------------------------------------------
void albaDeviceSet::AddDevice(albaDevice *device)
//------------------------------------------------------------------------------
{
  assert (device);
  assert (device->GetName()); // all devices must have a name
  m_DevicesMutex->Lock();
  m_Devices.push_back(device);
  device->Register(this);
  device->SetListener(this);
  AddObserver(device,MCH_DOWN);
  m_DevicesMutex->Unlock();
  
  InvokeEvent(this,DEVICE_ADDED,MCH_UP,device);
}

//------------------------------------------------------------------------------
albaDevice *albaDeviceSet::GetDevice(const char *name)
//------------------------------------------------------------------------------
{
  m_DevicesMutex->Lock();
  for (std::list<albaDevice*>::iterator it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    albaDevice *device=*it;
    if (albaCString(device->GetName())==name)
    {
      m_DevicesMutex->Unlock();
      return device;
    }
  }

  m_DevicesMutex->Unlock();
  return NULL;
}
//------------------------------------------------------------------------------
albaDevice *albaDeviceSet::GetDevice(albaID id)
//------------------------------------------------------------------------------
{
  m_DevicesMutex->Lock();
  std::list<albaDevice*>::iterator it;
  for (it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    albaDevice *device=*it;
    if (device && device->GetID()==id)
    {
      m_DevicesMutex->Unlock();
      return device;
    }
  }

  for (it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    albaDevice *device=*it;
    albaDeviceSet *device_set=albaDeviceSet::SafeDownCast(device);
    if (device_set)
    {
      albaDevice *sub_device=device_set->GetDevice(id);
      if (sub_device)
      {
        m_DevicesMutex->Unlock();
        return sub_device;
      }
    }
  }

  m_DevicesMutex->Unlock();
  return NULL;
}

//------------------------------------------------------------------------------
albaDevice *albaDeviceSet::GetDeviceByIndex(int idx)
//------------------------------------------------------------------------------
{
  m_DevicesMutex->Lock();
  std::list<albaDevice*>::iterator it=m_Devices.begin();
  
  for (int i=0;i<idx;i++) it++;
  
  albaDevice *device=*it;
  m_DevicesMutex->Unlock();
  return device;
  
}

//------------------------------------------------------------------------------
int albaDeviceSet::RemoveDeviceByIndex(int idx, bool force)
//------------------------------------------------------------------------------
{
  if (albaDevice *device=GetDeviceByIndex(idx))
  {
    return RemoveDevice(device,force);
  }

  albaErrorMacro("Trying to delete an inexistent device");
  return false;
}

//------------------------------------------------------------------------------
int albaDeviceSet::RemoveDevice(albaID id, bool force)
//------------------------------------------------------------------------------
{
  if (albaDevice *device=GetDevice(id))
  {
    return RemoveDevice(device,force);
  }

  albaErrorMacro("Trying to delete an inexistent device");
  return false;
}

//------------------------------------------------------------------------------
int albaDeviceSet::RemoveDevice(albaDevice *device, bool force)
//------------------------------------------------------------------------------
{
  assert(device);
  std::list<albaDevice*>::iterator it;
  for (it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    if (device==*it)
      break;
  }
  
  if (it!=m_Devices.end())
  {
    // do not remove persistent devices if not forced
    if (device->IsPersistent()&&!force) 
      return false;
  
    InvokeEvent(this,DEVICE_REMOVING,MCH_UP,device);
    m_DevicesMutex->Lock();
    device->Stop();
    device->RemoveObserver(this);
    m_Devices.erase(it);
    device->Delete();
    m_DevicesMutex->Unlock();
    return ALBA_OK;
  }
  
  return ALBA_ERROR;
}


//------------------------------------------------------------------------------
int albaDeviceSet::RemoveDevice(const char *name, bool force)
//------------------------------------------------------------------------------
{
  albaDevice *device=GetDevice(name);

  return RemoveDevice(device,force);
}

//------------------------------------------------------------------------------ 
void albaDeviceSet::RemoveAllDevices(bool force)
//------------------------------------------------------------------------------
{
  m_DevicesMutex->Lock();

  // Remove All (non-persistent) devices
  std::list<albaDevice*>::iterator it;
  for (it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    albaDevice *device=*it;

    // do not remove persistent devices if not forced
    if (device->IsPersistent()&&!force)
      continue;

    device->Stop();
    InvokeEvent(this,DEVICE_REMOVING,MCH_UP,device);
    
    device->Delete();
    *it=NULL;
  }
  
  // remove cleaned nodes
  std::list<albaDevice*>::iterator next_it;

  for (it=m_Devices.begin();it!=m_Devices.end();it=next_it)
  {
    next_it=it;
    next_it++;

    if (*it==NULL)
      m_Devices.erase(it);
  }
  
  m_DevicesMutex->Unlock();
}

//------------------------------------------------------------------------------
void albaDeviceSet::OnEvent(albaEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event&&event->GetSender());

  int id = event->GetId();
  int channel = event->GetChannel();

  if (channel == MCH_UP)
  {
    if (id==DEVICE_ADD)
    {
      // this could create problems since there's no control on data to really be of the right type
      AddDevice((albaDevice *)event->GetData());
      return;
    }
    else if (id==DEVICE_REMOVE)
    {
      this->RemoveDevice((albaDevice *)event->GetData());
      return;
    }
  }
  
  Superclass::OnEvent(event);
}
