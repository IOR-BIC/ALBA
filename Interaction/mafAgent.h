/*=========================================================================

 Program: MAF2
 Module: mafAgent
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafAgent_h
#define __mafAgent_h

#include "mafReferenceCounted.h"
#include "mafObserver.h"
#include "mafEventSender.h"
#include "mafString.h"
#include "mafEventBase.h"

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,mafEventSource*);
#endif

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class vtkObject;
class vtkCallbackCommand;
class mafEventBroadcaster;

/** An agent is a computational object with a default I/O interface.
  An agent is a computational object with a conventional events I/O interface,
  processing its input events, taking some action and producing output events toward
  other agents.
  An agent is a mafObserver, thus an object able to listen to MAF events coming from other objects
  and extends the idea of channels keeping an internal dynamic array of channels: this was an agent
  can be attached as observer of events on any channel of other events, allowing the creation of a network 
  of agents.
  This class support also bridging of events coming from VTK: a mafAgent can be set as observer of VTK events
  which are tunneled inside MAF events.
  @sa mafEventBase mafAgent mafAgentEventQueue mafAgentThreaded */
class MAF_EXPORT mafAgent: public mafReferenceCounted, public mafObserver, public mafEventSender
{
public:
  mafAbstractTypeMacro(mafAgent,mafReferenceCounted);
  
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events
      Event used to force initialization of this object */
  MAF_ID_DEC(AGENT_INITIALIZE); 
  
  /**
  Attach/Detach this object as event source for the specified listener. If the specified channel is 
  the MCH_UP, the SetListener(listener) is called. For all other channel more then 
  one listener can be specified, and a broad cast using the VTK Subject/Observer is performed.
  UnPlugListener works for all channels at the same time.
  NULL listeners are ignored.*/
  void AddObserver(mafObserver *listener,mafID channel=MCH_UP, int priority = 0);
  void RemoveObserver(mafObserver *listener);
  void RemoveAllObservers();
  
  /**
  Process an event: the event is processed immediately and synchronously, i.e.
  the function doesn't return until the event is processed by someone. If the class
  cannot manage the event it is passed to its listeners on the same channel and so on,
  until it's processed.*/
  virtual void OnEvent(mafEventBase *event);
  
  /**  return true if there's an observer on the specified channel. (noarg == MCH_UP) */
  bool HasObservers(mafID channel);
  
  /** return a list of the observers on a channel */
  void GetObservers(mafID channel,std::vector<mafObserver *> &olist);

  /**
    Initialize this agent. Subclasses can redefine InternalInitialize() to customize
    the initialization. */
  int Initialize();

  /**
    Shutdown the agent, Subclasses can redefine InternalShutdown() to customize 
    actions for shutting down */
  void Shutdown();

  /** Return true if this agent has been initialized */
  int IsInitialized() {return m_Initialized;}
  
  /**
  The Agent name is used to identify the Agent in a unique way*/
  virtual void SetName(const char *name) {m_Name=name;}
  const char *GetName() {return m_Name;}

protected:
  mafAgent();
  virtual ~mafAgent();

  /**
  This function is overridden by subclasses to perform custom initialization*/
  virtual int InternalInitialize() {return 0;};

  /** to be redefined by subclasses to define the shutdown actions */
  virtual void InternalShutdown() {};

#ifdef MAF_USE_VTK
  /**
  Static event handler function. This function simply call the OnEvent()
  virtual function and is used to convey events from VTK observers*/
  static void InternalProcessVTKEvents(vtkObject* sender, unsigned long channel, void* clientdata, void* calldata);
#endif 

  /**
  This function make an event to be passed to observers of this class for the specified channel. In case
  of MCH_UP the message is passed to the object pointed by the "m_Listener" member variable, otherwise 
  the event is broadcasted by means of the Subject/Observer on the specified channel. */
  void InvokeEvent(mafEventBase &event, mafID channel=-1);
  void InvokeEvent(mafEventBase *event, mafID channel=-1);
  void InvokeEvent(int id, mafID channel=MCH_UP,void *data=NULL);

  mafString       m_Name;
  bool            m_Initialized; // flag set true by Initialize()

  std::vector<mafEventBroadcaster *> m_Channels;

#ifdef MAF_USE_VTK
  vtkCallbackCommand *m_EventCallbackCommand; ///< this object is used as connection to event sources
#endif 

private:
  mafAgent(const mafAgent&);  // Not implemented.
  void operator=(const mafAgent&);  // Not implemented.

};

#endif 

