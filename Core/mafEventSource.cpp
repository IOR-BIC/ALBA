/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEventSource.cpp,v $
  Language:  C++
  Date:      $Date: 2005-02-20 23:24:58 $
  Version:   $Revision: 1.7 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafEventSource.h"
#include "mafObserver.h"
#include "mafObserverCallback.h"
#include "mafEventBase.h"
#include <list>
#include <utility>
#include "assert.h"

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------
typedef std::pair<int,mafObserver *> mafObserversPairType;
typedef std::list< mafObserversPairType > mafObserversListType;
class mafObserversList
{
public:
  mafObserversListType m_List; 
};

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafEventSource)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafEventSource::mafEventSource(void *owner)
//------------------------------------------------------------------------------
{
  m_Data  = NULL;
  m_Observers   = new mafObserversList;
  m_Owner       = owner;
}

//------------------------------------------------------------------------------
mafEventSource::~mafEventSource()
//------------------------------------------------------------------------------
{
  delete m_Observers; m_Observers = NULL;
}

//------------------------------------------------------------------------------
void mafEventSource::AddObserver(mafObserver &obj, int priority)
//------------------------------------------------------------------------------
{
  AddObserver(&obj,priority);
}

//------------------------------------------------------------------------------
void mafEventSource::AddObserver(mafObserver *obj, int priority)
//------------------------------------------------------------------------------
{
  // search for first element with priority <= priority
  mafObserversListType::iterator it;
  for (it=m_Observers->m_List.begin(); it!=m_Observers->m_List.end() && (*it).first>priority ;it++) ;

  m_Observers->m_List.insert(it,mafObserversPairType(priority,obj));
}
//------------------------------------------------------------------------------
bool mafEventSource::RemoveObserver(mafObserver *obj)
//------------------------------------------------------------------------------
{
  if (m_Observers->m_List.empty())
    return false;

  // an observer could be present more then one time!
  bool flag=false;
  mafObserversListType::iterator it;
  for (it=m_Observers->m_List.begin(); it!=m_Observers->m_List.end() ;it++)
  {
    if ((*it).second == obj)
    {
      m_Observers->m_List.erase(it);
      flag=true;
    }
  }

  return flag;
}
//------------------------------------------------------------------------------
bool mafEventSource::IsObserver(mafObserver *obj)
//------------------------------------------------------------------------------
{
  if (m_Observers->m_List.empty())
    return false;

  mafObserversListType::iterator it;
  for (it=m_Observers->m_List.begin();it!=m_Observers->m_List.end();it++) 
    if ((*it).second!=obj) return true;

  return false;
}

//------------------------------------------------------------------------------
bool mafEventSource::HasObservers()
//------------------------------------------------------------------------------
{
  return !m_Observers->m_List.empty();
}

//------------------------------------------------------------------------------
void mafEventSource::InvokeEvent(mafEventBase *e)
//------------------------------------------------------------------------------
{
  if (m_Observers->m_List.empty())
    return;  
  
  e->SetSource(this);

  mafObserversListType::iterator it;
  for (it=m_Observers->m_List.begin();it!=m_Observers->m_List.end();it++)
  {
    // rise an event to observers
    mafObserver *observer=(*it).second;
    observer->OnEvent(e);
  }
}

//------------------------------------------------------------------------------
void mafEventSource::InvokeEvent(mafEventBase &e)
//------------------------------------------------------------------------------
{
  if (m_Observers->m_List.empty())
    return;  
  
  e.SetSource(this);

  mafObserversListType::iterator it;
  for (it=m_Observers->m_List.begin();it!=m_Observers->m_List.end()&&!e.GetSkipFlag();it++)
  {
    // rise an event to observers
    mafObserver *observer=(*it).second;
    observer->OnEvent(&e);
  }
}

//------------------------------------------------------------------------------
void mafEventSource::InvokeEvent(void *sender, mafID id, void *data)
//------------------------------------------------------------------------------
{
  mafEventBase e(sender,id,data);

  InvokeEvent(e);
}

//------------------------------------------------------------------------------
void *mafEventSource::GetData()
//------------------------------------------------------------------------------
{
  return m_Data;
}

//------------------------------------------------------------------------------
void mafEventSource::SetData(void *data)
//------------------------------------------------------------------------------
{
  m_Data = data;
}

//------------------------------------------------------------------------------
void mafEventSource::SetOwner(void *owner)
//------------------------------------------------------------------------------
{
  m_Owner=owner;
}

//------------------------------------------------------------------------------
void *mafEventSource::GetOwner()
//------------------------------------------------------------------------------
{
  return m_Owner;
}

//------------------------------------------------------------------------------
mafObserverCallback *mafEventSource::AddObserverCallback(void (*f)(void *sender,
    mafID eid, void *clientdata, void *calldata))
//------------------------------------------------------------------------------
{
  mafObserverCallback *observer=new mafObserverCallback();
  AddObserver(observer);
  return observer;
}
