/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAction
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDevice.h"
#include "albaInteractor.h"

#include "albaAction.h"
#include "mmuIdFactory.h"
#include "albaStorageElement.h"
#include "albaEvent.h"

//#include "vtkRenderer.h"

#include <utility>


//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
ALBA_ID_IMP(albaAction::DEVICE_BIND);
ALBA_ID_IMP(albaAction::QUERY_CONNECTED_DEVICES);
ALBA_ID_IMP(albaAction::DEVICE_PLUGGED);
ALBA_ID_IMP(albaAction::DEVICE_UNPLUGGED);

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaAction)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaAction::albaAction()
//------------------------------------------------------------------------------
{
  m_Type  = SHARED_ACTION;
}

//------------------------------------------------------------------------------
albaAction::~albaAction()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void albaAction::BindDevice(albaDevice *device)
//------------------------------------------------------------------------------
{
  assert(device);
  
  // the device an observer of events on the
  // device input channel
  device->AddObserver(this,MCH_INPUT);
  
  // this action must observe the device output channel
  AddObserver(device,MCH_OUTPUT);

  m_Devices.push_back(albaAutoPointer<albaDevice>(device));
}
//------------------------------------------------------------------------------
void albaAction::UnBindDevice(albaDevice *device)
//------------------------------------------------------------------------------
{
  assert(device);

  // disconnect the device from this action
  device->RemoveObserver(this);   
  this->RemoveObserver(device);  
  
  // remove the device from the list
  for (mmuDeviceList::iterator it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    if (it->GetPointer() == device)
    {
      m_Devices.erase(it);
      return;
    }
  }
}

//------------------------------------------------------------------------------
void albaAction::BindInteractor(albaInteractor *inter)
//------------------------------------------------------------------------------
{
  assert(inter);

  // the interactor is added as an observer on the device input channel
  AddObserver(inter,MCH_INPUT);
  
  // it is plugged as event source on the output channel
  inter->AddObserver(this,MCH_OUTPUT);
  
  inter->Initialize(); // force interactor to initialize to be ready for incoming events
}

//------------------------------------------------------------------------------
void albaAction::UnBindInteractor(albaInteractor *inter)
//------------------------------------------------------------------------------
{
  assert(inter);
  inter->Shutdown();
  RemoveObserver(inter);
  inter->RemoveObserver(this);
  
}

//------------------------------------------------------------------------------
int albaAction::InternalStore(albaStorageElement *node)
//------------------------------------------------------------------------------
{
  // Store bindings to devices, not bindings to interactors, 
  // since the last ones are created at runtime.
  
  node->SetAttribute("Name",GetName());
  albaStorageElement *subnode = node->AppendChild("Device");

  for (mmuDeviceList::iterator it = m_Devices.begin(); it!=m_Devices.end(); it++)
  {
    albaDevice *device=it->GetPointer();
    subnode->SetAttribute("Name",device->GetName());
    subnode->SetAttribute("ID",(albaID)(device->GetID()));
  }

  return ALBA_OK;
}

//------------------------------------------------------------------------------
int albaAction::InternalRestore(albaStorageElement *node)
//------------------------------------------------------------------------------
{
  albaString name;
  node->GetAttribute("Name",name);
 
  SetName(name);

  albaStorageElement::ChildrenVector &children=node->GetChildren();
  for (int i=0;i<children.size();i++)
  {
    albaStorageElement *subnode = children[i];
    assert(subnode);
    if (albaCString(subnode->GetName()) == "Device")
    {
      albaID id;
      albaString name;

      if (subnode->GetAttributeAsInteger("ID",id) && subnode->GetAttribute("Name",name))
      {
        // forward an event to device manager to perform binding...
        albaEventMacro(albaEvent(this,DEVICE_BIND,(long)id));
      }
      else
      {
        albaErrorMacro("Wrong MIS file, cannot found device ID or name parsing <Action>");
        return ALBA_ERROR;
      }
    }
  }

  return ALBA_OK;
}


//------------------------------------------------------------------------------
void albaAction::OnEvent(albaEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);

  albaID id=event->GetId();
  albaID ch=event->GetChannel();

  // for catching view select event
  if (ch==MCH_OUTPUT && id==QUERY_CONNECTED_DEVICES)
  {
    albaAgent *sender=(albaAgent *)event->GetSender();
    assert(sender);
    
    
    for (mmuDeviceList::iterator it=m_Devices.begin();it!=m_Devices.end();it++)
    {
      albaDevice *dev=it->GetPointer();
      if (dev->IsInitialized())
      {
        // send an event only to the inquiring object about all plugged devices
        sender->OnEvent(&albaEventBase(this,DEVICE_PLUGGED,dev,MCH_INPUT));
      }      
    }
  }
  else if (ch==MCH_INPUT && id==albaDevice::DEVICE_STARTED)
  {
    // send an event to all observers to advise about a plugged device
    InvokeEvent(this,DEVICE_BIND,MCH_INPUT,(albaDevice *)event->GetSender());
  }
  else if (ch==MCH_INPUT && id==albaDevice::DEVICE_STOPPED)
  {
    // send an event to all observers to advise about an unplugged device
    InvokeEvent(this,DEVICE_UNPLUGGED,MCH_INPUT,(albaDevice *)event->GetSender());
  }
  else
  {
    // simply forward the event
    InvokeEvent(event,ch);
  }
}
