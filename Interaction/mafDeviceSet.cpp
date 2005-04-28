/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDeviceSet.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-28 16:10:11 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows
#ifdef __GNUG__
    #pragma implementation "mafDeviceSet.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

// base
#include "mafDeviceSet.h"

// factory
#include "mafInteractionFactory.h"

// events
#include "mflEvent.h"

// serialization
#include "mflXMLWriter.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLDataParser.h" 
#include "mafAttribute.h"

// VTK locals
#include "vtkTemplatedList.txx"
#include "vtkString.h"
#include <assert.h>

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MFL_EVT_IMP(mafDeviceSet::AddDeviceEvent);
MFL_EVT_IMP(mafDeviceSet::RemoveDeviceEvent);
MFL_EVT_IMP(mafDeviceSet::DeviceAddedEvent);
MFL_EVT_IMP(mafDeviceSet::DeviceRemovingEvent);
MFL_EVT_IMP(mafDeviceSet::DeviceSetUpChannel);
MFL_EVT_IMP(mafDeviceSet::DeviceSetDownChannel);

//------------------------------------------------------------------------------
vtkStandardNewMacro(mafDeviceSet)

//------------------------------------------------------------------------------
mafDeviceSet::mafDeviceSet()
//------------------------------------------------------------------------------
{
  vtkNEW(DevicesMutex);
  vtkNEW(Devices);
}

//------------------------------------------------------------------------------
mafDeviceSet::~mafDeviceSet()
//------------------------------------------------------------------------------
{
  RemoveAllDevices(true);
  vtkDEL(Devices);
  vtkDEL(DevicesMutex);
}

//------------------------------------------------------------------------------
int mafDeviceSet::InternalInitialize()
//------------------------------------------------------------------------------
{
  Superclass::InternalInitialize();
  DevicesMutex->Lock();
  for (mafDevice *device=Devices->InitTraversal();device;device=Devices->GetNextItem())
  {
    if (device->StartUp())
    {
      vtkErrorMacro("Cannot Initilized Device: "<<device->GetName());
		  return -1;
    }
  }
  DevicesMutex->Unlock();

  return 0;
}

//------------------------------------------------------------------------------
void mafDeviceSet::InternalShutdown()
//------------------------------------------------------------------------------
{
  DevicesMutex->Lock();
  for (mafDevice *device=Devices->InitTraversal();device;device=Devices->GetNextItem())
  {
    device->Stop();
  }
  DevicesMutex->Unlock();
  Superclass::InternalShutdown();
}

//------------------------------------------------------------------------------
int mafDeviceSet::InternalStore(mflXMLWriter *writer)
//------------------------------------------------------------------------------
{
  if (Superclass::InternalStore(writer))
    return -1;

  DevicesMutex->Lock();
  for (mafDevice *device=Devices->InitTraversal();device;device=Devices->GetNextItem())
  {
    if (device->IsPersistent()) // do not store persistent devices
      continue;

    if (device->Store(writer))
    {
      vtkErrorMacro("Error Writing "<<device->GetName()<<" device");
      DevicesMutex->Unlock();
		  return -1;
    }
  }
  DevicesMutex->Unlock();
  return 0;
}

