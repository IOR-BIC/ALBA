/*=========================================================================

 Program: MAF2
 Module: mafEventBroadcaster
 Authors: Marco Petrone, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include "mafEventBroadcaster.h"
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
mafEventBroadcaster::mafEventBroadcaster(void *owner)
//------------------------------------------------------------------------------
{
  m_Data  = NULL;
  m_Observers   = new mafObserversList;
  m_Owner       = owner;
  m_Channel     = -1;
}

//------------------------------------------------------------------------------
mafEventBroadcaster::~mafEventBroadcaster()
//------------------------------------------------------------------------------
{
  delete m_Observers; m_Observers = NULL;
}

//------------------------------------------------------------------------------
void mafEventBroadcaster::AddObserver(mafObserver &obj, int priority)
//------------------------------------------------------------------------------
{
  AddObserver(&obj,priority);
}

//------------------------------------------------------------------------------
void mafEventBroadcaster::AddObserver(mafObserver *obj, int priority)
//------------------------------------------------------------------------------
{
  // search for first element with priority <= priority
  mafObserversListType::iterator it;
  for (it=m_Observers->m_List.begin(); it!=m_Observers->m_List.end() && (*it).first>priority ;it++) ;

  m_Observers->m_List.insert(it,mafObserversPairType(priority,obj));
}
//------------------------------------------------------------------------------
bool mafEventBroadcaster::RemoveObserver(mafObserver *obj)
//------------------------------------------------------------------------------
{
  if (m_Observers->m_List.empty())
    return false;

  // an observer could be present more then one time!
  bool flag = false;
  mafObserversListType::iterator it;
  for (it=m_Observers->m_List.begin(); it!=m_Observers->m_List.end() ;it++)
  {
    if ((*it).second == obj)
    {
      m_Observers->m_List.erase(it);
      flag=true;
      break;      // Paolo 08-06-2005
    }
  }

  return flag;
}

//------------------------------------------------------------------------------
void mafEventBroadcaster::RemoveAllObservers()
//------------------------------------------------------------------------------
{
  m_Observers->m_List.clear();
}

//------------------------------------------------------------------------------
bool mafEventBroadcaster::IsObserver(mafObserver *obj)
//------------------------------------------------------------------------------
{
  if (m_Observers->m_List.empty())
    return false;

  mafObserversListType::iterator it;
  for (it = m_Observers->m_List.begin(); it != m_Observers->m_List.end(); it++)
    if ((*it).second == obj) return true;

  return false;
}

//------------------------------------------------------------------------------
bool mafEventBroadcaster::HasObservers()
//------------------------------------------------------------------------------
{
  return !m_Observers->m_List.empty();
}

//------------------------------------------------------------------------------
void mafEventBroadcaster::GetObservers(std::vector<mafObserver *> &olist)
//------------------------------------------------------------------------------
{
  olist.clear();
  olist.resize(m_Observers->m_List.size());
  int i=0;
  for (mafObserversListType::iterator it=m_Observers->m_List.begin();it!=m_Observers->m_List.end();it++,i++)
  {
    olist[i]=it->second;
  }
}

//------------------------------------------------------------------------------
void mafEventBroadcaster::InvokeEvent(mafEventBase *e)
//------------------------------------------------------------------------------
{
  if (m_Observers->m_List.empty())
    return;
  
  // store old channel
  mafID old_ch=(m_Channel<0)?-1:e->GetChannel();
    
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
void mafEventBroadcaster::InvokeEvent(mafEventBase &e)
//------------------------------------------------------------------------------
{
  InvokeEvent(&e);
}

//------------------------------------------------------------------------------
void mafEventBroadcaster::InvokeEvent(void *sender, mafID id, void *data)
//------------------------------------------------------------------------------
{
  mafEventBase e(sender,id,data);

  InvokeEvent(e);
}

//------------------------------------------------------------------------------
mafObserverCallback *mafEventBroadcaster::AddObserverCallback(void (*f)(void *sender,
    mafID eid, void *clientdata, void *calldata))
//------------------------------------------------------------------------------
{
  mafObserverCallback *observer=new mafObserverCallback();
  AddObserver(observer);
  return observer;
}

//------------------------------------------------------------------------------
void mafEventBroadcaster::SetChannel(mafID ch)
//------------------------------------------------------------------------------
{
  m_Channel = ch;
}
//------------------------------------------------------------------------------
mafID mafEventBroadcaster::GetChannel()
//------------------------------------------------------------------------------
{
  return m_Channel;
}
