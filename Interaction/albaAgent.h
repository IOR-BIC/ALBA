/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAgent
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaAgent_h
#define __albaAgent_h

#include "albaReferenceCounted.h"
#include "albaObserver.h"
#include "albaEventBroadcaster.h"
#include "albaString.h"
#include "albaEventBase.h"

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT, albaEventBroadcaster *);
#endif

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class vtkObject;
class vtkCallbackCommand;
class albaEventBroadcaster;

/** An agent is a computational object with a default I/O interface.
  An agent is a computational object with a conventional events I/O interface,
  processing its input events, taking some action and producing output events toward
  other agents.
  An agent is a albaObserver, thus an object able to listen to ALBA events coming from other objects
  and extends the idea of channels keeping an internal dynamic array of channels: this was an agent
  can be attached as observer of events on any channel of other events, allowing the creation of a network 
  of agents.
  This class support also bridging of events coming from VTK: a albaAgent can be set as observer of VTK events
  which are tunneled inside ALBA events.
  @sa albaEventBase albaAgent albaAgentEventQueue albaAgentThreaded */
class ALBA_EXPORT albaAgent: public albaReferenceCounted, public albaObserver, public albaEventBroadcaster
{
public:
  albaAbstractTypeMacro(albaAgent,albaReferenceCounted);
  
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events
      Event used to force initialization of this object */
  ALBA_ID_DEC(AGENT_INITIALIZE); 
  
  /**
  Attach/Detach this object as event source for the specified listener. If the specified channel is 
  the MCH_UP, the SetListener(listener) is called. For all other channel more then 
  one listener can be specified, and a broad cast using the VTK Subject/Observer is performed.
  UnPlugListener works for all channels at the same time.
  NULL listeners are ignored.*/
  void AddObserver(albaObserver *listener,albaID channel=MCH_UP);
  void RemoveObserver(albaObserver *listener);
  void RemoveAllObservers();
  
  /**
  Process an event: the event is processed immediately and synchronously, i.e.
  the function doesn't return until the event is processed by someone. If the class
  cannot manage the event it is passed to its listeners on the same channel and so on,
  until it's processed.*/
  virtual void OnEvent(albaEventBase *event);
  
  /**  return true if there's an observer on the specified channel. (noarg == MCH_UP) */
  bool HasObservers(albaID channel);
  
  /** return a list of the observers on a channel */
  void GetObservers(albaID channel,std::vector<albaObserver *> &olist);

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
  albaAgent();
  virtual ~albaAgent();

  /**
  This function is overridden by subclasses to perform custom initialization*/
  virtual int InternalInitialize() {return 0;};

  /** to be redefined by subclasses to define the shutdown actions */
  virtual void InternalShutdown() {};

#ifdef ALBA_USE_VTK
  /**
  Static event handler function. This function simply call the OnEvent()
  virtual function and is used to convey events from VTK observers*/
  static void InternalProcessVTKEvents(vtkObject* sender, unsigned long channel, void* clientdata, void* calldata);
#endif 

  /**
  This function make an event to be passed to observers of this class for the specified channel. In case
  of MCH_UP the message is passed to the object pointed by the "m_Listener" member variable, otherwise 
  the event is broadcasted by means of the Subject/Observer on the specified channel. */
  void InvokeEvent(albaEventBase &event, albaID channel=-1);
  void InvokeEvent(albaEventBase *event, albaID channel=-1);
  void InvokeEvent(void *sender, int id, albaID channel=MCH_UP,void *data=NULL);

  albaString       m_Name;
  bool            m_Initialized; // flag set true by Initialize()

  std::vector<albaEventBroadcaster *> m_Channels;

#ifdef ALBA_USE_VTK
  vtkCallbackCommand *m_EventCallbackCommand; ///< this object is used as connection to event sources
#endif 

private:
  albaAgent(const albaAgent&);  // Not implemented.
  void operator=(const albaAgent&);  // Not implemented.

};

#endif 