//------------------------------------------------------------------------------
int mafDeviceSet::InternalRestore(vtkXMLDataElement *node,vtkXMLDataParser *parser)
//------------------------------------------------------------------------------
{
  assert(node&&parser);
  
  int fail=0;
  int old_state=IsInitialized();
  Stop();
  RemoveAllDevices();

  Superclass::InternalRestore(node,parser);

  for (int i=0;i<node->GetNumberOfNestedElements();i++)
  {
    // Must create the object before restoring since
    // the device must be already connected to the
    // device manager
    vtkXMLDataElement *device_node=node->GetNestedElement(i);
    if (mflString(device_node->GetName())=="Device")
    {
      if (const char *device_name=device_node->GetAttribute("Type"))
      {
        if (mafDevice *device=mafInteractionFactory::CreateDeviceInstance(device_name))
        {
          // Must set the device ID before adding to the device tree
          // since device tree identifies device by ID
          int id;
          if (!mafAttribute::RestoreNumeric(device_node,parser,id,"ID"))
          {
            device->SetID(id+MIN_DEVICE_ID);
            device->SetName("Dummy");
            AddDevice(device);
            fail=device->Restore(device_node,parser);
          }
          device->Delete();
        }
        else
        {
          vtkErrorMacro("Unknown Device type or XML parse error: "<< device_name);
          fail=-1;
        }
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
  int num=this->Devices->GetNumberOfItems();
  return num;
}

//------------------------------------------------------------------------------
void mafDeviceSet::AddDevice(mafDevice *device)
//------------------------------------------------------------------------------
{
  assert (device);
  assert (device->GetName()); // all devices must have a name
  DevicesMutex->Lock();
  Devices->AppendItem(device);
  device->SetListener(this);
  device->PlugEventSource(this,mflAgent::DownStreamChannel);
  DevicesMutex->Unlock();
  
  ForwardEvent(mflSmartEvent(DeviceAddedEvent,this,device));
}

//------------------------------------------------------------------------------
mafDevice *mafDeviceSet::GetDevice(const char *name)
//------------------------------------------------------------------------------
{
  DevicesMutex->Lock();
  for (mafDevice *device=Devices->InitTraversal();device;device=Devices->GetNextItem())
  {
    if (vtkString::Equals(device->GetName(),name))
    {
      DevicesMutex->Unlock();
      return device;
    }
  }

  DevicesMutex->Unlock();
  return NULL;
}
//------------------------------------------------------------------------------
mafDevice *mafDeviceSet::GetDevice(mafID id)
//------------------------------------------------------------------------------
{
  DevicesMutex->Lock();
  for (mafDevice *device=Devices->InitTraversal();device;device=Devices->GetNextItem())
  {
    if (device->GetID()==id)
    {
      DevicesMutex->Unlock();
      return device;
    }
  }

  for (device=Devices->InitTraversal();device;device=Devices->GetNextItem())
  {
    mafDeviceSet *device_set=mafDeviceSet::SafeDownCast(device);
    if (device_set)
    {
      mafDevice *sub_device=device_set->GetDevice(id);
      if (sub_device)
      {
        DevicesMutex->Unlock();
        return sub_device;
      }
    }
  }

  DevicesMutex->Unlock();
  return NULL;
}

//------------------------------------------------------------------------------
mafDevice *mafDeviceSet::GetDeviceByIndex(int idx)
//------------------------------------------------------------------------------
{
  DevicesMutex->Lock();
  mafDevice *device=Devices->GetItem(idx);
  DevicesMutex->Unlock();
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

  vtkErrorMacro("Trying to delete an inexistent device");
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

  vtkErrorMacro("Trying to delete an inexistent device");
  return false;
}

//------------------------------------------------------------------------------
int mafDeviceSet::RemoveDevice(mafDevice *device, bool force)
//------------------------------------------------------------------------------
{
  assert(device);

  // do not remove persistent devices if not forced
  if (device->IsPersistent()&&!force) 
    return false;

  ForwardEvent(DeviceRemovingEvent,DefaultChannel,device);
  DevicesMutex->Lock();
  device->Stop();
  device->SetListener(NULL);
  int flag=Devices->RemoveItem(device);
  DevicesMutex->Unlock();
  return flag;
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
  DevicesMutex->Lock();

  // Remove All (non-persistent) devices
  for (mafDevice *device=Devices->InitTraversal();!Devices->IsDoneWithTraversal();)
  {
    mafDevice *next_device=Devices->GetNextItem();

    // do not remove persistent devices if not forced
    if (device->IsPersistent()&&!force)
      continue;

    device->Stop();
    ForwardEvent(DeviceRemovingEvent,DefaultChannel,device);

    Devices->RemoveItem(device);

    device=next_device;
  }

  DevicesMutex->Unlock();
}

//------------------------------------------------------------------------------
void mafDeviceSet::ProcessEvent(mflEvent *event,mafID channel)
//------------------------------------------------------------------------------
{
  assert(event&&event->GetSender());

  int id=event->GetID();

  if (channel == DefaultChannel)
  {
    if (id==AddDeviceEvent)
    {
      // this could create problems since there's no control on data to really be of the right type
      this->AddDevice((mafDevice *)event->GetData());
      return;
    }
    else if (id==RemoveDeviceEvent)
    {
      this->RemoveDevice((mafDevice *)event->GetData());
      return;
    }
  }
  
  ForwardEvent(event,channel);
}
