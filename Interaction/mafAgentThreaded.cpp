/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: mafAgentThreaded.cpp,v $
Language:  C++
Date:      $Date: 2005-04-26 18:32:34 $
Version:   $Revision: 1.1 $



=========================================================================*/
#include "mafAgentThreaded.h"
#include "vtkObjectFactory.h"
#include "mflDefines.h"
#include "vtkCriticalSection.h"

#include <assert.h>

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MFL_EVT_IMP(mafAgentThreaded::AsyncDispatchEvent);

//------------------------------------------------------------------------------
vtkStandardNewMacro(mafAgentThreaded)

//------------------------------------------------------------------------------
mafAgentThreaded::mafAgentThreaded()
//------------------------------------------------------------------------------
{
  this->Threaded      = 1;
  this->ThreadId      = -1;
  this->ActiveFlag    = 0;
  this->ThreadData    = NULL;
  this->Threader      = NULL;
  this->SetDispatchModeToSelfProcess();

#ifdef _WIN32
  this->MessageSignal = CreateEvent(0, FALSE, FALSE, 0);
#else
  this->Gate = new vtkSimpleCriticalSection;
  this->Gate->Lock();
#endif
}

//------------------------------------------------------------------------------
mafAgentThreaded::~mafAgentThreaded()
//------------------------------------------------------------------------------
{
  Shutdown();
#ifdef _WIN32
  CloseHandle(this->MessageSignal);
#else
  delete this->Gate;
#endif
}

//------------------------------------------------------------------------------
void mafAgentThreaded::InternalShutdown()
//------------------------------------------------------------------------------
{
  if (this->Initialized)
    this->StopThread();
}

//------------------------------------------------------------------------------
void mafAgentThreaded::StopThread()
//------------------------------------------------------------------------------
{
  if (this->ThreadId!=-1)
  {
    if (this->ActiveFlag)
    {
      this->ActiveFlag=0;
      this->SignalNewMessage(); //awoke the thread to make it die
    }
    // terminate the thread
    this->Threader->TerminateThread(this->ThreadId);
    this->ThreadId = -1;

    vtkDEL(this->Threader);
  }
}

//------------------------------------------------------------------------------
int mafAgentThreaded::InternalInitialize()
//------------------------------------------------------------------------------
{
  // Spawns a thread if necessary
  if (this->Threaded)
  {
    if (this->ActiveFlag || this->ThreadId != -1) 
    {
      vtkErrorMacro("Dispatcher handler polling thread already started!");
      return -1;
    }

    vtkNEW(this->Threader);
    this->ActiveFlag=1;
    this->ThreadId = this->Threader->SpawnThread((vtkThreadFunctionType) \
                       &mafAgentThreaded::UpdateLoop,this);    
  }

  return 0;
}

//------------------------------------------------------------------------------
int mafAgentThreaded::GetActiveFlag()
//------------------------------------------------------------------------------
{
  int activeFlag = *(this->ThreadData->ActiveFlag);
  return activeFlag;
}

      
//------------------------------------------------------------------------------
void *mafAgentThreaded::UpdateLoop(vtkMultiThreader::ThreadInfo *data)
//------------------------------------------------------------------------------
{
  mafAgentThreaded *self = (mafAgentThreaded *)(data->UserData);

  self->SetThreadData(data);

  // wait for initialization to be completed
  for (;self->GetActiveFlag()&&!self->IsInitialized();)
    Sleep(100); 

  // This loop is termintated when active flag is set by TerminateThread() or 
  // if InternalUpdate() returns a value !=0
  for (;self->GetActiveFlag()&&!self->InternalUpdate();) ; // active loop

  self->SetThreadData(NULL);

  self->ThreadId=-1;

  return NULL;
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
void mafAgentThreaded::ProcessEvent(mflEvent *event,unsigned long channel)
//------------------------------------------------------------------------------
{
  assert(event);

  if (event->GetID()==AsyncDispatchEvent)
  {
    if (event->GetSender()==this)
    {
      // this is used for the asynchronous ForwardEvent
      this->ForwardEvent((mflEvent *)event->GetData(),channel);
    }
    else
    {
      assert(true);
      // this is used for the asynchronous SendEvent
      // notice we are using the sender field of the AsyncForwardEvent to store the recipient
      this->SendEvent((mflAgent *)event->GetSender(),(mflEvent *)event->GetData(),channel);
    }
  }
}

//------------------------------------------------------------------------------
void mafAgentThreaded::RequestForDispatching()
//------------------------------------------------------------------------------
{
  //if (this->Threaded&&this->Initialized&&this->GetActiveFlag())
  //{
    if (PeekLastEvent()->GetID()==AsyncDispatchEvent)
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
void mafAgentThreaded::AsyncForwardEvent(mflEvent *event, unsigned long channel)
//------------------------------------------------------------------------------
{
  this->PushEvent(mflSmartEvent(AsyncDispatchEvent,this,event),channel);
}

//------------------------------------------------------------------------------
void mafAgentThreaded::AsyncSendEvent(mflAgent *target,mflEvent *event, unsigned long channel)
//------------------------------------------------------------------------------
{
  if (event&&target&&target!=this)
  {
    // use the sender field to store the recipient.
    this->PushEvent(mflSmartEvent(AsyncDispatchEvent,target,event),channel);
  }
}

//----------------------------------------------------------------------------
void mafAgentThreaded::WaitForNewMessage()
//------------------------------------------------------------------------------
{
#ifdef _WIN32
  WaitForSingleObject( this->MessageSignal, INFINITE );
#else
  this->Gate->Lock();
#endif
}

//----------------------------------------------------------------------------
void mafAgentThreaded::SignalNewMessage()
//------------------------------------------------------------------------------
{
#ifdef _WIN32
  SetEvent( this->MessageSignal );
#else
  this->Gate->Unlock();
#endif
}

