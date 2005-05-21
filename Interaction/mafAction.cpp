/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAction.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-21 07:55:49 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDevice.h"
#include "mafInteractor.h"

#include "mafAction.h"
#include "mmuIdFactory.h"
#include "mafStorageElement.h"
#include "mafEvent.h"

//#include "vtkRenderer.h"

#include <utility>


//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafAction::ACTION_BIND_DEVICE);
MAF_ID_IMP(mafAction::ACTION_QUERY_CONNECTED_DEVICES);
MAF_ID_IMP(mafAction::ACTION_DEVICE_PLUGGED);
MAF_ID_IMP(mafAction::ACTION_DEVICE_UNPLUGGED);

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafAction)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafAction::mafAction()
//------------------------------------------------------------------------------
{
  m_Type  = SHARED_ACTION;
}

//------------------------------------------------------------------------------
mafAction::~mafAction()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void mafAction::BindDevice(mafDevice *device)
//------------------------------------------------------------------------------
{
  assert(device);
  
  // the device is plugged as a source of events on the
  // device input channel
  PlugEventSource(device,MCH_INPUT);
  
  // its also plugged as listener on the output channel
  AddObserver(device,MCH_OUTPUT);

  m_Devices.push_back(device);

}
//------------------------------------------------------------------------------
void mafAction::UnBindDevice(mafDevice *device)
//------------------------------------------------------------------------------
{
  assert(device);

  // disconnect the device from this action
  UnPlugEventSource(device);   
  this->RemoveObserver(device);  
  
  // remove the device from the list
  for (mmuDeviceList::iterator it=m_Devices.begin();it!=m_Devices.end();it++)
  {
    if (it->GetPointer() == device)
    {
      m_Devices.erase(it);
    }
  }
}

//------------------------------------------------------------------------------
void mafAction::BindInteractor(mafInteractor *inter)
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
void mafAction::UnBindInteractor(mafInteractor *inter)
//------------------------------------------------------------------------------
{
  assert(inter);
  inter->Shutdown();
  RemoveObserver(inter);
  inter->RemoveObserver(this);
  
}

//------------------------------------------------------------------------------
int mafAction::InternalStore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  // Store bindings to devices, not bindings to interactors, 
  // since the last ones are created at runtime.
  
  node->SetAttribute("Name",GetName());
  mafStorageElement *subnode = node->AppendChild("Device");

  for (mmuDeviceList::iterator it = m_Devices.begin(); it!=m_Devices.end(); it++)
  {
    mafDevice *device=it->GetPointer();
    subnode->SetAttribute("Name",device->GetName());
    subnode->SetAttribute("ID",(mafID)(device->GetID()-mafDevice::MIN_DEVICE_ID));
  }

  return MAF_OK;
}

//------------------------------------------------------------------------------
int mafAction::InternalRestore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  mafString name;
  node->GetAttribute("Name",name);

  assert(name==GetName());

  mafStorageElement::ChildrenVector &children=node->GetChildren();
  for (int i=0;i<children.size();i++)
  {
    mafStorageElement *subnode = children[i];
    assert(subnode);
    if (mafCString(subnode->GetName()) == "Device")
    {
      mafID id;
      mafString name;

      if (subnode->GetAttributeAsInteger("ID",id) && subnode->GetAttribute("Name",name))
      {
        // forward an event to device manager to perform binding...
        mafEventMacro(mafEvent(this,ACTION_BIND_DEVICE,(long)(id+mafDevice::MIN_DEVICE_ID)));
      }
      else
      {
        mafErrorMacro("Wrong MIS file, cannot found device ID or name parsing <Action>");
        return MAF_ERROR;
      }
    }
  }

  return MAF_OK;
}


//------------------------------------------------------------------------------
void mafAction::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);

  mafID id=event->GetId();
  mafID ch=event->GetChannel();

  // for catching view select event
  if (ch==MCH_OUTPUT && id==ACTION_QUERY_CONNECTED_DEVICES)
  {
    mafAgent *sender=(mafAgent *)event->GetSender();
    assert(sender);
    
    
    for (mmuDeviceList::iterator it=m_Devices.begin();it!=m_Devices.end();it++)
    {
      mafDevice *dev=it->GetPointer();
      if (dev->IsInitialized())
      {
        // send an event only to the inquiring object about all plugged devices
        sender->OnEvent(&mafEventBase(this,ACTION_BIND_DEVICE,dev,MCH_INPUT));
      }      
    }
  }
  else if (ch==MCH_INPUT && id==mafDevice::DEVICE_STARTED)
  {
    // send an event to all observers to advise about a plugged device
    InvokeEvent(ACTION_BIND_DEVICE,MCH_INPUT,(mafDevice *)event->GetSender());
  }
  else if (ch==MCH_INPUT && id==mafDevice::DEVICE_STOPPED)
  {
    // send an event to all observers to advise about an unplugged device
    InvokeEvent(ACTION_DEVICE_UNPLUGGED,MCH_INPUT,(mafDevice *)event->GetSender());
  }
  else
  {
    // simply forward the event
    InvokeEvent(event,ch);
  }
}
