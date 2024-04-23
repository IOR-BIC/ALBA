/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAgentThreaded
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaAgentThreaded.h"
#include "albaMutexLock.h"
#include "mmuIdFactory.h"

#include <assert.h>

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
ALBA_ID_IMP(albaAgentThreaded::AGENT_ASYNC_DISPATCH);

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaAgentThreaded);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaAgentThreaded::albaAgentThreaded()
//------------------------------------------------------------------------------
{
  m_Threaded      = 1;
  m_ThreadId      = -1;
  m_ThreadData    = NULL;
  m_Threader      = NULL;
  m_ActiveFlag    = 0;

  SetDispatchModeToSelfProcess();

#ifdef _WIN32
  m_MessageSignal = CreateEvent(0, false, false, 0);
#else
  m_Gate = new albaMutexLock;
  m_Gate->Lock();
#endif
}

//------------------------------------------------------------------------------
albaAgentThreaded::~albaAgentThreaded()
//------------------------------------------------------------------------------
{
  Shutdown();
#ifdef _WIN32
  CloseHandle(m_MessageSignal);
#else
  delete m_Gate;
#endif
}

//------------------------------------------------------------------------------
void albaAgentThreaded::InternalShutdown()
//------------------------------------------------------------------------------
{
  StopThread();
}

//------------------------------------------------------------------------------
void albaAgentThreaded::StopThread()
//------------------------------------------------------------------------------
{
  if (m_ThreadId!=-1)
  {
    if (m_ActiveFlag)
    {
      m_ActiveFlag=0;
      this->SignalNewMessage(); //awoke the thread to make it die
    }
    // terminate the thread
    m_Threader->TerminateThread(m_ThreadId);
    m_ThreadId = -1;

    delete m_Threader;
  }
}

//------------------------------------------------------------------------------
int albaAgentThreaded::InternalInitialize()
//------------------------------------------------------------------------------
{
  // Spawns a thread if necessary
  if (m_Threaded)
  {
    if (m_ActiveFlag || m_ThreadId != -1) 
    {
      albaErrorMacro("Dispatcher handler polling thread already started!");
      return -1;
    }

    m_Threader = new albaMultiThreader;
    m_ActiveFlag=1;

    m_ThreadId = m_Threader->SpawnThread(&albaAgentThreaded::UpdateLoop,this);    
  }

  return 0;
}

//------------------------------------------------------------------------------
int albaAgentThreaded::GetActiveFlag()
//------------------------------------------------------------------------------
{
  return m_ActiveFlag;
}

      
//------------------------------------------------------------------------------
void albaAgentThreaded::UpdateLoop(mmuThreadInfoStruct *data)
//------------------------------------------------------------------------------
{
  albaAgentThreaded *self = (albaAgentThreaded *)(data->m_UserData);

  self->m_ThreadData=data;

  // wait for initialization to be completed
  for (;self->GetActiveFlag()&&!self->IsInitialized();)
    albaSleep(100); 

  // This loop is termintated when active flag is set by TerminateThread() or 
  // if InternalUpdate() returns a value !=0
  for (;self->GetActiveFlag()&&!self->InternalUpdate();) ; // active loop

  self->m_ThreadData=NULL;

  self->m_ThreadId=-1;
}

//------------------------------------------------------------------------------
int albaAgentThreaded::InternalUpdate()
//------------------------------------------------------------------------------
{
  this->WaitForNewMessage();

  this->DispatchEvents();
  
  return 0;
}

//------------------------------------------------------------------------------
void albaAgentThreaded::OnEvent(albaEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);

  if (event->GetId()==AGENT_ASYNC_DISPATCH)
  {
    if (event->GetSender()==this)
    {
      // this is used for the asynchronous InvokeEvent
      this->InvokeEvent((albaEventBase *)event->GetData());
    }
    else
    {
      assert(true); // should not pass from here
      
      // this is used for the asynchronous SendEvent
      // notice we are using the sender field of the AsyncInvokeEvent to store the recipient
      //((albaObserver *)event->GetSender())->OnEvent((albaEventBase *)event->GetData());
    }
  }
  else
  {
    Superclass::OnEvent(event);
  }
}

//------------------------------------------------------------------------------
void albaAgentThreaded::RequestForDispatching()
//------------------------------------------------------------------------------
{
  //if (m_Threaded&&this->Initialized&&this->GetActiveFlag())
  //{
    if (PeekLastEvent()->GetId()==AGENT_ASYNC_DISPATCH)
    {
      Superclass::RequestForDispatching();
    }
    else
    {
      // send my self an event to awoke my thread
      this->SignalNewMessage();
    }
  //}
  //else
  //{
  //  this->DispatchEvents();
  //}

}

//------------------------------------------------------------------------------
void albaAgentThreaded::AsyncInvokeEvent(albaEventBase *event, albaID channel)
//------------------------------------------------------------------------------
{
  albaEventBase *copy_of_event = event->NewInstance();
  copy_of_event->DeepCopy(event);
  copy_of_event->SetChannel(channel);
  PushEvent(AGENT_ASYNC_DISPATCH,this,copy_of_event); // this make a copy of the event
  albaDEL(copy_of_event);
}

//------------------------------------------------------------------------------
void albaAgentThreaded::AsyncSendEvent(albaObserver *target,albaEventBase *event, albaID channel)
//------------------------------------------------------------------------------
{
  if (event&&target&&target!=this)
  {
    // use the sender field to store the recipient.
    albaID old_ch=event->GetChannel();
    event->SetChannel(channel); // set the right channel
    PushEvent(AGENT_ASYNC_DISPATCH,target,event);
    event->SetChannel(old_ch);
  }
}

//----------------------------------------------------------------------------
void albaAgentThreaded::AsyncInvokeEvent(albaID id, albaID channel,void *data)
//----------------------------------------------------------------------------
{
  AsyncInvokeEvent(&albaEventBase(this,id,data,channel),channel);
}

//----------------------------------------------------------------------------
void albaAgentThreaded::WaitForNewMessage()
//------------------------------------------------------------------------------
{
#ifdef _WIN32
  WaitForSingleObject( m_MessageSignal, INFINITE );
#else
  m_Gate->Lock();
#endif
}

//----------------------------------------------------------------------------
void albaAgentThreaded::SignalNewMessage()
//------------------------------------------------------------------------------
{
#ifdef _WIN32
  SetEvent( m_MessageSignal );
#else
  m_Gate->Unlock();
#endif
}
//----------------------------------------------------------------------------
void albaAgentThreaded::AsyncSendEvent(albaObserver *target, void *sender, albaID id, albaID channel,void *data)
//----------------------------------------------------------------------------
{
  AsyncSendEvent(target,&albaEventBase(sender,id,data,channel),channel);
}
