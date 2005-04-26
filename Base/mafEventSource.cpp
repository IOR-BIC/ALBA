/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEventSource.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-26 18:31:17 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



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
  m_Channel     = -1;
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
void mafEventSource::RemoveAllObservers()
//------------------------------------------------------------------------------
{
  m_Observers->m_List.clear();
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
  
  // store old channel
  mafID old_ch=(m_Channel<0)?-1:e->GetChannel();
    
  e->SetSource(this);
  
  mafObserversListType::iterator it;
  for (it=m_Observers->m_List.begin();it!=m_Observers->m_List.end()&&!e->GetSkipFlag();it++)
  {
    // Set the event channel (if neccessary).
    // Must set it at each iteration since it could have
    // been changed by other event sources on the path
    if (m_Channel>=0&&m_Channel!=e->GetChannel())
      e->SetChannel(m_Channel);

    // rise an event to observers
    mafObserver *observer=(*it).second;
    observer->OnEvent(e);
  }
  
  // reset skip flag
  e->SetSkipFlag(false);

  // restore old channel
  if (old_ch>0) e->SetChannel(old_ch);
}

//------------------------------------------------------------------------------
void mafEventSource::InvokeEvent(mafEventBase &e)
//------------------------------------------------------------------------------
{
  InvokeEvent(&e);
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

//------------------------------------------------------------------------------
void mafEventSource::SetChannel(mafID ch)
//------------------------------------------------------------------------------
{
  m_Channel = ch;
}
//------------------------------------------------------------------------------
mafID mafEventSource::GetChannel()
//------------------------------------------------------------------------------
{
  return m_Channel;
}