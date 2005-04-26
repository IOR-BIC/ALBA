/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAgentEventQueue.h,v $
  Language:  C++
  Date:      $Date: 2005-04-26 18:32:34 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafAgentEventQueue.h_h
#define __mafAgentEventQueue.h_h

#include "mflAgent.h"
#include "mflCoreWin32Header.h"
#include "vtkCriticalSection.h"

/** Base class for managing events queueing and synchronized dispatching.
  This class is thought to manage events queueing and synchronized dispatching in a multithreaded 
  events oriented architecture. An EventQueue is a mflAgent (i.e. able to
  listen to events coming from children as argument of the ProcessEvent() function), but also process
  events in the queue. Events in the queue are also processed by ProcessEvent()
  To queue events you can use the PushEvent() function. Queued events are dispatched when the
  DispatchEvents() function is called. Dispatching an event means either, to pass the dequeued event to the
  ProcessEvent() function (SelfProcessMode) or to rise a broadcast event having as "callData" the event itself
  (BroadcastMode). The default ProcessEvent() function forwards the event to the queue listener, but subclasses 
  can override this virtual method.
  The EventQueue class is also the base class for managing the synchronization. To do this, 
  when the first event is queued (i.e. the queue was previously empty), the mafAgentEventQueue.h creates
  a new event with ID "mafAgentEventQueue.h::DispatchEvent", (it's a request for dispatching) and
  sends it to its Listener (if present). This events should be managed by a dispatched object, usually
  another queue or better a threaded agent (@sa mflThreadedAgent and mafEventHandler).
  While dispatching events in the queue if a mafAgentEventQueue.h::DispatchEvent is found it not processed
  by ProcessEvent neither broadcasted, but DispatchEvents() of the sender is called. This way a manager
  can manage dispatching of events in a separate thread by simpling calling PushEvent() when it finds
  a mafAgentEventQueue.h::DispatchEvent during ProcessEvent() (see mafDeviceManager).

  When dispatching events (i.e. when DispatchEvents() is called), if the EventQueue finds
  a request for dispatching in the queue it calls the DispatchEvents() function of the sender
  (i.e. triggers the dispatching of events in that queue).
  As already stated, each dispatched event is either self processed by means of the ProcessEvent()
  function or broadcasted rising a broadcasting event on the original queue on the original channel.
  Whether self process or broadcast depends on SelfProcessMode flag (default is broadcasting).
  
  This class is the base for managing events coming from multiple sources and to perform
  synchronizations (i.e. a basic form of data fusion). More complex data fusions can be
  performed redefining the DispacthEvents function.

  @sa mflAgent mflThreadedAgent mafEventHandler*/
class MFL_CORE_EXPORT mafAgentEventQueue.h : public mflAgent
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events
      This is used by queues to manage synchronization (see class description) */
  MFL_EVT_DEC(DispatchEvent);

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

  static mafAgentEventQueue.h *New();
  vtkTypeMacro(mafAgentEventQueue.h,mflAgent);

  /**
  Push an event in the queue (FIFO)*/
  virtual int PushEvent(mflEvent *event,unsigned long channel=mflAgent::DefaultChannel);
  int PushEvent(unsigned long event_id, mafAgentEventQueue.h *sender, vtkObjectBase *data=NULL,unsigned long channel=mflAgent::DefaultChannel);

  /**
  return the first event in the queue (FIFO)*/
  mflEvent *PeekEvent();
  
  /**
  return the last event in the queue (FIFO)*/
  mflEvent *PeekLastEvent();


  /**
  Return event queue size*/
  int mafAgentEventQueue.h::GetQueueSize();

  /**
  return false if there are events in the queue*/
  bool IsQueueEmpty() {return this->GetQueueSize()==0;}

  /**
  Dispatch events in queue; */
  virtual bool DispatchEvents(); 

  /**
  Set the dequeueing modality during dispatching. Possible values are SingleEventMode
  or MultipleEventMode, i.e. dispatching one event per time or all events contemporary.*/
  vtkSetMacro(DequeueMode,int);
  vtkGetMacro(DequeueMode,int);
  void SetDequeueModeToMultipleEvent() {this->SetDequeueMode(MultipleEventMode);}
  void SetDequeueModeToSingleEvent() {this->SetDequeueMode(SingleEventMode);}

  /**
  Set the dispatch modality, i.e. what action to perform  to SingleEvent or MultipleEvent, i.e. dispatch one event
  per time or all events contemporary.*/
  vtkSetMacro(DispatchMode,int);
  vtkGetMacro(DispatchMode,int);
  void SetDispatchModeToSelfProcess() {this->SetDispatchMode(SelfProcessMode);}
  void SetDispatchModeToBroadcast() {this->SetDispatchMode(BroadcastMode);}

  /**
  Set the Push modality. If in DispatchEvent mode, the first time a new event is pushed
  in the queue, a DispatchEvent is sent to the Listener. If in polling mode no event is sent
  and events can be dispatched by explicitelly calling the DispatchEvent function.*/
  void SetPushMode(int mode) {this->PushMode=mode;}
  int GetPushMode() {return this->PushMode;}
  void SetPushModeToDispatchEvent() {this->PushMode=DispatchEventMode;}
  void SetPushModeToPolling() {this->PushMode=PollingMode;}
 
protected:
  mafAgentEventQueue.h();
  virtual ~mafAgentEventQueue.h();

  //------------------------------------------------------------------------------
  // Forward declarations
  //------------------------------------------------------------------------------
  class EventQueueItem;
  struct InternalEventQueue;

  /**
  return the first event of the queue (FIFO) and remove it from the queue. If the queue is
  empty the first function returns an event with dispatcher==NULL, the second returns "false" and set
  the event with a NULL sender.*/
  mflEvent *PopEvent();
  int PopEvent(mflSmartEvent &event,unsigned long &channel);
  virtual int PopEvent(mflEvent *&event,unsigned long &channel);

  /**
  Get/Set the Dispatched flag. This flags is true when events are in queue and not completelly dispatched 
  yet. This function is thread safe and should be used by disps triggering the dispatching of this
  object events.*/
  bool GetDispatched() {return this->Dispatched;}
  void SetDispatched(bool value=true);

  //virtual void SetListener(mflAgent *listener) {this->Superclass::SetListener(listener);}

  /**
  Internal function used to request the dispatching*/
  virtual void RequestForDispatching();

  InternalEventQueue *EventQueue;

  vtkSimpleCriticalSection *Mutex;

  int DispatchMode;
  int DequeueMode;
  int PushMode;

  bool Dispatched;

private:
  mafAgentEventQueue.h(const mafAgentEventQueue.h&);  // Not implemented.
  void operator=(const mafAgentEventQueue.h&);  // Not implemented.

};


#endif 
