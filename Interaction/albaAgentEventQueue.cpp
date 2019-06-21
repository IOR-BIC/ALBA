/*=========================================================================

Program:   Multimod Fundation Library
Module:    $RCSfile: albaAgentEventQueue.cpp,v $
Language:  C++
Date:      $Date: 2007-03-15 17:12:24 $
Version:   $Revision: 1.8 $

=========================================================================*/
#include "albaAgentEventQueue.h"
#include "albaMutexLock.h"
#include "mmuIdFactory.h"

#include <assert.h>

#include <deque>

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------
class albaAgentEventQueue::EventQueueItem
{
public:
  EventQueueItem(albaEventBase *event=NULL):m_Event(event) {};

  albaEventBase *m_Event;
};


struct albaAgentEventQueue::InternalEventQueue 
{
  std::deque< albaAgentEventQueue::EventQueueItem > Q;
};

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
ALBA_ID_IMP(albaAgentEventQueue::EVENT_DISPATCH);

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaAgentEventQueue);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaAgentEventQueue::albaAgentEventQueue()
//------------------------------------------------------------------------------
{
  m_Dispatched      = true;
  m_EventQueue      = new InternalEventQueue;
  m_DispatchMode    = SelfProcessMode;
  m_DequeueMode     = MultipleEventMode;
  m_PushMode        = DispatchEventMode;
  
  m_Mutex = new albaMutexLock;
}

//------------------------------------------------------------------------------
albaAgentEventQueue::~albaAgentEventQueue()
//------------------------------------------------------------------------------
{
  //vtkGenericWarningMacro("Destroying albaAgentEventQueue");
  delete m_EventQueue; m_EventQueue=NULL;
  cppDEL(m_Mutex);
}

//------------------------------------------------------------------------------
void albaAgentEventQueue::SetDispatched(bool value)
//------------------------------------------------------------------------------
{
  m_Mutex->Lock();
  m_Dispatched=value;
  m_Mutex->Unlock();
}

//------------------------------------------------------------------------------
bool albaAgentEventQueue::DispatchEvents()
//------------------------------------------------------------------------------
{
  if (m_Dispatched)
    return false;

  if (m_DequeueMode==MultipleEventMode)
  {
    albaEventBase *event;
    unsigned long channel;
    
    albaEventBase *last_event=PeekLastEvent(); // store the last event of the queue.
  
    // flush the queue but avoid infinite loops, by looping for no more than queue size
    //for (int qsize=GetQueueSize();(qsize>0)&&this->PopEvent(event,channel);qsize--)    
    do 
    {
      this->PopEvent(event);
       
      if (event)
      {
        channel=event->GetChannel();
        if (event->GetId()==EVENT_DISPATCH)
        {
          albaAgentEventQueue *sender=(albaAgentEventQueue *)event->GetSender();
          sender->DispatchEvents();
        }
        else
        {
          if (m_DispatchMode==SelfProcessMode || channel==MCH_INPUT)
          {
            OnEvent(event);
          }
          else
          {
            InvokeEvent(event,channel);
          }
        }
        event->Delete(); // release memory
      }      
    }
    while (event&&PeekEvent()!=last_event);

    //this->SetDispatched();

    m_Mutex->Lock();
    if (m_EventQueue->Q.size()==0)
    {
        this->SetDispatched();
    }
    else
    {
      this->RequestForDispatching();
    }
    m_Mutex->Unlock();
  }
  else
  {
    albaEventBase *event;
    unsigned long channel;

    if (this->PopEvent(event))
    {
      channel=event->GetChannel();
      if (event->GetId()==EVENT_DISPATCH)
      {
        albaAgentEventQueue *sender=(albaAgentEventQueue *)event->GetSender();
        sender->DispatchEvents();
      }
      else
      {
        if (m_DispatchMode==SelfProcessMode)
        {
          this->OnEvent(event);
        }
        else
        {
          this->InvokeEvent(event,channel);
        }
        event->Delete(); // release memory
      }
    }

    if (this->IsQueueEmpty())
    {
      this->SetDispatched();
    }
    else
    {
      this->RequestForDispatching();
    }
  }

  return true;
}

//------------------------------------------------------------------------------
bool albaAgentEventQueue::PushEvent(albaID event, void *sender,void *data)
//------------------------------------------------------------------------------
{
  return this->PushEvent(&albaEventBase(sender,event,data));
}

//------------------------------------------------------------------------------
bool albaAgentEventQueue::PushEvent(albaEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);
  if (event)
  {
    m_Mutex->Lock();
    
    albaEventBase *new_event=event->NewInstance();
    assert(new_event);
    *new_event=*event;
    
    albaAgentEventQueue::EventQueueItem item(new_event);
    
    m_EventQueue->Q.push_front(item);

    if (m_Dispatched&&m_PushMode==DispatchEventMode)
    {
      // advise listener class a new event is in the queue
      m_Dispatched=false;
      RequestForDispatching();
    }
    m_Mutex->Unlock();
    return true;
  }
  
  albaErrorMacro("Trying to push a NULL event");
  return false;
}

//------------------------------------------------------------------------------
void albaAgentEventQueue::RequestForDispatching()
//------------------------------------------------------------------------------
{
  InvokeEvent(this,EVENT_DISPATCH);
}

//------------------------------------------------------------------------------
int albaAgentEventQueue::PopEvent(albaEventBase *&event)
//------------------------------------------------------------------------------
{
  m_Mutex->Lock();

  int ret;
  if (m_EventQueue->Q.size()>0)
  {
    
    // Notice the event object destruction is left to DispatchEvents()
    // after dispatching to avoid object copying.
    albaAgentEventQueue::EventQueueItem item=m_EventQueue->Q.back();
    m_EventQueue->Q.pop_back();
    event=item.m_Event;
    assert(event);
    ret=true;
  }
  else
  {
    event=NULL;
    ret=false;
  }

  m_Mutex->Unlock();

  return ret;

}

//------------------------------------------------------------------------------
albaEventBase *albaAgentEventQueue::PopEvent()
//------------------------------------------------------------------------------
{
  albaEventBase *ev;

  if (this->PopEvent(ev))
  { 
    return ev;
  }

  return NULL;
}

//------------------------------------------------------------------------------
int albaAgentEventQueue::PopEvent(albaEventBase &event)
//------------------------------------------------------------------------------
{
  albaEventBase *ev;

  if (this->PopEvent(ev))
  {
    event.DeepCopy(ev);
    ev->Delete(); // destroy the queued event

    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
albaEventBase *albaAgentEventQueue::PeekEvent()
//------------------------------------------------------------------------------
{
  m_Mutex->Lock();

  albaEventBase *event=NULL;
  if (m_EventQueue->Q.size()>0)
  {
    EventQueueItem item=m_EventQueue->Q.back();
    event=item.m_Event;  
  }
  
  m_Mutex->Unlock();

  return event;
}

//------------------------------------------------------------------------------
albaEventBase *albaAgentEventQueue::PeekLastEvent()
//------------------------------------------------------------------------------
{
  m_Mutex->Lock();

  albaEventBase *event=NULL;
  if (m_EventQueue->Q.size()>0)
  {
    EventQueueItem item=m_EventQueue->Q.front();
    event=item.m_Event;  
  }
  
  m_Mutex->Unlock();

  return event;
}
//------------------------------------------------------------------------------
int albaAgentEventQueue::GetQueueSize()
//------------------------------------------------------------------------------
{
  m_Mutex->Lock();
  return m_EventQueue->Q.size();
  m_Mutex->Unlock();
}

