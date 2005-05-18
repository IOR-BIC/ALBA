/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiSER.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-18 17:29:06 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mmiSER.h"
#include "mmdTracker.h"
#include "mafInteractionDecl.h"

#include <assert.h>

//------------------------------------------------------------------------------
mafCxxTypeMacro(mmiSER)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmiSER::mmiSER()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mmiSER::~mmiSER()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
int mmiSER::InternalStore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  for (mafAction *action=m_Actions->InitTraversal();action;action=m_Actions->GetNextItem())
  {
    node->StoreObject("Action",action);   
  }

  return 0;
}

//------------------------------------------------------------------------------
int mmiSER::InternalRestore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  mafStorageElement::ChildrenVector children=node->GetChildren();
  for (int i=0;i<children.size();i++)
  {
    mafStorageElement *subnode=children[i];
    if (!mafCString(subnode->GetName())=="Action")
    {
      mafErrorMacro("Unexpected element <"<<subnode->GetName()<<">");
      return MAF_ERROR;
    }
   
    // create the object to be restored mannualy since mafAction is not in the factory
    mafAction *action = mafAction::New();
    if (action->Restore(subnode))
    {
      mafErrorMacro("I/O Error restoring action");
    }
  }
  return MAF_OK;
}

//------------------------------------------------------------------------------
mafAction *mmiSER::GetAction(const char *name)
//------------------------------------------------------------------------------
{
  return m_Actions->GetItem(name);
}

//------------------------------------------------------------------------------
void mmiSER::GetActions(std::vector<mafAction *> &actions)
//------------------------------------------------------------------------------
{
  actions.clear();
  actions.resize(m_Actions.size());
  int i=0;
  for (std::map<mafString,mafAutoPointer<mafAction> >::iterator it=m_Actions.begin();it!=m_Actions.end();it++,i++)
  {
    actions[i]=it->second;
  }
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

  mafSmartPointer<mafAction> action;
  action->SetName(name);
  action->SetType(type);
  AddAction(action,priority);
  
  return action;
}

//------------------------------------------------------------------------------
void mmiSER::AddAction(mafAction *action, float priority)
//------------------------------------------------------------------------------
{
  m_Actions->SetItem(action->GetName(),action);
  
  // attach the action both as a listener and an event source 
  action->AddObserver(this);
  //action->PlugEventSource(this,MCH_CAMERA CameraUpdateChannel);
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
  
  for (mafAction *action=m_Actions->InitTraversal();action;action=m_Actions->GetNextItem())
  {
    if (action->GetDevices()->IsItemPresent(device))
    {
      actions->AddItem(action);
    }
  }
}
