/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAgentThreaded.h,v $
  Language:  C++
  Date:      $Date: 2005-04-26 18:32:34 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafAgentThreaded_h
#define __mafAgentThreaded_h

#include "mflAgent.h"
#include "mflCoreWin32Header.h"
#include "vtkMultiThreader.h"
#include "mflEventQueue.h"

//------------------------------------------------------------------------------
// Forward declarations:
//------------------------------------------------------------------------------
class vtkSimpleCriticalSection;

/** An agent processing queued events on a separate thread.
  This is a special case of agent, able to run a separate thread to process
  queued events. The class also features an asynchronous mechanism for
  forwarding (or sending) events allowing the precessing thread to send
  an event to be processed by a separate thread. This happens by queuing 
  the output event wrapped in a AsyncDispatchEvent, which is processed in
  the main thread, by using the original mflEventQueue mechanism for dispatching.
  @sa mflAgent mflEventQueue mflEvent
*/
class MFL_CORE_EXPORT mafAgentThreaded : public mflEventQueue
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  MFL_EVT_DEC(AsyncDispatchEvent);

  static mafAgentThreaded *New();
  vtkTypeMacro(mafAgentThreaded,mflEventQueue);

  /**
    Return true if this is a Agent with its own polling thread. Dispatchers requiring
    a polling thread set this variable to != 0, which makes the intialize function to 
    spawn a thread with a loop continuously calling the InternalUpdate() function.*/
  vtkGetMacro(Threaded,int);

  /**
    This is used to enable thread spawning for this Agent*/
  vtkSetMacro(Threaded,int);

  /**
    This function forces the agent to update its state. Return 0 if Update is OK.*/
  int Update() {return this->IsInitialized()?this->InternalUpdate():-1;}

  /**
    This function enques events to be processed by agent. Events are processed by agent's
    thread by means of the ProcessDispatchedEvent() function.*/
  virtual void ProcessEvent(mflEvent *event,mafID channel=mflAgent::DefaultChannel);
  
  /**
    Send an event to the specified object (i.e. Post to that listener) asynchronously. 
    This happens by queuing the output event wrapped in a AsyncDispatchEvent, 
    which is processed in the main thread, by using the original mflEventQueue 
    mechanism for dispatching.
    @sa SendEvent() for general semantic */
  void AsyncSendEvent(mflAgent *target,mflEvent *event, mafID channel=DefaultChannel);
  void AsyncSendEvent(mflAgent *target, void *sender, mafID id, mafID channel=DefaultChannel,vtkObjectBase *data=NULL) \
      {AsyncSendEvent(target,mflSmartEvent(id,sender,data),channel);}

  /**
    This function implements asynchronous forward of an event. This happens by queuing 
  the output event wrapped in a AsyncDispatchEvent, which is processed in
  the main thread, by using the original mflEventQueue mechanism for dispatching.
  @sa ForwardEvent() for general semantic */
  void AsyncForwardEvent(mflEvent *event, mafID channel=DefaultChannel);
  void AsyncForwardEvent(int id, mafID channel=DefaultChannel,vtkObjectBase *data=NULL) \
    {this->AsyncForwardEvent(mflSmartEvent(id,this,data),channel);}

protected:
  mafAgentThreaded();
  virtual ~mafAgentThreaded();

  /**
  // Internal function used to request the dispatching*/
  virtual void RequestForDispatching();

  void StopThread();

  virtual int InternalInitialize();

  virtual void InternalShutdown();

  /**
    This function must be overriden by subclasses to perform custom polling of dispatcher when
    in threaded mode. This function should poll the dispatcher and return 0. In case it returns
    a value !=0 the polling loop is stopped. The default function stops immediatelly the loop.*/
  virtual int InternalUpdate();

  /**
    Internal function to Set/Get thread struct data...*/
  void SetThreadData(vtkMultiThreader::ThreadInfo *data) {this->ThreadData=data;};
  vtkMultiThreader::ThreadInfo *GetThreadData() {return this->ThreadData;};

  /**
    Get the present value of the Thread Active flag. This function is
    thread safe and returns the value of the activeFlag memeber
    variable of the ThreadInfoStruct stored in ThreadData.*/
  int GetActiveFlag();
  
  /**
  // This funciton is used to startup the thread. Subclasses should override
  // the InternalUpdateLoop() function which is called by this one.*/
  static void *UpdateLoop(vtkMultiThreader::ThreadInfo *data);

  /**
  // Internal functions used to send a wakeup signal among the different threads*/
  void SignalNewMessage();
  void WaitForNewMessage();

  vtkMultiThreader              *Threader;
  vtkMultiThreader::ThreadInfo  *ThreadData;

  int       ThreadId;
  int       Threaded;
  int       ActiveFlag;

#ifdef _WIN32
  // Event signaling the arrival of a new message.
  // Windows implementation only.
  HANDLE MessageSignal;
#else
  // This mutex is normally locked.  It is used to block the execution 
  // of the receiving process when the send has not been called yet.
  vtkSimpleCriticalSection* Gate;
#endif

private:
  mafAgentThreaded(const mafAgentThreaded&);  // Not implemented.
  void operator=(const mafAgentThreaded&);  // Not implemented.
};

#endif 
