/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEventSource.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-08 19:59:56 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
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
  mafObserversListType List; 
};

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafEventSource)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafEventSource::mafEventSource(void *owner)
//------------------------------------------------------------------------------
{
  Data  = NULL;
  Observers   = new mafObserversList;
  Owner       = owner;
}

//------------------------------------------------------------------------------
mafEventSource::~mafEventSource()
//------------------------------------------------------------------------------
{
  delete Observers; Observers = NULL;
}

//------------------------------------------------------------------------------
void mafEventSource::AddObserver(mafObserver *obj, int priority)
//------------------------------------------------------------------------------
{
  // searches for first element with priority <= priority
  mafObserversListType::iterator it;
  for (it=Observers->List.begin(); it!=Observers->List.end() && (*it).first>priority ;it++) ;

  Observers->List.insert(it,mafObserversPairType(priority,obj));
}
//------------------------------------------------------------------------------
bool mafEventSource::RemoveObserver(mafObserver *obj)
//------------------------------------------------------------------------------
{
  if (Observers->List.empty())
    return false;

  // an observer could be present more then one time!
  bool flag=false;
  mafObserversListType::iterator it;
  for (it=Observers->List.begin(); it!=Observers->List.end() ;it++)
  {
    if ((*it).second == obj)
    {
      Observers->List.erase(it);
      flag=true;
    }
  }

  return flag;
}
//------------------------------------------------------------------------------
bool mafEventSource::IsObserver(mafObserver *obj)
//------------------------------------------------------------------------------
{
  if (Observers->List.empty())
    return false;

  mafObserversListType::iterator it;
  for (it=Observers->List.begin();it!=Observers->List.end();it++) 
    if ((*it).second!=obj) return true;

  return false;
}

//------------------------------------------------------------------------------
bool mafEventSource::HasObservers()
//------------------------------------------------------------------------------
{
  return !Observers->List.empty();
}

//------------------------------------------------------------------------------
void mafEventSource::InvokeEvent(mafEventBase *e)
//------------------------------------------------------------------------------
{
  if (Observers->List.empty())
    return;  
  
  e->SetSource(this);

  mafObserversListType::iterator it;
  for (it=Observers->List.begin();it!=Observers->List.end();it++)
  {
    // rise an event to observers
    mafObserver *observer=(*it).second;
    observer->OnEvent(e);
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
  return Data;
}

//------------------------------------------------------------------------------
void mafEventSource::SetData(void *data)
//------------------------------------------------------------------------------
{
  Data = data;
}

//------------------------------------------------------------------------------
void mafEventSource::SetOwner(void *owner)
//------------------------------------------------------------------------------
{
  Owner=owner;
}

//------------------------------------------------------------------------------
void *mafEventSource::GetOwner()
//------------------------------------------------------------------------------
{
  return Owner;
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