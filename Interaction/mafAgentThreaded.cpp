/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: mafAgentThreaded.cpp,v $
Language:  C++
Date:      $Date: 2005-04-28 16:10:10 $
Version:   $Revision: 1.3 $



=========================================================================*/
#include "mafAgentThreaded.h"
#include "mafMutexLock.h"
#include "mmuIdFactory.h"

#include <assert.h>

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafAgentThreaded::AGENT_ASYNC_DISPATCH);

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafAgentThreaded);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafAgentThreaded::mafAgentThreaded()
//------------------------------------------------------------------------------
{
  m_Threaded      = 1;
  m_ThreadId      = -1;
  m_ThreadData    = NULL;
  m_Threader      = NULL;
  m_ActiveFlag    = 0;

  SetDispatchModeToSelfProcess();

#ifdef _WIN32
  m_MessageSignal = CreateEvent(0, FALSE, FALSE, 0);
#else
  m_Gate = new vtkSimpleCriticalSection;
  m_Gate->Lock();
#endif
}

//------------------------------------------------------------------------------
mafAgentThreaded::~mafAgentThreaded()
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
void mafAgentThreaded::InternalShutdown()
//------------------------------------------------------------------------------
{
  StopThread();
}

//------------------------------------------------------------------------------
void mafAgentThreaded::StopThread()
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
int mafAgentThreaded::InternalInitialize()
//------------------------------------------------------------------------------
{
  // Spawns a thread if necessary
  if (m_Threaded)
  {
    if (m_ActiveFlag || m_ThreadId != -1) 
    {
      mafErrorMacro("Dispatcher handler polling thread already started!");
      return -1;
    }

    m_Threader = new mafMultiThreader;
    m_ActiveFlag=1;

    m_ThreadId = m_Threader->SpawnThread(&mafAgentThreaded::UpdateLoop,this);    
  }

  return 0;
}

//------------------------------------------------------------------------------
int mafAgentThreaded::GetActiveFlag()
//------------------------------------------------------------------------------
{
  return m_ActiveFlag;
}

      
//------------------------------------------------------------------------------
void mafAgentThreaded::UpdateLoop(mmuThreadInfoStruct *data)
//------------------------------------------------------------------------------
{
  mafAgentThreaded *self = (mafAgentThreaded *)(data->m_UserData);

  self->m_ThreadData=data;

  // wait for initialization to be completed
  for (;self->GetActiveFlag()&&!self->IsInitialized();)
    Sleep(100); 

  // This loop is termintated when active flag is set by TerminateThread() or 
  // if InternalUpdate() returns a value !=0
  for (;self->GetActiveFlag()&&!self->InternalUpdate();) ; // active loop

  self->m_ThreadData=NULL;

  self->m_ThreadId=-1;
}

//------------------------------------------------------------------------------
int mafAgentThreaded::InternalUpdate()
//------------------------------------------------------------------------------
{
  this->WaitForNewMessage();

  this->DispatchEvents();
  
  return 0;
}

//------------------------------------------------------------------------------
void mafAgentThreaded::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);

  if (event->GetId()==AGENT_ASYNC_DISPATCH)
  {
    if (event->GetSender()==this)
    {
      // this is used for the asynchronous ForwardEvent
      this->ForwardEvent((mafEventBase *)event->GetData(),event->GetChannel());
    }
    else
    {
      assert(true); // should not pass from here
      
      // this is used for the asynchronous SendEvent
      // notice we are using the sender field of the AsyncForwardEvent to store the recipient
      //((mafObserver *)event->GetSender())->OnEvent((mafEventBase *)event->GetData());
    }
  }
}

//------------------------------------------------------------------------------
void mafAgentThreaded::RequestForDispatching()
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
void mafAgentThreaded::AsyncForwardEvent(mafEventBase *event, mafID channel)
//------------------------------------------------------------------------------
{
  mafID old_ch=event->GetChannel();
  event->SetChannel(channel);
  PushEvent(AGENT_ASYNC_DISPATCH,this,event); // this make a copy of the event
  event->SetChannel(old_ch);
}

//------------------------------------------------------------------------------
void mafAgentThreaded::AsyncSendEvent(mafObserver *target,mafEventBase *event, mafID channel)
//------------------------------------------------------------------------------
{
  if (event&&target&&target!=this)
  {
    // use the sender field to store the recipient.
    mafID old_ch=event->GetChannel();
    event->SetChannel(channel); // set the right channel
    PushEvent(AGENT_ASYNC_DISPATCH,target,event);
    event->SetChannel(old_ch);
  }
}

//----------------------------------------------------------------------------
void mafAgentThreaded::AsyncForwardEvent(mafID id, mafID channel,void *data)
//----------------------------------------------------------------------------
{
  AsyncForwardEvent(&mafEventBase(this,id,data,channel),channel);
}

//----------------------------------------------------------------------------
void mafAgentThreaded::WaitForNewMessage()
//------------------------------------------------------------------------------
{
#ifdef _WIN32
  WaitForSingleObject( m_MessageSignal, INFINITE );
#else
  m_Gate->Lock();
#endif
}

//----------------------------------------------------------------------------
void mafAgentThreaded::SignalNewMessage()
//------------------------------------------------------------------------------
{
#ifdef _WIN32
  SetEvent( m_MessageSignal );
#else
  m_Gate->Unlock();
#endif
}
//----------------------------------------------------------------------------
void AsyncSendEvent(mafObserver *target, void *sender, mafID id, mafID channel=MCH_UP,void *data=NULL)
//----------------------------------------------------------------------------
{
  AsyncSendEvent(target,&mafEventBase(sender,id,data,channel),channel);
}