/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiSER.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-21 12:03:50 $
  Version:   $Revision: 1.5 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mmiSER.h"
#include "mmdTracker.h"
#include "mafStorageElement.h"

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
  for (mmuActionsMap::iterator it=m_Actions.begin();it!=m_Actions.end();it++)
  {
    mafAction *action=it->second;
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
    if (mafCString(subnode->GetName())!="Action")
    {
      mafErrorMacro("Unexpected element <"<<subnode->GetName()<<">");
      return MAF_ERROR;
    }
   
    // create the object to be restored mannualy since mafAction is not in the factory
    mafString action_name;
    subnode->GetAttribute("Name",action_name);
    mafAction *action = GetAction(action_name);

    if (action && action->Restore(subnode))
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
  mmuActionsMap::iterator it=m_Actions.find(name);
  return (it!=m_Actions.end()?it->second:NULL);
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
  assert(action);
  m_Actions[action->GetName()]=action;
  
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
  std::vector<mafAction *> device_actions;
  GetDeviceBindings(device,device_actions);

  for (int i=0;i<device_actions.size();i++)
  {
    device_actions[i]->UnBindDevice(device);
  }
}

//------------------------------------------------------------------------------
void mmiSER::GetDeviceBindings(mafDevice *device,std::vector<mafAction *> &actions)
//------------------------------------------------------------------------------
{
  assert(device);
  
  actions.clear();

  std::vector<mafObserver *> observers;
  device->GetObservers(MCH_INPUT,observers);
  
  for (int i=0;i<observers.size();i++)
  {
    mafAction *action;
    try { action=dynamic_cast<mafAction *>(observers[i]); } catch (std::bad_cast) { action=NULL;}
   
    if (action)
    {
      actions.push_back(action);
    }  
  }
}
