/*=========================================================================

 Program: MAF2
 Module: mafEventBroadcaster
 Authors: Marco Petrone, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafEventBroadcaster_h
#define __mafEventBroadcaster_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafEventSender.h"
#include <vector>
#include <list>

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class mafObserver;
class mafObserversList;
class mafEventBase;
class mafObserverCallback;

//------------------------------------------------------------------------------
// mafEventBroadcaster
//------------------------------------------------------------------------------
/** This class inerith form mafEventSender and extends mafEventSender with the capacity 
	of send events to multiple objects. 
	Set/Get listener are overridded, when you call set listener you also remove all other 
	observers and get listener returns the first observer on the list.
  mafEventBroadcaster is a class implementing the "subject" in the Subject/Observer 
  design pattern. Objective of this object is to generate events to be sent to observers.
  An observer must be registered to a subject to create the communication channel 
  between the two.
  @sa mafObserver mafObserverCallback
*/
class MAF_EXPORT mafEventBroadcaster : public mafEventSender
{
public:
  mafEventBroadcaster(void *owner=NULL);
  virtual ~mafEventBroadcaster();
 
  mafEventBroadcaster(const mafEventBroadcaster& c) {}

  /** Register an observer of this subject */
  void AddObserver(mafObserver *obj);

  /** Register an observer of this subject */
  void AddObserver(mafObserver &obj);

  /** 
    Add as observer a callback function. This function returns  
    pointer to an observer object that must be deleted by 
    consumer after having detached it from the event source */
  mafObserverCallback *AddObserverCallback(void (*f)(void *sender,  mafID eid, void *clientdata, void *calldata));

  /** Unregister an observer. Return false if object is not an observer */
  bool RemoveObserver(mafObserver *obj);

  /** remove all observers at once */
  void RemoveAllObservers();

  /** return true if object is an observer of this subject */
  bool IsObserver(mafObserver *obj);

  /** return true if this class has observers */
  bool HasObservers();

  /** return a vector with the list of observers of this event source */
  void GetObservers(std::vector<mafObserver *> &olist);

  /** invoke an event of this subject */
  void InvokeEvent(mafEventBase &e);

  /** invoke an event of this subject */
  void InvokeEvent(mafEventBase *e);

  /** invoke an event of this subject */
	void InvokeEvent(void *sender,mafID id = ID_NO_EVENT, void *data = NULL);
	 
  /** 
    set the channel Id assigned to this event source. If set to <0 
    no channel is assigned */
  void SetChannel(mafID ch);

  /** 
    return the channel assigned to this event source. If <0 no 
    channel has been assigned */
  mafID GetChannel();

	/** Removes all the observer and add o to the observer list */
	virtual void SetListener(mafObserver *o);

protected:
	typedef std::list<mafObserver *> mafObserversList;

  mafObserversList  m_Observers;  ///< list of observers
  mafID             m_Channel;     ///< a channel assigned to this event source, if <0 no channel is assigned
private:
  
};

#endif /* __mafEventBroadcaster_h */
