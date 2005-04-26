/*=========================================================================

Program:   Multimod Fundation Library
Module:    $RCSfile: mafAgentEventQueue.cpp,v $
Language:  C++
Date:      $Date: 2005-04-26 18:32:33 $
Version:   $Revision: 1.1 $

=========================================================================*/
#include "mafAgentEventQueue.h.h"
#include "vtkObjectFactory.h"

#include "mflDefines.h"
#include "vtkMultiThreader.h"
#include "vtkCriticalSection.h"
#include <assert.h>

#include <deque>

//typedef std::deque< mflSmartEvent > EventQueue;

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------
class mafAgentEventQueue.h::EventQueueItem
{
public:
  EventQueueItem(mflEvent *event=NULL,unsigned long channel=mflAgent::DefaultChannel):Event(event),Channel(channel) {};
//  EventQueueItem(const EventQueueItem& s):Event(s.Event),Channel(s.Channel) {};

  mflEvent *Event;
  unsigned long Channel;
};


struct mafAgentEventQueue.h::InternalEventQueue 
{
  std::deque< mafAgentEventQueue.h::EventQueueItem > Q;
};

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MFL_EVT_IMP(mafAgentEventQueue.h::DispatchEvent);

//------------------------------------------------------------------------------
vtkStandardNewMacro(mafAgentEventQueue.h);

//------------------------------------------------------------------------------
mafAgentEventQueue.h::mafAgentEventQueue.h()
//------------------------------------------------------------------------------
{
  this->Initialized     = 0;
  this->Dispatched      = true;
  this->EventQueue      = new InternalEventQueue;
  this->DispatchMode    = SelfProcessMode;
  this->DequeueMode     = MultipleEventMode;
  this->PushMode        = DispatchEventMode;
  
  vtkNEW(Mutex);
}

//------------------------------------------------------------------------------
mafAgentEventQueue.h::~mafAgentEventQueue.h()
//------------------------------------------------------------------------------
{
  //vtkGenericWarningMacro("Destroying mafAgentEventQueue.h");
  delete this->EventQueue; this->EventQueue=NULL;
  vtkDEL(Mutex);
}

//------------------------------------------------------------------------------
void mafAgentEventQueue.h::SetDispatched(bool value)
//------------------------------------------------------------------------------
{
  this->Mutex->Lock();
  this->Dispatched=value;
  this->Mutex->Unlock();
}

