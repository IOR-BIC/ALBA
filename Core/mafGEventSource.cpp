/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGEventSource.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-04 20:59:27 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafGEventSource.h"
#include "mafGObserver.h"
//#include "mafGObserverCallback.h"
#include "mafEventBase.h"
#include <list>
#include <utility>
#include "assert.h"

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------
typedef std::pair<int,mafGObserver *> mafGObserversPairType;
typedef std::list< mafGObserversPairType > mafGObserversListType;
class mafGObserversList
{
public:
  mafGObserversListType List; 
};

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafGEventSource)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafGEventSource::mafGEventSource(void *owner)
//------------------------------------------------------------------------------
{
  Data  = NULL;
  Observers   = new mafGObserversList;
  Owner       = owner;
}

//------------------------------------------------------------------------------
mafGEventSource::~mafGEventSource()
//------------------------------------------------------------------------------
{
  delete Observers; Observers = NULL;
}

//------------------------------------------------------------------------------
void mafGEventSource::AddObserver(mafGObserver *obj, int priority)
//------------------------------------------------------------------------------
{
  // searches for first element with priority <= priority
  mafGObserversListType::iterator it;
  for (it=Observers->List.begin(); it!=Observers->List.end() && (*it).first>priority ;it++) ;

  Observers->List.insert(it,mafGObserversPairType(priority,obj));
}
//------------------------------------------------------------------------------
bool mafGEventSource::RemoveObserver(mafGObserver *obj)
//------------------------------------------------------------------------------
{
  if (Observers->List.empty())
    return false;

  // an observer could be present more then one time!
  bool flag=false;
  mafGObserversListType::iterator it;
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
bool mafGEventSource::IsObserver(mafGObserver *obj)
//------------------------------------------------------------------------------
{
  if (Observers->List.empty())
    return false;

  mafGObserversListType::iterator it;
  for (it=Observers->List.begin();it!=Observers->List.end();it++) 
    if ((*it).second!=obj) return true;

  return false;
}

//------------------------------------------------------------------------------
bool mafGEventSource::HasObservers()
//------------------------------------------------------------------------------
{
  return !Observers->List.empty();
}

//------------------------------------------------------------------------------
void mafGEventSource::InvokeEvent(mafEventBase *e)
//------------------------------------------------------------------------------
{
  if (Observers->List.empty())
    return;  
  
  //e->SetSource(this);

  mafGObserversListType::iterator it;
  for (it=Observers->List.begin();it!=Observers->List.end();it++)
  {
    // rise an event to observers
    mafGObserver *observer=(*it).second;
    observer->ProcessEvent(e);
  }
}

//------------------------------------------------------------------------------
void mafGEventSource::InvokeEvent(void *sender,void *data)
//------------------------------------------------------------------------------
{
  mafEventBase e(sender,data);

  InvokeEvent(e);
}

//------------------------------------------------------------------------------
void *mafGEventSource::GetData()
//------------------------------------------------------------------------------
{
  return Data;
}

//------------------------------------------------------------------------------
void mafGEventSource::SetData(void *data)
//------------------------------------------------------------------------------
{
  Data = data;
}

//------------------------------------------------------------------------------
void mafGEventSource::SetOwner(void *owner)
//------------------------------------------------------------------------------
{
  Owner=owner;
}

//------------------------------------------------------------------------------
void *mafGEventSource::GetOwner()
//------------------------------------------------------------------------------
{
  return Owner;
}

//------------------------------------------------------------------------------
/*mafGObserverCallback *mafGEventSource::AddObserverCallback(void (*f)(void *sender,
    mafID eid, void *clientdata, void *calldata))
//------------------------------------------------------------------------------
{
  mafGObserverCallback *observer=new mafGObserverCallback();
  AddObserver(observer);
  return observer;
}*/