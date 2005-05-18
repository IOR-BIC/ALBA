/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDeviceSet.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-18 17:29:04 $
  Version:   $Revision: 1.5 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows

// base
#include "mafDeviceSet.h"

// events
#include "mafEventBase.h"

// serialization
#include "mafStorageElement.h"

#include "mafMutexLock.h"
#include "mmuIdFactory.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafDeviceSet::DEVICE_ADD);
MAF_ID_IMP(mafDeviceSet::DEVICE_REMOVE);
MAF_ID_IMP(mafDeviceSet::DEVICE_ADDED);
MAF_ID_IMP(mafDeviceSet::DEVICE_REMOVING);
//MAF_ID_IMP(mafDeviceSet::MCH_DEVICE_SETUP);
//MAF_ID_IMP(mafDeviceSet::MCH_DEVICE_SETDOWN);

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafDeviceSet)

//------------------------------------------------------------------------------
mafDeviceSet::mafDeviceSet()
//------------------------------------------------------------------------------
{
  m_DevicesMutex = new mafMutexLock;
}

//------------------------------------------------------------------------------
mafDeviceSet::~mafDeviceSet()
//------------------------------------------------------------------------------
{
  RemoveAllDevices(true);
  cppDEL(m_DevicesMutex);
}

//------------------------------------------------------------------------------
int mafDeviceSet::InternalInitialize()
//------------------------------------------------------------------------------
{
  Superclass::InternalInitialize();
  m_DevicesMutex->Lock();
  for (std::list<mafDevice*>::iterator it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    mafDevice *device=*it;
    assert(device);
    if (device->StartUp())
    {
      mafErrorMacro("Cannot Initilized Device: "<<device->GetName());
		  return MAF_ERROR;
    }
  }
  m_DevicesMutex->Unlock();

  return MAF_OK;
}

//------------------------------------------------------------------------------
void mafDeviceSet::InternalShutdown()
//------------------------------------------------------------------------------
{
  m_DevicesMutex->Lock();
  for (std::list<mafDevice*>::iterator it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    mafDevice *device=*it;
    assert(device);
    device->Stop();
  }
  m_DevicesMutex->Unlock();
  Superclass::InternalShutdown();
}

//------------------------------------------------------------------------------
int mafDeviceSet::InternalStore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  if (Superclass::InternalStore(node))
    return -1;

  m_DevicesMutex->Lock();
  for (std::list<mafDevice*>::iterator it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    mafDevice *device=*it;
    if (device->IsPersistent()) // do not store persistent devices
      continue;

    if (node->StoreObject("Device",device))
    {
      mafErrorMacro("Error Writing "<<device->GetName()<<" device");
      m_DevicesMutex->Unlock();
		  return MAF_ERROR;;
    }
  }
  m_DevicesMutex->Unlock();
  return MAF_OK;
}

//------------------------------------------------------------------------------
int mafDeviceSet::InternalRestore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  assert(node);
  
  int fail=MAF_OK;
  int old_state=IsInitialized();
  Stop();
  RemoveAllDevices();

  Superclass::InternalRestore(node);

  std::vector<mafStorageElement *> devices=node->GetChildren();
  for (int i=0;i<devices.size();i++)
  {
    // Must create the object before restoring since
    // the device must be already connected to the
    // device manager
    mafStorageElement *device_node=devices[i];  
    if (mafCString(device_node->GetName())=="Device")
    {
      if (mafObject *obj=device_node->RestoreObject())
      {
        if (mafDevice *device=mafDevice::SafeDownCast(obj)) // check the restored object is really a mafDevice
        {
          AddDevice(device);
        } 
        else
        {
          mafErrorMacro("Wrong object type, expect \"mafDevice\" found \""<<obj->GetTypeName()<<"\".");
          fail=MAF_ERROR;
          obj->Delete(); // release memory
        }
      }
      else
      {
        mafErrorMacro("Unknown Device type, I/O parse error.");
        fail=MAF_ERROR;
      }
      
    }
  }

  if (old_state)
    return Start();

  return fail;
}

//------------------------------------------------------------------------------
int mafDeviceSet::GetNumberOfDevices()
//------------------------------------------------------------------------------
{
  int num=m_Devices.size();
  return num;
}