//------------------------------------------------------------------------------
bool mafAgentEventQueue.h::DispatchEvents()
//------------------------------------------------------------------------------
{
  if (this->Dispatched)
    return false;

  if (this->DequeueMode==MultipleEventMode)
  {
    mflEvent *event;
    unsigned long channel;
    
    mflEvent *last_event=PeekLastEvent(); // store the last event of the queue.
  
    // flush the queue but avoid infinite loops, by looping for no more than queue size
    //for (int qsize=GetQueueSize();(qsize>0)&&this->PopEvent(event,channel);qsize--)    
    do 
    {
      this->PopEvent(event,channel);
      if (event)
      {
        if (event->GetID()==mafAgentEventQueue.h::DispatchEvent)
        {
          mafAgentEventQueue.h *sender=(mafAgentEventQueue.h *)event->GetSender();
          sender->DispatchEvents();
        }
        else
        {
          if (this->DispatchMode==SelfProcessMode)
          {
            this->ProcessEvent(event,channel);
          }
          else
          {
            this->ForwardEvent(event,channel);
          }
        }
        event->UnRegister(this); // release memory
      }
    }
    while (event&&PeekEvent()!=last_event);

    //this->SetDispatched();

    Mutex->Lock();
    if (this->EventQueue->Q.size()==0)
    {
        this->SetDispatched();
    }
    else
    {
      this->RequestForDispatching();
    }
    Mutex->Unlock();
  }
  else
  {
    mflEvent *event;
    unsigned long channel;

    if (this->PopEvent(event,channel))
    {
      if (event->GetID()==mafAgentEventQueue.h::DispatchEvent)
      {
        mafAgentEventQueue.h *sender=(mafAgentEventQueue.h *)event->GetSender();
        sender->DispatchEvents();
      }
      else
      {
        if (this->DispatchMode==SelfProcessMode)
        {
          this->ProcessEvent(event,channel);
        }
        else
        {
          this->ForwardEvent(event,channel);
        }
        event->UnRegister(this); // release memory
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
int mafAgentEventQueue.h::PushEvent(unsigned long event, mafAgentEventQueue.h *sender,vtkObjectBase *data,unsigned long channel)
//------------------------------------------------------------------------------
{
  return this->PushEvent(mflSmartEvent(event,sender,data),channel);
}

//------------------------------------------------------------------------------
int mafAgentEventQueue.h::PushEvent(mflEvent *event,unsigned long channel)
//------------------------------------------------------------------------------
{
  if (event)
  {
    this->Mutex->Lock();
    event->Register(this);

    mafAgentEventQueue.h::EventQueueItem item(event,channel);

    this->EventQueue->Q.push_front(item);

    if (this->Dispatched&&this->PushMode==DispatchEventMode)
    {
      // advise listener class a new event is in the queue
      this->Dispatched=false;

      this->RequestForDispatching();
    }
    this->Mutex->Unlock();
    return true;
  }
  
  vtkErrorMacro("Trying to push a NULL event");
  return false;
}

//------------------------------------------------------------------------------
void mafAgentEventQueue.h::RequestForDispatching()
//------------------------------------------------------------------------------
{
  if (this->Listener)
    this->Listener->ProcessEvent(mflSmartEvent(mafAgentEventQueue.h::DispatchEvent,this));
}

//------------------------------------------------------------------------------
int mafAgentEventQueue.h::PopEvent(mflEvent *&event,unsigned long &channel)
//------------------------------------------------------------------------------
{
  this->Mutex->Lock();

  int ret;
  if (this->EventQueue->Q.size()>0)
  {
    mafAgentEventQueue.h::EventQueueItem item=this->EventQueue->Q.back();
    event=item.Event;
    channel=item.Channel;

    // decrease reference counting without releasing memory 
    //event->SetReferenceCount(event->GetReferenceCount()-1); 

    // Notice the reference counting is descreased by DispatchEvents
    // after dispatching, to keep the object alive.
    this->EventQueue->Q.pop_back();

    ret=true;
  }
  else
  {
    event=NULL;
    channel=0;
    ret=false;
  }

  this->Mutex->Unlock();

  return ret;

}

//------------------------------------------------------------------------------
mflEvent *mafAgentEventQueue.h::PopEvent()
//------------------------------------------------------------------------------
{
  mflEvent *ev;
  unsigned long channel;

  if (this->PopEvent(ev,channel))
  { 
    return ev;
  }

  return NULL;
}

//------------------------------------------------------------------------------
int mafAgentEventQueue.h::PopEvent(mflSmartEvent &event,unsigned long &channel)
//------------------------------------------------------------------------------
{
  mflEvent *ev;

  if (this->PopEvent(ev,channel))
  {
    event=ev;
    
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
mflEvent *mafAgentEventQueue.h::PeekEvent()
//------------------------------------------------------------------------------
{
  this->Mutex->Lock();

  mflEvent *event=NULL;
  if (this->EventQueue->Q.size()>0)
  {
    EventQueueItem item=this->EventQueue->Q.back();
    event=item.Event;  
  }
  
  this->Mutex->Unlock();

  return event;
}

//------------------------------------------------------------------------------
mflEvent *mafAgentEventQueue.h::PeekLastEvent()
//------------------------------------------------------------------------------
{
  this->Mutex->Lock();

  mflEvent *event=NULL;
  if (this->EventQueue->Q.size()>0)
  {
    EventQueueItem item=this->EventQueue->Q.front();
    event=item.Event;  
  }
  
  this->Mutex->Unlock();

  return event;
}
//------------------------------------------------------------------------------
int mafAgentEventQueue.h::GetQueueSize()
//------------------------------------------------------------------------------
{
  this->Mutex->Lock();
  return this->EventQueue->Q.size();
  this->Mutex->Unlock();
}