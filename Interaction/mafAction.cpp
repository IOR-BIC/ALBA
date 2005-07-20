/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAction.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-20 15:49:47 $
  Version:   $Revision: 1.5 $
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
MAF_ID_IMP(mafAction::DEVICE_BIND);
MAF_ID_IMP(mafAction::QUERY_CONNECTED_DEVICES);
MAF_ID_IMP(mafAction::DEVICE_PLUGGED);
MAF_ID_IMP(mafAction::DEVICE_UNPLUGGED);

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
    subnode->SetAttribute("ID",(mafID)(device->GetID()));
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
        mafEventMacro(mafEvent(this,DEVICE_PLUGGED,(long)id));
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
  if (ch==MCH_OUTPUT && id==QUERY_CONNECTED_DEVICES)
  {
    mafAgent *sender=(mafAgent *)event->GetSender();
    assert(sender);
    
    
    for (mmuDeviceList::iterator it=m_Devices.begin();it!=m_Devices.end();it++)
    {
      mafDevice *dev=it->GetPointer();
      if (dev->IsInitialized())
      {
        // send an event only to the inquiring object about all plugged devices
        sender->OnEvent(&mafEventBase(this,DEVICE_PLUGGED,dev,MCH_INPUT));
      }      
    }
  }
  else if (ch==MCH_INPUT && id==mafDevice::DEVICE_STARTED)
  {
    // send an event to all observers to advise about a plugged device
    InvokeEvent(DEVICE_BIND,MCH_INPUT,(mafDevice *)event->GetSender());
  }
  else if (ch==MCH_INPUT && id==mafDevice::DEVICE_STOPPED)
  {
    // send an event to all observers to advise about an unplugged device
    InvokeEvent(DEVICE_UNPLUGGED,MCH_INPUT,(mafDevice *)event->GetSender());
  }
  else
  {
    // simply forward the event
    InvokeEvent(event,ch);
  }
}
