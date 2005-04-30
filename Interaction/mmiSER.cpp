/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiSER.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:34:58 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows
#ifdef __GNUG__
    #pragma implementation "mmiSER.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "mmiSER.h"
#include "mmdTracker.h"
#include "mafInteractionDecl.h"

#include "mflEvent.h"


#include "vtkObjectFactory.h"
#include "vtkCollection.h"
#include "vtkTemplatedMap.txx"
#include "vtkTemplatedList.txx"

#include "mflXMLWriter.h"
#include "vtkXMLDataParser.h"
#include "vtkXMLDataElement.h"

#include <assert.h>

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
//MFL_EVT_IMP(mmiSER::MoveActionEvent);

//------------------------------------------------------------------------------
vtkStandardNewMacro(mmiSER)
//------------------------------------------------------------------------------
mmiSER::mmiSER()
//------------------------------------------------------------------------------
{
  vtkNEW(Actions); // actions list
}

//------------------------------------------------------------------------------
mmiSER::~mmiSER()
//------------------------------------------------------------------------------
{
  vtkDEL(Actions);
}
//------------------------------------------------------------------------------
int mmiSER::Store(mflXMLWriter *writer)
//------------------------------------------------------------------------------
{
  writer->OpenElement("DeviceBindings");
  
  for (mafAction *action=Actions->InitTraversal();action;action=Actions->GetNextItem())
  {
    action->Store(writer);   
  }
  writer->CloseElement("DeviceBindings");

  return 0;
}

//------------------------------------------------------------------------------
int mmiSER::Restore(vtkXMLDataElement *parent,vtkXMLDataParser *parser)
//------------------------------------------------------------------------------
{
  vtkXMLDataElement *node=parent->FindNestedElementWithName("DeviceBindings");
  if (node)
  {
    for (int i=0;i<node->GetNumberOfNestedElements();i++)
    {
      vtkXMLDataElement *subnode=node->GetNestedElement(i);
      if (!vtkString::Equals(subnode->GetName(),"Action"))
      {
        vtkGenericWarningMacro("Unexpected element <"<<subnode->GetName()<<">");
        return -1;
      }

      const char *action_name=subnode->GetAttribute("Name");
      if (mafAction *action=GetAction(action_name))
      {
        action->Restore(subnode,parser);
      }
      else
      {
        vtkGenericWarningMacro("Action not found: Cannot restore bindings for action "<<action_name);
      }

    }
    return 0;
  }

  vtkErrorMacro("XML Parse error: cannot find <DeviceBindings> element");
  return -1;
}

//------------------------------------------------------------------------------
mafAction *mmiSER::GetAction(const char *name)
//------------------------------------------------------------------------------
{
  return Actions->GetItem(name);
}

//------------------------------------------------------------------------------
vtkTemplatedMap<wxString,mafAction> *mmiSER::GetActions()
//------------------------------------------------------------------------------
{
  return Actions;
}
//------------------------------------------------------------------------------
int mmiSER::BindAction(const char *action,mafInteractor *agent)
//------------------------------------------------------------------------------
{
  mafAction *a=GetAction(action);
  if (a)
  {
    a->BindInteractor(agent);
    return 0;
  }
  
  return -1;
}

//------------------------------------------------------------------------------
int mmiSER::UnBindAction(const char *action,mafInteractor *agent)
//------------------------------------------------------------------------------
{
  mafAction *a=GetAction(action);
  if (a)
  {
    a->UnBindInteractor(agent);
    return 0;
  }

  return -1;
}

//------------------------------------------------------------------------------
mafAction *mmiSER::AddAction(const char *name, float priority, int type)
//------------------------------------------------------------------------------
{
  if (mafAction *old_action=GetAction(name))
    return old_action;

  mflSmartPointer<mafAction> action;
  action->SetName(name);
  action->SetType(type);
  AddAction(action,priority);
  
  return action;
}

//------------------------------------------------------------------------------
void mmiSER::AddAction(mafAction *action, float priority)
//------------------------------------------------------------------------------
{
  Actions->SetItem(action->GetName(),action);
  
  // attach the action both as a listener and an event source 
  action->PlugListener(this);
  action->PlugEventSource(this,CameraUpdateChannel);
}
//------------------------------------------------------------------------------
void mmiSER::BindDeviceToAction(mafDevice *device,mafAction *action)
//------------------------------------------------------------------------------
{
  assert(device);
  assert(action);
  action->BindDevice(device);
}
//------------------------------------------------------------------------------
void mmiSER::BindDeviceToAction(mafDevice *device,const char *action_name)
//------------------------------------------------------------------------------
{
  BindDeviceToAction(device,GetAction(action_name));
}
//------------------------------------------------------------------------------
void mmiSER::UnBindDeviceFromAction(mafDevice *device,mafAction *action)
//------------------------------------------------------------------------------
{
  assert(device);
  assert(action);
  action->UnBindDevice(device);
}
//------------------------------------------------------------------------------
void mmiSER::UnBindDeviceFromAction(mafDevice *device,const char *action_name)
//------------------------------------------------------------------------------
{
  UnBindDeviceFromAction(device,GetAction(action_name));
}

//------------------------------------------------------------------------------
void mmiSER::UnBindDeviceFromAllActions(mafDevice *device)
//------------------------------------------------------------------------------
{
  assert(device);
  mflSmartPointer<vtkCollection> device_actions;
  GetDeviceBindings(device,device_actions);
  device_actions->InitTraversal();
  for (mafAction *action=(mafAction *)device_actions->GetNextItemAsObject();action; \
    action=(mafAction *)device_actions->GetNextItemAsObject())
  {
    action->UnBindDevice(device);
  }
  
}

//------------------------------------------------------------------------------
void mmiSER::GetDeviceBindings(mafDevice *device,vtkCollection *actions)
//------------------------------------------------------------------------------
{
  assert(actions);
  assert(device);
  
  actions->RemoveAllItems();

  // Currently there's no way to extract listeners list from a device (or agent),
  // thus I have to search all the actions for the device as an event source 
  
  for (mafAction *action=Actions->InitTraversal();action;action=Actions->GetNextItem())
  {
    if (action->GetDevices()->IsItemPresent(device))
    {
      actions->AddItem(action);
    }
  }
}
/*
//------------------------------------------------------------------------------
void mmiSER::ProcessEvent(mflEvent *event,mafID ch)
//------------------------------------------------------------------------------
{
  assert(event);
  
  mafID id=event->GetID();

  if (ch==mafDevice::DeviceInputChannel)
  {
    
  }
}
*/