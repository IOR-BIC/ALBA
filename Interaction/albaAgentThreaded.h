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

#ifndef __albaAgentThreaded_h
#define __albaAgentThreaded_h

#include "albaAgentEventQueue.h"
#include "albaMultiThreader.h"

//------------------------------------------------------------------------------
// Forward declarations:
//------------------------------------------------------------------------------
class albaMutexLock;

/** An agent processing the queued events on a separate thread.
  This is a special case of agent, able to run a separate thread for processing
  queued events. The class also features an asynchronous mechanism for
  sending events to observers, allowing the internal thread to send
  an event to be processed by a separate thread. This happens by queuing 
  the output event wrapped in a AGENT_ASYNC_DISPATCH, which is later on processed in
  the main thread, by using the original albaAgentEventQueue mechanism for dispatching.
  Notice that, the asynchronous mechanism requires the  of this class 
  to be called on a different thread. Dispatching on another thread is therefore 
  responsibility of a different class, specifically designed to process it incoming events
  on a different tread (e.g. albaAgentEventHandler).
  The albaAgentThreaded DispatchEvents() will take care of extracting the message contained 
  inside a AGENT_ASYNC_DISPATCH event type and dispatch it.
  @sa albaAgent albaAgentEventQueue albaEventBase
*/
class ALBA_EXPORT albaAgentThreaded : public albaAgentEventQueue
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  ALBA_ID_DEC(AGENT_ASYNC_DISPATCH);

  albaTypeMacro(albaAgentThreaded,albaAgentEventQueue);

  /**
    Return true if this is a Agent with its own polling thread. Dispatchers requiring
    a polling thread set this variable to != 0, which makes the initialize function to 
    spawn a thread with a loop continuously calling the InternalUpdate() function.*/
  int GetThreaded() {return m_Threaded;}

  /**
    This is used to enable thread spawning for this Agent*/
  void SetThreaded(int flag){m_Threaded=flag;} 

  /**
    This function forces the agent to update its state. Return 0 if Update is OK.*/
  int Update() {return IsInitialized()?InternalUpdate():-1;}

  /**
    This function enques events to be processed by agent. Events are processed by agent's
    thread by means of the ProcessDispatchedEvent() function.*/
  virtual void OnEvent(albaEventBase *event);
  
  /**
    Send an event to the specified object (i.e. Post to that listener) asynchronously. 
    This happens by queuing the output event wrapped in a AGENT_ASYNC_DISPATCH, 
    which is processed in the main thread, by using the original albaAgentEventQueue 
    mechanism for dispatching.
    @sa SendEvent() for general semantic */
  void AsyncSendEvent(albaObserver *target,albaEventBase *event, albaID channel=MCH_UP);
  void AsyncSendEvent(albaObserver *target, void *sender, albaID id, albaID channel=MCH_UP,void *data=NULL);

  /**
    This function implements asynchronous forward of an event. This happens by queuing 
    the output event wrapped in a AGENT_ASYNC_DISPATCH, which is processed in
    the main thread, by using the original albaAgentEventQueue mechanism for dispatching.
    @sa InvokeEvent() for general semantic */
  void AsyncInvokeEvent(albaEventBase *event, albaID channel=MCH_UP);
  void AsyncInvokeEvent(albaID id, albaID channel=MCH_UP,void *data=NULL);

protected:
  albaAgentThreaded();
  virtual ~albaAgentThreaded();

  /** Internal function used to request the dispatching*/
  virtual void RequestForDispatching();
  void StopThread();
  virtual int InternalInitialize();
  virtual void InternalShutdown();

  /**
    This function must be overridden by subclasses to perform custom polling of dispatcher when
    in threaded mode. This function should poll the dispatcher and return 0. In case it returns
    a value !=0 the polling loop is stopped. The default function stops immediately the loop.*/
  virtual int InternalUpdate();

  /**
   This function is used to startup the thread. Subclasses should override
   the InternalUpdateLoop() function which is called by this one.*/
  static void UpdateLoop(mmuThreadInfoStruct *data);
    
  /**
    Get the present value of the Thread Active flag. This function is
    thread safe and returns the value of the activeFlag member
    variable of the ThreadInfoStruct stored in ThreadData.*/
  int GetActiveFlag();

  /**
  // Internal functions used to send a wakeup signal among the different threads*/
  void SignalNewMessage();
  void WaitForNewMessage();

  albaMultiThreader*     m_Threader;
  mmuThreadInfoStruct*  m_ThreadData;

  int                   m_ThreadId;
  int                   m_Threaded;
  int                   m_ActiveFlag;

#ifdef _WIN32
 
  /** Event signaling the arrival of a new message. Windows implementation only. */
  HANDLE        m_MessageSignal;
#else
  
  /** This mutex is normally locked. It is used to block the execution of the receiving process when the send has not been called yet. */
  albaMutexLock* m_Gate;
#endif

private:
  albaAgentThreaded(const albaAgentThreaded&);  // Not implemented.
  void operator=(const albaAgentThreaded&);  // Not implemented.
};

#endif 
