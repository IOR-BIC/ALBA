/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAgent.h,v $
  Language:  C++
  Date:      $Date: 2005-04-27 16:56:03 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafAgent_h
#define __mafAgent_h

#include "mafObject.h"
#include "mafObserver.h"
#include "mafString.h"
#include "mafEventBase.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class vtkObject;
class vtkCallbackCommand;

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
class MAF_EXPORT mafAgent: public mafObject, public mafObserver//, public mafEventSender
{
public:
  mafAbstractTypeMacro(mafAgent,mafObject);
  
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events
      Event used to force initialization of this object */
  MAF_ID_DEC(InitializeEvent); 
 
  /**
  Plug in a source of events, specifying the channel ID. All the mechanism is based on Observers, 
  and the channel ID correspond to the event ID used to convey events through observers.
  Beware, before destroying the Listener remember to UnPlug the source since the listener
  doesn't keep a list of the sources it is connected to. Otherwise the vtkCallbackCommand used
  as a bridge for the events will remain alive.*/
  virtual void PlugEventSource(mafAgent *source,mafID channel=MCH_ANY, float priority = 0.0);
  virtual void UnPlugEventSource(mafAgent *source);

#ifdef MAF_USE_VTK
  /**
    Plug in a VTK object as source of events, specifying the VTK event ID to be observed.*/
  virtual void PlugEventSource(vtkObject *source,mafID eventid, float priority = 0.0);

  /**
  This is a commodity function to attach a callback function to a VTK event. 'source' is the vtkObject
  to which attach the event, 'arg' is typically the pointer of the class that wants to link to the event (the 'self')
  and 'f' is the callback function to be attached to the event.*/
  static int PlugEventSource(vtkObject *source,void (*f)(void *), void *self, mafID eventid, float priority = 0.0);

  /**
  Unplug an events source. Remember to explicitly unplug all the source before destroying the
  listener.*/
  virtual void UnPlugEventSource(vtkObject *source);
#endif
  
  /**
  Attach/Detach this object as event source for the specified listener. If the specified channel is 
  the MCH_UP, the SetListener(listener) is called. For all other channel more then 
  one listener can be specified, and a broad cast using the VTK Subject/Observer is performed.
  UnPlugListener works for all channels at the same time.
  NULL listeners are ignored.*/
  void AddObserver(mafObserver *listener,mafID channel=MCH_UP, float priority = 0.0);
  void RemoveObserver(mafObserver *listener);
  void RemoveAllObservers();

  /**
  Process an event: the event is processed immediately and synchronously, i.e.
  the function doesn't return until the event is processed by someone. If the class
  cannot manage the event it is passed to its listeners on the same channel and so on,
  until it's processed.*/
  virtual void OnEvent(mafEventBase *event);

  /**  return true if there's a listener on the specified channel. (noarg == MCH_UP) */
  inline int HasObserver(mafID channel);

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
  void SetName() {};
  const char *GetName();

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
  This function make an event to be passed to listeners of this class for the specified channel. In case
  of MCH_UP the message is passed to the object pointed by the "Listener" member variable, otherwise 
  the event is broad casted by means of the VTK Subject/Observer mechanism on the specified channel,
  i.e. rising an event with ID equal to the channel.*/
  void ForwardEvent(mafEventBase *event, mafID channel=MCH_UP);
  void ForwardEvent(int id, mafID channel=MCH_UP,void *data=NULL);

  mafString       m_Name;
  bool            m_Initialized; // flag set true by Initialize()

  std::vector<mafEventSource *> m_Channels;

#ifdef MAF_USE_VTK
  vtkCallbackCommand *m_EventCallbackCommand; ///< this object is used as connection to event sources
#endif 

private:
  mafAgent(const mafAgent&);  // Not implemented.
  void operator=(const mafAgent&);  // Not implemented.

};

#endif 

