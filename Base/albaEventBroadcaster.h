/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventBroadcaster
 Authors: Marco Petrone, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaEventBroadcaster_h
#define __albaEventBroadcaster_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaEventSender.h"
#include <vector>
#include <list>

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class albaObserver;
class albaObserversList;
class albaEventBase;
class albaObserverCallback;

//------------------------------------------------------------------------------
// albaEventBroadcaster
//------------------------------------------------------------------------------
/** This class inerith form albaEventSender and extends albaEventSender with the capacity 
	of send events to multiple objects. 
	Set/Get listener are overridded, when you call set listener you also remove all other 
	observers and get listener returns the first observer on the list.
  albaEventBroadcaster is a class implementing the "subject" in the Subject/Observer 
  design pattern. Objective of this object is to generate events to be sent to observers.
  An observer must be registered to a subject to create the communication channel 
  between the two.
  @sa albaObserver albaObserverCallback
*/
class ALBA_EXPORT albaEventBroadcaster : public albaEventSender
{
public:
  albaEventBroadcaster(void *owner=NULL);
  virtual ~albaEventBroadcaster();
 
  albaEventBroadcaster(const albaEventBroadcaster& c) {}

  /** Register an observer of this subject */
  void AddObserver(albaObserver *obj);

  /** Register an observer of this subject */
  void AddObserver(albaObserver &obj);

  /** 
    Add as observer a callback function. This function returns  
    pointer to an observer object that must be deleted by 
    consumer after having detached it from the event source */
  albaObserverCallback *AddObserverCallback(void (*f)(void *sender,  albaID eid, void *clientdata, void *calldata));

  /** Unregister an observer. Return false if object is not an observer */
  bool RemoveObserver(albaObserver *obj);

  /** remove all observers at once */
  void RemoveAllObservers();

  /** return true if object is an observer of this subject */
  bool IsObserver(albaObserver *obj);

  /** return true if this class has observers */
  bool HasObservers();

  /** return a vector with the list of observers of this event source */
  void GetObservers(std::vector<albaObserver *> &olist);

  /** invoke an event of this subject */
  void InvokeEvent(albaEventBase &e);

  /** invoke an event of this subject */
  void InvokeEvent(albaEventBase *e);

  /** invoke an event of this subject */
	void InvokeEvent(void *sender,albaID id = ID_NO_EVENT, void *data = NULL);
	 
  /** 
    set the channel Id assigned to this event source. If set to <0 
    no channel is assigned */
  void SetChannel(albaID ch);

  /** 
    return the channel assigned to this event source. If <0 no 
    channel has been assigned */
  albaID GetChannel();

	/** Removes all the observer and add o to the observer list */
	virtual void SetListener(albaObserver *o);

protected:
	typedef std::list<albaObserver *> albaObserversList;

  albaObserversList  m_Observers;  ///< list of observers
  albaID             m_Channel;     ///< a channel assigned to this event source, if <0 no channel is assigned
private:
  
};

#endif /* __albaEventBroadcaster_h */
