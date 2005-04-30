/*=========================================================================

Program:   Multimod Fundation Library
Module:    $RCSfile: mafAgentEventQueue.cpp,v $
Language:  C++
Date:      $Date: 2005-04-30 14:34:52 $
Version:   $Revision: 1.4 $

=========================================================================*/
#include "mafAgentEventQueue.h"
#include "mafMutexLock.h"
#include "mmuIdFactory.h"

#include <assert.h>

#include <deque>

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------
class mafAgentEventQueue::EventQueueItem
{
public:
  EventQueueItem(mafEventBase *event=NULL):m_Event(event) {};

  mafEventBase *m_Event;
};


struct mafAgentEventQueue::InternalEventQueue 
{
  std::deque< mafAgentEventQueue::EventQueueItem > Q;
};

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafAgentEventQueue::EVENT_DISPATCH);

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafAgentEventQueue);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafAgentEventQueue::mafAgentEventQueue()
//------------------------------------------------------------------------------
{
  m_Dispatched      = true;
  m_EventQueue      = new InternalEventQueue;
  m_DispatchMode    = SelfProcessMode;
  m_DequeueMode     = MultipleEventMode;
  m_PushMode        = DispatchEventMode;
  
  m_Mutex = new mafMutexLock;
}

//------------------------------------------------------------------------------
mafAgentEventQueue::~mafAgentEventQueue()
//------------------------------------------------------------------------------
{
  //vtkGenericWarningMacro("Destroying mafAgentEventQueue");
  delete m_EventQueue; m_EventQueue=NULL;
  cppDEL(m_Mutex);
}

//------------------------------------------------------------------------------
void mafAgentEventQueue::SetDispatched(bool value)
//------------------------------------------------------------------------------
{
  m_Mutex->Lock();
  m_Dispatched=value;
  m_Mutex->Unlock();
}

//------------------------------------------------------------------------------
bool mafAgentEventQueue::DispatchEvents()
//------------------------------------------------------------------------------
{
  if (m_Dispatched)
    return false;

  if (m_DequeueMode==MultipleEventMode)
  {
    mafEventBase *event;
    unsigned long channel;
    
    mafEventBase *last_event=PeekLastEvent(); // store the last event of the queue.
  
    // flush the queue but avoid infinite loops, by looping for no more than queue size
    //for (int qsize=GetQueueSize();(qsize>0)&&this->PopEvent(event,channel);qsize--)    
    do 
    {
      this->PopEvent(event);
      channel=event->GetChannel();
      if (event)
      {
        if (event->GetId()==EVENT_DISPATCH)
        {
          mafAgentEventQueue *sender=(mafAgentEventQueue *)event->GetSender();
          sender->DispatchEvents();
        }
        else
        {
          if (m_DispatchMode==SelfProcessMode)
          {
            OnEvent(event);
          }
          else
          {
            ForwardEvent(event,channel);
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
    mafEventBase *event;
    unsigned long channel;

    if (this->PopEvent(event))
    {
      channel=event->GetChannel();
      if (event->GetId()==EVENT_DISPATCH)
      {
        mafAgentEventQueue *sender=(mafAgentEventQueue *)event->GetSender();
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
          this->ForwardEvent(event,channel);
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
int mafAgentEventQueue::PushEvent(mafID event, void *sender,void *data)
//------------------------------------------------------------------------------
{
  return this->PushEvent(&mafEventBase(sender,event,data));
}

//------------------------------------------------------------------------------
int mafAgentEventQueue::PushEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  if (event)
  {
    m_Mutex->Lock();
    
    mafEventBase *new_event=event->NewInstance();
    assert(new_event);
    *new_event=*event;
    
    mafAgentEventQueue::EventQueueItem item(new_event);
    
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
  
  mafErrorMacro("Trying to push a NULL event");
  return false;
}

//------------------------------------------------------------------------------
void mafAgentEventQueue::RequestForDispatching()
//------------------------------------------------------------------------------
{
  ForwardEvent(EVENT_DISPATCH,MCH_UP);
}

//------------------------------------------------------------------------------
int mafAgentEventQueue::PopEvent(mafEventBase *&event)
//------------------------------------------------------------------------------
{
  m_Mutex->Lock();

  int ret;
  if (m_EventQueue->Q.size()>0)
  {
    
    // Notice the event object destruction is left to DispatchEvents()
    // after dispatching to avoid object copying.
    mafAgentEventQueue::EventQueueItem item=m_EventQueue->Q.back();
    m_EventQueue->Q.pop_back();
    event=item.m_Event;
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
mafEventBase *mafAgentEventQueue::PopEvent()
//------------------------------------------------------------------------------
{
  mafEventBase *ev;

  if (this->PopEvent(ev))
  { 
    return ev;
  }

  return NULL;
}

//------------------------------------------------------------------------------
int mafAgentEventQueue::PopEvent(mafEventBase &event)
//------------------------------------------------------------------------------
{
  mafEventBase *ev;

  if (this->PopEvent(ev))
  {
    event=*ev;
    ev->Delete(); // destroy the queued event

    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
mafEventBase *mafAgentEventQueue::PeekEvent()
//------------------------------------------------------------------------------
{
  m_Mutex->Lock();

  mafEventBase *event=NULL;
  if (m_EventQueue->Q.size()>0)
  {
    EventQueueItem item=m_EventQueue->Q.back();
    event=item.m_Event;  
  }
  
  m_Mutex->Unlock();

  return event;
}

//------------------------------------------------------------------------------
mafEventBase *mafAgentEventQueue::PeekLastEvent()
//------------------------------------------------------------------------------
{
  m_Mutex->Lock();

  mafEventBase *event=NULL;
  if (m_EventQueue->Q.size()>0)
  {
    EventQueueItem item=m_EventQueue->Q.front();
    event=item.m_Event;  
  }
  
  m_Mutex->Unlock();

  return event;
}
//------------------------------------------------------------------------------
int mafAgentEventQueue::GetQueueSize()
//------------------------------------------------------------------------------
{
  m_Mutex->Lock();
  return m_EventQueue->Q.size();
  m_Mutex->Unlock();
}