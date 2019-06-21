/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorSER
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaInteractorSER.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaStorageElement.h"

#include <assert.h>

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractorSER)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaInteractorSER::albaInteractorSER()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
albaInteractorSER::~albaInteractorSER()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
int albaInteractorSER::InternalStore(albaStorageElement *node)
//------------------------------------------------------------------------------
{
  for (mmuActionsMap::iterator it=m_Actions.begin();it!=m_Actions.end();it++)
  {
    albaAction *action=it->second;
    node->StoreObject("Action",action);   
  }

  return 0;
}

//------------------------------------------------------------------------------
int albaInteractorSER::InternalRestore(albaStorageElement *node)
//------------------------------------------------------------------------------
{
  albaStorageElement::ChildrenVector children=node->GetChildren();
  for (int i=0;i<children.size();i++)
  {
    albaStorageElement *subnode=children[i];
    if (albaCString(subnode->GetName())!="Action")
    {
      albaErrorMacro("Unexpected element <"<<subnode->GetName()<<">");
      return ALBA_ERROR;
    }
   
    // create the object to be restored mannualy since albaAction is not in the factory
    albaString action_name;
    subnode->GetAttribute("Name",action_name);
    albaAction *action = GetAction(action_name);

    if (action && action->Restore(subnode))
    {
      albaErrorMacro("I/O Error restoring action");
    }
  }
  return ALBA_OK;
}

//------------------------------------------------------------------------------
albaAction *albaInteractorSER::GetAction(const char *name)
//------------------------------------------------------------------------------
{
  mmuActionsMap::iterator it=m_Actions.find(name);
  return (it!=m_Actions.end()?it->second:NULL);
}

//------------------------------------------------------------------------------
void albaInteractorSER::GetActions(std::vector<albaAction *> &actions)
//------------------------------------------------------------------------------
{
  actions.clear();
  actions.resize(m_Actions.size());
  int i=0;
  for (std::map<albaString,albaAutoPointer<albaAction> >::iterator it=m_Actions.begin();it!=m_Actions.end();it++,i++)
  {
    actions[i]=it->second;
  }
}
//------------------------------------------------------------------------------
int albaInteractorSER::BindAction(const char *action,albaInteractor *agent)
//------------------------------------------------------------------------------
{
  albaAction *a=GetAction(action);
  if (a)
  {
    a->BindInteractor(agent);
    return 0;
  }
  
  return -1;
}

//------------------------------------------------------------------------------
int albaInteractorSER::UnBindAction(const char *action,albaInteractor *agent)
//------------------------------------------------------------------------------
{
  albaAction *a=GetAction(action);
  if (a)
  {
    a->UnBindInteractor(agent);
    return 0;
  }

  return -1;
}

//------------------------------------------------------------------------------
albaAction *albaInteractorSER::AddAction(const char *name, float priority, int type)
//------------------------------------------------------------------------------
{
  if (albaAction *old_action=GetAction(name))
    return old_action;

  albaSmartPointer<albaAction> action;
  action->SetName(name);
  action->SetType(type);
  AddAction(action,priority);
  
  return action;
}

//------------------------------------------------------------------------------
void albaInteractorSER::AddAction(albaAction *action, float priority)
//------------------------------------------------------------------------------
{
  assert(action);
  m_Actions[action->GetName()]=action;
  
  // attach the action both as a listener and an event source 
  action->AddObserver(this);
}
//------------------------------------------------------------------------------
void albaInteractorSER::BindDeviceToAction(albaDevice *device,albaAction *action)
//------------------------------------------------------------------------------
{
  assert(device);
  assert(action);
  action->BindDevice(device);
}
//------------------------------------------------------------------------------
void albaInteractorSER::BindDeviceToAction(albaDevice *device,const char *action_name)
//------------------------------------------------------------------------------
{
  BindDeviceToAction(device,GetAction(action_name));
}
//------------------------------------------------------------------------------
void albaInteractorSER::UnBindDeviceFromAction(albaDevice *device,albaAction *action)
//------------------------------------------------------------------------------
{
  assert(device);
  assert(action);
  action->UnBindDevice(device);
}
//------------------------------------------------------------------------------
void albaInteractorSER::UnBindDeviceFromAction(albaDevice *device,const char *action_name)
//------------------------------------------------------------------------------
{
  UnBindDeviceFromAction(device,GetAction(action_name));
}

//------------------------------------------------------------------------------
void albaInteractorSER::UnBindDeviceFromAllActions(albaDevice *device)
//------------------------------------------------------------------------------
{
  assert(device);
  std::vector<albaAction *> device_actions;
  GetDeviceBindings(device,device_actions);

  for (int i=0;i<device_actions.size();i++)
  {
    device_actions[i]->UnBindDevice(device);
  }
}

//------------------------------------------------------------------------------
void albaInteractorSER::GetDeviceBindings(albaDevice *device,std::vector<albaAction *> &actions)
//------------------------------------------------------------------------------
{
  assert(device);
  
  actions.clear();

  std::vector<albaObserver *> observers;
  device->GetObservers(MCH_INPUT,observers);
  
  for (int i=0;i<observers.size();i++)
  {
    albaAction *action;
    try { action=dynamic_cast<albaAction *>(observers[i]); } catch (std::bad_cast) { action=NULL;}
   
    if (action)
    {
      actions.push_back(action);
    }  
  }
}