//------------------------------------------------------------------------------
void mafDeviceSet::AddDevice(mafDevice *device)
//------------------------------------------------------------------------------
{
  assert (device);
  assert (device->GetName()); // all devices must have a name
  m_DevicesMutex->Lock();
  m_Devices.push_back(device);
  device->SetListener(this);
  device->PlugEventSource(this,MCH_DOWN);
  m_DevicesMutex->Unlock();
  
  InvokeEvent(DEVICE_ADDED,MCH_UP,device);
}

//------------------------------------------------------------------------------
mafDevice *mafDeviceSet::GetDevice(const char *name)
//------------------------------------------------------------------------------
{
  m_DevicesMutex->Lock();
  for (std::list<mafDevice*>::iterator it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    mafDevice *device=*it;
    if (mafCString(device->GetName())==name)
    {
      m_DevicesMutex->Unlock();
      return device;
    }
  }

  m_DevicesMutex->Unlock();
  return NULL;
}
//------------------------------------------------------------------------------
mafDevice *mafDeviceSet::GetDevice(mafID id)
//------------------------------------------------------------------------------
{
  m_DevicesMutex->Lock();
  std::list<mafDevice*>::iterator it;
  for (it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    mafDevice *device=*it;
    if (device->GetID()==id)
    {
      m_DevicesMutex->Unlock();
      return device;
    }
  }

  for (it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    mafDevice *device=*it;
    mafDeviceSet *device_set=mafDeviceSet::SafeDownCast(device);
    if (device_set)
    {
      mafDevice *sub_device=device_set->GetDevice(id);
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
mafDevice *mafDeviceSet::GetDeviceByIndex(int idx)
//------------------------------------------------------------------------------
{
  m_DevicesMutex->Lock();
  std::list<mafDevice*>::iterator it=m_Devices.begin();
  
  for (int i=0;i<idx;i++) it++;
  
  mafDevice *device=*it;
  m_DevicesMutex->Unlock();
  return device;
  
}

//------------------------------------------------------------------------------
int mafDeviceSet::RemoveDeviceByIndex(int idx, bool force)
//------------------------------------------------------------------------------
{
  if (mafDevice *device=GetDeviceByIndex(idx))
  {
    return RemoveDevice(device,force);
  }

  mafErrorMacro("Trying to delete an inexistent device");
  return false;
}

//------------------------------------------------------------------------------
int mafDeviceSet::RemoveDevice(mafID id, bool force)
//------------------------------------------------------------------------------
{
  if (mafDevice *device=GetDevice(id))
  {
    return RemoveDevice(device,force);
  }

  mafErrorMacro("Trying to delete an inexistent device");
  return false;
}

//------------------------------------------------------------------------------
int mafDeviceSet::RemoveDevice(mafDevice *device, bool force)
//------------------------------------------------------------------------------
{
  assert(device);
  std::list<mafDevice*>::iterator it;
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
  
    InvokeEvent(DEVICE_REMOVING,MCH_UP,device);
    m_DevicesMutex->Lock();
    device->Stop();
    device->RemoveObserver(this);
    m_Devices.erase(it);
    device->Delete();
    m_DevicesMutex->Unlock();
    return MAF_OK;
  }
  
  return MAF_ERROR;
}


//------------------------------------------------------------------------------
int mafDeviceSet::RemoveDevice(const char *name, bool force)
//------------------------------------------------------------------------------
{
  mafDevice *device=GetDevice(name);

  return RemoveDevice(device,force);
}

//------------------------------------------------------------------------------ 
void mafDeviceSet::RemoveAllDevices(bool force)
//------------------------------------------------------------------------------
{
  m_DevicesMutex->Lock();

  // Remove All (non-persistent) devices
  for (std::list<mafDevice*>::iterator it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    mafDevice *device=*it;
    //it++;
    //mafDevice *next_device=*it;

    // do not remove persistent devices if not forced
    if (device->IsPersistent()&&!force)
      continue;

    device->Stop();
    InvokeEvent(DEVICE_REMOVING,MCH_UP,device);
    
    device->Delete();
  }
  
  m_Devices.clear();

  m_DevicesMutex->Unlock();
}

//------------------------------------------------------------------------------
void mafDeviceSet::OnEvent(mafEventBase *event)
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
      AddDevice((mafDevice *)event->GetData());
      return;
    }
    else if (id==DEVICE_REMOVE)
    {
      this->RemoveDevice((mafDevice *)event->GetData());
      return;
    }
  }
  
  Superclass::OnEvent(event);
}
