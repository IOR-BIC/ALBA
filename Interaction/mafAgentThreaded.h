/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAgentThreaded.h,v $
  Language:  C++
  Date:      $Date: 2005-04-27 16:56:03 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafAgentThreaded_h
#define __mafAgentThreaded_h

#include "mafAgentEventQueue.h"
#include "mafMultiThreader.h"

//------------------------------------------------------------------------------
// Forward declarations:
//------------------------------------------------------------------------------
class mafMutexLock;

/** An agent processing queued events on a separate thread.
  This is a special case of agent, able to run a separate thread to process
  queued events. The class also features an asynchronous mechanism for
  forwarding (or sending) events allowing the precessing thread to send
  an event to be processed by a separate thread. This happens by queuing 
  the output event wrapped in a AsyncDispatchEvent, which is processed in
  the main thread, by using the original mafAgentEventQueue mechanism for dispatching.
  @sa mafAgent mafAgentEventQueue mafEventBase
*/
class MAF_EXPORT mafAgentThreaded : public mafAgentEventQueue
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  MAF_ID_DEC(AsyncDispatchEvent);

  mafTypeMacro(mafAgentThreaded,mafAgentEventQueue);

  /**
    Return true if this is a Agent with its own polling thread. Dispatchers requiring
    a polling thread set this variable to != 0, which makes the intialize function to 
    spawn a thread with a loop continuously calling the InternalUpdate() function.*/
  int GetThreaded() {return m_Threaded;}

  /**
    This is used to enable thread spawning for this Agent*/
  void SetThreaded(int flag){m_Threaded=flag;} 

  /**
    This function forces the agent to update its state. Return 0 if Update is OK.*/
  int Update() {return this->IsInitialized()?this->InternalUpdate():-1;}

  /**
    This function enques events to be processed by agent. Events are processed by agent's
    thread by means of the ProcessDispatchedEvent() function.*/
  virtual void OnEvent(mafEventBase *event);
  
  /**
    Send an event to the specified object (i.e. Post to that listener) asynchronously. 
    This happens by queuing the output event wrapped in a AsyncDispatchEvent, 
    which is processed in the main thread, by using the original mafAgentEventQueue 
    mechanism for dispatching.
    @sa SendEvent() for general semantic */
  void AsyncSendEvent(mafObserver *target,mafEventBase *event, mafID channel=MCH_UP);
  void AsyncSendEvent(mafObserver *target, void *sender, mafID id, mafID channel=MCH_UP,void *data=NULL);

  /**
    This function implements asynchronous forward of an event. This happens by queuing 
    the output event wrapped in a AsyncDispatchEvent, which is processed in
    the main thread, by using the original mafAgentEventQueue mechanism for dispatching.
    @sa ForwardEvent() for general semantic */
  void AsyncForwardEvent(mafEventBase *event, mafID channel=MCH_UP);
  void AsyncForwardEvent(mafID id, mafID channel=MCH_UP,void *data=NULL);

protected:
  mafAgentThreaded();
  virtual ~mafAgentThreaded();

  /** Internal function used to request the dispatching*/
  virtual void RequestForDispatching();
  void StopThread();
  virtual int InternalInitialize();
  virtual void InternalShutdown();

  /**
    This function must be overriden by subclasses to perform custom polling of dispatcher when
    in threaded mode. This function should poll the dispatcher and return 0. In case it returns
    a value !=0 the polling loop is stopped. The default function stops immediately the loop.*/
  virtual int InternalUpdate();

  /**
   This funciton is used to startup the thread. Subclasses should override
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

  mafMultiThreader*     m_Threader;
  mmuThreadInfoStruct*  m_ThreadData;

  int                   m_ThreadId;
  int                   m_Threaded;
  int                   m_ActiveFlag;

#ifdef _WIN32
 
  /** Event signaling the arrival of a new message. Windows implementation only. */
  HANDLE        m_MessageSignal;
#else
  
  /** This mutex is normally locked. It is used to block the execution of the receiving process when the send has not been called yet. */
  mafMutexLock* m_Gate;
#endif

private:
  mafAgentThreaded(const mafAgentThreaded&);  // Not implemented.
  void operator=(const mafAgentThreaded&);  // Not implemented.
};

#endif 
