/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDeviceManager.cpp,v $
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
    #pragma implementation "mafDeviceManager.cpp"
#endif

// For compilers that support pre-compilation, includes "wx/wx.h".
#include "wx/wxprec.h"

// MAF events
#include "mafDecl.h"

// base includes
#include "mafDeviceManager.h"

// factory
#include "mafInteractionFactory.h"

// MFL events
#include "mflEvent.h"

// local 
#include "mafDeviceSet.h"

// serialization
#include "mflXMLWriter.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLDataParser.h" 

// VTK local
#include "mflString.h"
#include "vtkTemplatedList.txx"
#include <assert.h>




//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MFL_EVT_IMP(mafDeviceManager::StartDispatchingEvent);
MFL_EVT_IMP(mafDeviceManager::EndDispatchingEvent);
//------------------------------------------------------------------------------
// Static Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vtkStandardNewMacro(mafDeviceManager)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafDeviceManager::mafDeviceManager()
//------------------------------------------------------------------------------
{
  RestoringFlag   = false;
  DeviceIdCounter = mafDevice::MIN_DEVICE_ID;
  PersistentDeviceIdCounter = 0;
  vtkNEW(DeviceSet);
  DeviceSet->SetPersistentFlag(true);
  DeviceSet->SetThreaded(false);
  DeviceSet->SetName("DeviceSet");
  DeviceSet->AutoStartOn();
  DeviceSet->SetListener(this);
  DeviceSet->SetID(++PersistentDeviceIdCounter); // ID of the root device
}

//------------------------------------------------------------------------------
mafDeviceManager::~mafDeviceManager()
//------------------------------------------------------------------------------
{
  vtkDEL(DeviceSet)
}

//------------------------------------------------------------------------------
int mafDeviceManager::InternalInitialize()
//------------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize())
    return -1;

  // startup all devices
  return DeviceSet->StartUp();
}

//------------------------------------------------------------------------------
void mafDeviceManager::InternalShutdown()
//------------------------------------------------------------------------------
{
  DeviceSet->Stop();
  Superclass::InternalShutdown();
  mafYield();
}

//------------------------------------------------------------------------------
mafID mafDeviceManager::AddDevice(mafDevice *device)
//------------------------------------------------------------------------------
{
  DeviceSet->AddDevice(device);
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
    
    const char *dev_name=iFactory->GetDeviceName(type);
    assert(dev_name);

    mflString base_name=dev_name;    
    mflString instance_name = base_name;

    for (int id=1;true;id++)
    {
      
      // if no device with the same name exists break!
      if (DeviceSet->GetDevice(instance_name)==NULL)
        break;

      // else append an numeric postfix
      instance_name=base_name+" ("+mflString(id)+")";
    }
    
    device->SetName(instance_name);
    
    mafDevice *ret = (AddDevice(device)>0)?device:NULL;

    device->Delete();

    return ret;
  }

  return device;
}

//------------------------------------------------------------------------------
int mafDeviceManager::RemoveDevice(mafDevice *device, bool force)
//------------------------------------------------------------------------------
{
  return DeviceSet->RemoveDevice(device,force);
}

//------------------------------------------------------------------------------
int mafDeviceManager::RemoveDevice(const char *name, bool force)
//------------------------------------------------------------------------------
{
  return DeviceSet->RemoveDevice(name,force);
}

//------------------------------------------------------------------------------
mafDevice *mafDeviceManager::GetDevice(const char *name)
//------------------------------------------------------------------------------
{
  return DeviceSet->GetDevice(name);
}

//------------------------------------------------------------------------------
mafDevice *mafDeviceManager::GetDevice(mafID id)
//------------------------------------------------------------------------------
{
  return DeviceSet->GetDevice(id);
}
//------------------------------------------------------------------------------
vtkTemplatedList<mafDevice> *mafDeviceManager::GetDevices()
//------------------------------------------------------------------------------
{
  return DeviceSet->GetDevices();
}

//------------------------------------------------------------------------------
int mafDeviceManager::GetNumberOfDevices()
{
  return DeviceSet->GetNumberOfDevices();
}

//------------------------------------------------------------------------------
void mafDeviceManager::RemoveAllDevices(bool force)
//------------------------------------------------------------------------------
{
  DeviceSet->RemoveAllDevices(force);
}

//----------------------------------------------------------------------------
int mafDeviceManager::Store(mflXMLWriter *writer)
//----------------------------------------------------------------------------
{
  assert(writer);
  writer->OpenTag("DeviceManager");
  writer->AddAttribute("DeviceIdCounter",DeviceIdCounter);
  writer->CloseTag("DeviceManager");

  if (DeviceSet->Store(writer))
    return -1;
  
  writer->CloseElement("DeviceManager");
  return 0;
}

//----------------------------------------------------------------------------
int mafDeviceManager::Restore(vtkXMLDataElement *parent,vtkXMLDataParser *parser)
//----------------------------------------------------------------------------
{
  assert(parent);
  RestoringFlag=true; // used to avoid DeviceManager set device ID when restoring
  int fail=-1;

  vtkXMLDataElement *node=parent->FindNestedElementWithName("DeviceManager");
  if (node)
  {
    int id_counter;
    if (node->GetScalarAttribute("DeviceIdCounter",id_counter))
    {
      DeviceIdCounter=id_counter;
      vtkXMLDataElement *sub_node=node->FindNestedElementWithName("Device");
      if (sub_node&&mflString(sub_node->GetAttribute("Type"))=="mafDeviceSet")
        fail=DeviceSet->Restore(sub_node,parser);
    }
  }
  
  RestoringFlag=false;

  return fail;
}

//------------------------------------------------------------------------------
bool mafDeviceManager::DispatchEvents()
//------------------------------------------------------------------------------
{
  if (Dispatched)
    return false;

  ForwardEvent(StartDispatchingEvent); // Advise dispatching is starting 
  bool ret=Superclass::DispatchEvents();
  ForwardEvent(EndDispatchingEvent); // Advise dispatching is finished 

  return ret;
}

//------------------------------------------------------------------------------
void mafDeviceManager::ProcessEvent(mflEvent *event,mafID channel)
//------------------------------------------------------------------------------
{
  assert(event&&event->GetSender());

  int id=event->GetID();

  if (id==DispatchEvent)
  {
    // Push the event to the queue. The DispatchEvent() of this
    // function will automatically call DispatchEvents() of the
    // sender. Note DispatchEvent() of this function is called in 
    // the wxWindows message pump thread. This way this object acts 
    // the synchronization of all the events coming from different sources.
    PushEvent(event,channel);
  }
  else if (id==mafDeviceSet::DeviceAddedEvent )
  {
    mafDevice *device=(mafDevice *)event->GetData();
    if (!RestoringFlag)
    {
      if (device->IsPersistent())
      {
        device->SetID(++PersistentDeviceIdCounter);
      }
      else
      {
        device->SetID(++DeviceIdCounter);
      }

    }
    ForwardEvent(event,channel);
  }
 
  Superclass::ProcessEvent(event);
 
}