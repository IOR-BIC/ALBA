/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAgentEventQueue
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaAgentEventQueue_h
#define __albaAgentEventQueue_h

#include "albaAgent.h"


//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaMutexLock;

/** Base class for managing event queueing and synchronized dispatching.
  This class is thought to manage events queueing and synchronized dispatching in a multithreaded 
  events oriented architecture. An EventQueue is a albaAgent (i.e. able to
  listen to events coming from other objects and passed as arguments of the OnEvent() function), but it can
  also process events in its queue. Events in the queue are also processed by OnEvent(), while
  to queue events you can use the PushEvent() function. Queued events are dispatched when the
  DispatchEvents() function is called. Dispatching an event means either to pass the dequeued events to the
  OnEvent() function (SelfProcessMode) or to rise a broadcast event having as "callData" the event itself
  (BroadcastMode). The default OnEvent() function forwards the event to the queue listener, but subclasses 
  can override this virtual method.
  The EventQueue class is also the base class for managing the synchronization. To do this, 
  when the first event is queued (i.e. the queue was previously empty), the albaAgentEventQueue creates
  a new event with ID "albaAgentEventQueue::EVENT_DISPATCH", (it's a request for dispatching) and
  sends it to its Listener (if present). This event should be managed by a dispatcher object, usually
  another queue or better a threaded agent (@sa mflThreadedAgent and albaEventHandler).
  If a albaAgentEventQueue::EVENT_DISPATCH is found, while dispatching events in the queue, it's not processed
  by OnEvent and neither broadcasted, but the DispatchEvents() function of the sender object is called.
  Dispatching of events in a separate thread can be performed by calling PushEvent() when an incoming event
  is found to be a albaAgentEventQueue::EVENT_DISPATCH during the OnEvent() (see albaDeviceManager).

  When dispatching events (i.e. when DispatchEvents() is called), if the EventQueue finds
  a request for dispatching in the queue it calls the DispatchEvents() function of the sender
  (i.e. triggers the dispatching of events in that queue).
  As already stated, each dispatched event is either self processed by means of the OnEvent()
  function or broadcasted rising a broadcasting event on the original queue on the original channel.
  Whether self process or broadcast depends on SelfProcessMode flag (default is broadcasting).
  
  This class is the base for managing events coming from multiple sources and to perform
  synchronizations (i.e. a basic form of data fusion). More complex data fusions can be
  performed redefining the DispacthEvents function.

  @sa albaAgent albaAgentThreaded albaAgentEventHandler*/
class ALBA_EXPORT albaAgentEventQueue : public albaAgent
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events
      This is used by queues to manage synchronization (see class description) */
  ALBA_ID_DEC(EVENT_DISPATCH);

  enum DispatchModalities {
    SelfProcessMode = 0,
    BroadcastMode
  };

  enum PopModalities {
    SingleEventMode = 0,
    MultipleEventMode
  };

  enum PushModalities {
    DispatchEventMode=0,
    PollingMode
  };

  albaTypeMacro(albaAgentEventQueue,albaAgent);

  /** Push an event in the queue (FIFO)*/
  bool PushEvent(albaEventBase &event) {return PushEvent(&event);}
  virtual bool PushEvent(albaEventBase *event);
  bool PushEvent(albaID event_id, void *sender, void *data=NULL);

  /**
  return the first event in the queue (FIFO)*/
  albaEventBase *PeekEvent();
  
  /** return the last event in the queue (FIFO)*/
  albaEventBase *PeekLastEvent();

  /** Return event queue size*/
  int GetQueueSize();

  /** return false if there are events in the queue*/
  bool IsQueueEmpty() {return this->GetQueueSize()==0;}

  /** Dispatch events in queue; */
  virtual bool DispatchEvents(); 

  /**
  Set the dequeuing modality during dispatching. Possible values are SingleEventMode
  or MultipleEventMode, i.e. dispatching one event per time or all events contemporary.*/
  void SetDequeueMode(int mode) {m_DequeueMode=mode;}
  int GetDequeueMode() {return m_DequeueMode;}
  void SetDequeueModeToMultipleEvent() {SetDequeueMode(MultipleEventMode);}
  void SetDequeueModeToSingleEvent() {SetDequeueMode(SingleEventMode);}

  /**
  Set the dispatch modality, i.e. what action to perform  to SingleEvent or MultipleEvent, i.e. dispatch one event
  per time or all events contemporary.*/
  void SetDispatchMode(int mode) {m_DispatchMode=mode;}
  int GetDispatchMode() {return m_DispatchMode;}
  void SetDispatchModeToSelfProcess() {SetDispatchMode(SelfProcessMode);}
  void SetDispatchModeToBroadcast() {SetDispatchMode(BroadcastMode);}

  /**
  Set the Push modality. If in EVENT_DISPATCH mode, the first time a new event is pushed
  in the queue, a EVENT_DISPATCH is sent to the Listener. If in polling mode no event is sent
  and events can be dispatched by explicitly calling the EVENT_DISPATCH function.*/
  void SetPushMode(int mode) {m_PushMode=mode;}
  int GetPushMode() {return m_PushMode;}
  void SetPushModeToDispatchEvent() {m_PushMode=DispatchEventMode;}
  void SetPushModeToPolling() {m_PushMode=PollingMode;}
 
protected:
  albaAgentEventQueue();
  virtual ~albaAgentEventQueue();

  //------------------------------------------------------------------------------
  // Forward declarations
  //------------------------------------------------------------------------------
  class EventQueueItem;
  struct InternalEventQueue;

  /**
  return the first event of the queue (FIFO) and remove it from the queue. If the queue is
  empty the first function returns an event with dispatcher==NULL, the second returns "false" and set
  the event with a NULL sender.*/
  albaEventBase *PopEvent();
  int PopEvent(albaEventBase &event);
  virtual int PopEvent(albaEventBase *&event);

  /**
  Get/Set the Dispatched flag. This flags is true when events are in queue and not completelly dispatched 
  yet. This function is thread safe and should be used by disps triggering the dispatching of this
  object events.*/
  bool GetDispatched() {return m_Dispatched;}
  void SetDispatched(bool value=true);

  //virtual void SetListener(albaAgent *listener) {this->Superclass::SetListener(listener);}

  /**
  Internal function used to request the dispatching*/
  virtual void RequestForDispatching();

  InternalEventQueue *m_EventQueue;

  albaMutexLock *m_Mutex;

  int m_DispatchMode;
  int m_DequeueMode;
  int m_PushMode;

  bool m_Dispatched;

private:
  albaAgentEventQueue(const albaAgentEventQueue&);  // Not implemented.
  void operator=(const albaAgentEventQueue&);  // Not implemented.

};


#endif 
