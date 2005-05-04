/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAction.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-04 16:27:46 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows
#ifdef __GNUG__
    #pragma implementation "mafAction.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "mafInteractionDecl.h"
#include "mafEventBase.h"
#include "mafDevice.h"
#include "mafInteractor.h"

#include "mafAction.h"
#include "vtkObjectFactory.h"
#include "vtkObjectMap.h"
#include "vtkTemplatedList.txx"
#include "mflXMLWriter.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLDataParser.h"
#include "vtkRenderer.h"

#include <utility>


//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MFL_EVT_IMP(mafAction::BindDeviceToActionEvent);
MFL_EVT_IMP(mafAction::QueryForConnectedDeviceEvent);
MFL_EVT_IMP(mafAction::PluggedDeviceEvent);
MFL_EVT_IMP(mafAction::UnPluggedDeviceEvent);

//------------------------------------------------------------------------------
vtkStandardNewMacro(mafAction)
vtkCxxSetObjectMacro(mafAction,Renderer,vtkRenderer);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafAction::mafAction()
//------------------------------------------------------------------------------
{
  Type  = SHARED_ACTION;
  vtkNEW(Devices);
  Renderer = NULL;
}

//------------------------------------------------------------------------------
mafAction::~mafAction()
//------------------------------------------------------------------------------
{
  SetRenderer(NULL);
  vtkDEL(Devices);
}

//------------------------------------------------------------------------------
void mafAction::BindDevice(mafDevice *device)
//------------------------------------------------------------------------------
{
  assert(device);
  
  // the device is plugged as a source of events on the
  // device input channel
  PlugEventSource(device,mafDevice::DeviceInputChannel);
  // its also plugged as listener on the output channel
  PlugListener(device,mafDevice::DeviceOutputChannel);
  Devices->AppendItem(device);
  //if (device->IsInitialized())
  //{
  //}
}
//------------------------------------------------------------------------------
void mafAction::UnBindDevice(mafDevice *device)
//------------------------------------------------------------------------------
{
  assert(device);
  UnPlugEventSource(device);
  UnPlugListener(device);
  Devices->RemoveItem(device);
}

//------------------------------------------------------------------------------
void mafAction::BindInteractor(mafInteractor *inter)
//------------------------------------------------------------------------------
{
  assert(inter);

  // the interactor is plugged as listener on the device
  // input channel
  PlugListener(inter,mafDevice::DeviceInputChannel);\
  // it is plugged as event source on the output channel
  PlugEventSource(inter,mafDevice::DeviceOutputChannel);
  //inter->SetRenderer(Renderer);
  inter->Initialize();
}

//------------------------------------------------------------------------------
void mafAction::UnBindInteractor(mafInteractor *inter)
//------------------------------------------------------------------------------
{
  assert(inter);
  inter->Shutdown();
  UnPlugListener(inter);
  UnPlugEventSource(inter);
  
}

//------------------------------------------------------------------------------
int mafAction::Store(mflXMLWriter *writer)
//------------------------------------------------------------------------------
{
  // Store bindings to devices, not binsings to interactors, 
  // since these last ones are created at runtime
  writer->OpenTag("Action");
  writer->AddAttribute("Name",GetName());
  writer->CloseTag("Action");
  writer->DisplayXML("\r");  
  for (mafDevice *device=Devices->InitTraversal();device;device=Devices->GetNextItem())
  {
    writer->OpenTag("Device");
    writer->AddAttribute("Name",device->GetName());
    writer->AddAttribute("ID",mflString(device->GetID()-mafDevice::MIN_DEVICE_ID));
    writer->CloseTag("Device");
    writer->CloseElement("Device");
  }
  writer->CloseElement("Action");
  return 0;
}

//------------------------------------------------------------------------------
int mafAction::Restore(vtkXMLDataElement *node,vtkXMLDataParser *parser)
//------------------------------------------------------------------------------
{
  if (vtkString::Equals(node->GetName(),"Action"))
  {
    const char *name=node->GetAttribute("Name");
    assert(vtkString::Equals(name,GetName()));
    for (int i=0;i<node->GetNumberOfNestedElements();i++)
    {
      vtkXMLDataElement *subnode=node->GetNestedElement(i);
      assert(subnode);
      if (vtkString::Equals(subnode->GetName(),"Device"))
      {
        int id;
        const char *name=subnode->GetAttribute("Name");
        if (subnode->GetScalarAttribute("ID",id)&&name)
        {
          // forward an event to device manager to perform binding...
          ForwardEvent(mafSmartEvent(this,BindDeviceToActionEvent,(long)(id+mafDevice::MIN_DEVICE_ID)));
        }
        else
        {
          vtkErrorMacro("Wrong MIS file, cannot found device ID or name parsing <Action>");
          return -1;
        }
      }
    }
    return 0;
  }
  else
  {
    vtkErrorMacro("Error XML parsing error: expected <Action> found <"<<node->GetName()<<">");
  }
  return -1;
}


//------------------------------------------------------------------------------
void mafAction::ProcessEvent(mflEvent *event,mafID ch)
//------------------------------------------------------------------------------
{
  assert(event);

  mafID id=event->GetID();

  // for catching view select event
  if (ch==mafDevice::DeviceOutputChannel && id==QueryForConnectedDeviceEvent)
  {
    mflAgent *sender=mflAgent::SafeDownCast((vtkObject *)event->GetSender());
    assert(sender);
    
    for (mafDevice *dev=Devices->InitTraversal();dev;dev=Devices->GetNextItem())
    {
      if (dev->IsInitialized())
      {
        // send an event only to the inquiring object about all plugged devices
        mflSmartEvent e(PluggedDeviceEvent,this,dev);
        sender->ProcessEvent(e,mafDevice::DeviceInputChannel);
      }      
    }
  }
  else if (ch==mafDevice::DeviceInputChannel && id==mafDevice::DeviceStartedEvent)
  {
    // send an event to all observers to advise about a plugged device
    ForwardEvent(PluggedDeviceEvent,mafDevice::DeviceInputChannel,(mafDevice *)event->GetSender());
  }
  else if (ch==mafDevice::DeviceInputChannel && id==mafDevice::DeviceStoppedEvent)
  {
    // send an event to all observers to advise about an unplugged device
    ForwardEvent(UnPluggedDeviceEvent,mafDevice::DeviceInputChannel,(mafDevice *)event->GetSender());
  }
  else
  {
    // simply forward the event
    ForwardEvent(event,ch);
  }
}
