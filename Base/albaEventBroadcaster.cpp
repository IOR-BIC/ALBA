/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventBroadcaster
 Authors: Marco Petrone, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include "albaEventBroadcaster.h"
#include "albaObserver.h"
#include "albaObserverCallback.h"
#include "albaEventBase.h"
#include <utility>
#include "assert.h"

//------------------------------------------------------------------------------
albaEventBroadcaster::albaEventBroadcaster(void *owner)
{
  m_Channel     = -1;
}

//------------------------------------------------------------------------------
albaEventBroadcaster::~albaEventBroadcaster()
{
}

//------------------------------------------------------------------------------
void albaEventBroadcaster::AddObserver(albaObserver &obj)
{
  AddObserver(&obj);
}

//------------------------------------------------------------------------------
void albaEventBroadcaster::AddObserver(albaObserver *obj)
{
	if (m_Observers.empty())
		m_Listener = obj;

	if(obj)
		m_Observers.push_front(obj);
}
bool albaEventBroadcaster::RemoveObserver(albaObserver *obj)
//------------------------------------------------------------------------------
{
  if (m_Observers.empty())
    return false;

  // an observer could be present more then one time!
  bool flag = false;
  albaObserversList::iterator it;
  for (it=m_Observers.begin(); it!=m_Observers.end() ;it++)
  {
    if (*it == obj)
    {
      m_Observers.erase(it);
      flag=true;
      break;      // Paolo 08-06-2005
    }
  }

	if (m_Observers.empty())
		m_Listener = NULL;

  return flag;
}

//------------------------------------------------------------------------------
void albaEventBroadcaster::RemoveAllObservers()
{
  m_Observers.clear();
	m_Listener = NULL;
}

//------------------------------------------------------------------------------
bool albaEventBroadcaster::IsObserver(albaObserver *obj)
{
  if (m_Observers.empty())
    return false;

  albaObserversList::iterator it;
  for (it = m_Observers.begin(); it != m_Observers.end(); it++)
    if (*it == obj) return true;

  return false;
}

//------------------------------------------------------------------------------
bool albaEventBroadcaster::HasObservers()
{
  return !m_Observers.empty();
}

//------------------------------------------------------------------------------
void albaEventBroadcaster::GetObservers(std::vector<albaObserver *> &olist)
{
  olist.clear();
  olist.resize(m_Observers.size());
  int i=0;
  for (albaObserversList::iterator it=m_Observers.begin();it!=m_Observers.end();it++,i++)
  {
		olist[i] = *it;
  }
}

//------------------------------------------------------------------------------
void albaEventBroadcaster::InvokeEvent(albaEventBase *e)
{
  if (m_Observers.empty())
    return;
  
  // store old channel
  albaID old_ch=(m_Channel<0)?-1:e->GetChannel();
    
  albaObserversList::iterator it;
  for (it=m_Observers.begin();it!=m_Observers.end();it++)
  {
    // Set the event channel (if necessary).
    // Must set it at each iteration since it could have
    // been changed by other event sources on the path
    if (m_Channel>=0&&m_Channel!=e->GetChannel())
      e->SetChannel(m_Channel);

    // rise an event to observers
    albaObserver *observer=*it;
    observer->OnEvent(e);
  }

	// restore old channel
  if (old_ch>0) e->SetChannel(old_ch);
}

//------------------------------------------------------------------------------
void albaEventBroadcaster::InvokeEvent(albaEventBase &e)
{
  InvokeEvent(&e);
}

//------------------------------------------------------------------------------
void albaEventBroadcaster::InvokeEvent(void *sender, albaID id, void *data)
{
  albaEventBase e(sender,id,data);

  InvokeEvent(e);
}

//------------------------------------------------------------------------------
albaObserverCallback *albaEventBroadcaster::AddObserverCallback(void (*f)(void *sender, albaID eid, void *clientdata, void *calldata))
{
  albaObserverCallback *observer=new albaObserverCallback();
  AddObserver(observer);
  return observer;
}

//------------------------------------------------------------------------------
void albaEventBroadcaster::SetChannel(albaID ch)
{
  m_Channel = ch;
}
//------------------------------------------------------------------------------
albaID albaEventBroadcaster::GetChannel()
{
  return m_Channel;
}

//----------------------------------------------------------------------------
void albaEventBroadcaster::SetListener(albaObserver *o)
{
	RemoveAllObservers();
	AddObserver(o);
}
