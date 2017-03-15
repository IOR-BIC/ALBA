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
#include <utility>
#include "assert.h"

//------------------------------------------------------------------------------
mafEventBroadcaster::mafEventBroadcaster(void *owner)
{
  m_Data  = NULL;
  m_Owner       = owner;
  m_Channel     = -1;
}

//------------------------------------------------------------------------------
mafEventBroadcaster::~mafEventBroadcaster()
{
}

//------------------------------------------------------------------------------
void mafEventBroadcaster::AddObserver(mafObserver &obj)
{
  AddObserver(&obj);
}

//------------------------------------------------------------------------------
void mafEventBroadcaster::AddObserver(mafObserver *obj)
{
  m_Observers.push_back(obj);
}
bool mafEventBroadcaster::RemoveObserver(mafObserver *obj)
//------------------------------------------------------------------------------
{
  if (m_Observers.empty())
    return false;

  // an observer could be present more then one time!
  bool flag = false;
  mafObserversList::iterator it;
  for (it=m_Observers.begin(); it!=m_Observers.end() ;it++)
  {
    if (*it == obj)
    {
      m_Observers.erase(it);
      flag=true;
      break;      // Paolo 08-06-2005
    }
  }

  return flag;
}

//------------------------------------------------------------------------------
void mafEventBroadcaster::RemoveAllObservers()
{
  m_Observers.clear();
}

//------------------------------------------------------------------------------
bool mafEventBroadcaster::IsObserver(mafObserver *obj)
{
  if (m_Observers.empty())
    return false;

  mafObserversList::iterator it;
  for (it = m_Observers.begin(); it != m_Observers.end(); it++)
    if (*it == obj) return true;

  return false;
}

//------------------------------------------------------------------------------
bool mafEventBroadcaster::HasObservers()
{
  return !m_Observers.empty();
}

//------------------------------------------------------------------------------
void mafEventBroadcaster::GetObservers(std::vector<mafObserver *> &olist)
{
  olist.clear();
  olist.resize(m_Observers.size());
  int i=0;
  for (mafObserversList::iterator it=m_Observers.begin();it!=m_Observers.end();it++,i++)
  {
		olist[i] = *it;
  }
}

//------------------------------------------------------------------------------
void mafEventBroadcaster::InvokeEvent(mafEventBase *e)
{
  if (m_Observers.empty())
    return;
  
  // store old channel
  mafID old_ch=(m_Channel<0)?-1:e->GetChannel();
    
  mafObserversList::iterator it;
  for (it=m_Observers.begin();it!=m_Observers.end();it++)
  {
    // Set the event channel (if necessary).
    // Must set it at each iteration since it could have
    // been changed by other event sources on the path
    if (m_Channel>=0&&m_Channel!=e->GetChannel())
      e->SetChannel(m_Channel);

    // rise an event to observers
    mafObserver *observer=*it;
    observer->OnEvent(e);
  }

	// restore old channel
  if (old_ch>0) e->SetChannel(old_ch);
}

//------------------------------------------------------------------------------
void mafEventBroadcaster::InvokeEvent(mafEventBase &e)
{
  InvokeEvent(&e);
}

//------------------------------------------------------------------------------
void mafEventBroadcaster::InvokeEvent(mafID id, void *data)
{
  mafEventBase e(this,id,data);

  InvokeEvent(e);
}

//------------------------------------------------------------------------------
mafObserverCallback *mafEventBroadcaster::AddObserverCallback(void (*f)(void *sender, mafID eid, void *clientdata, void *calldata))
{
  mafObserverCallback *observer=new mafObserverCallback();
  AddObserver(observer);
  return observer;
}

//------------------------------------------------------------------------------
void mafEventBroadcaster::SetChannel(mafID ch)
{
  m_Channel = ch;
}
//------------------------------------------------------------------------------
mafID mafEventBroadcaster::GetChannel()
{
  return m_Channel;
}

//----------------------------------------------------------------------------
void mafEventBroadcaster::SetListener(mafObserver *o)
{
	RemoveAllObservers();
	AddObserver(o);
}

//----------------------------------------------------------------------------
mafObserver * mafEventBroadcaster::GetListener()
{
	if (m_Observers.size() > 0)
		return m_Observers.front();
	else
		return NULL;
}

//----------------------------------------------------------------------------
bool mafEventBroadcaster::HasListener()
{
	return (m_Observers.size() > 0);
}
