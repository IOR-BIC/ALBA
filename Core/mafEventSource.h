/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEventSource.h,v $
  Language:  C++
  Date:      $Date: 2004-11-04 12:18:53 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafEventSource_h
#define __mafEventSource_h

#include "mafObject.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class mafObserver;
class mafObserversList;
class mafEventBase;
class mafObserverCallback;

//------------------------------------------------------------------------------
// mafEventSource
//------------------------------------------------------------------------------
/** Implementation of the Subject/Observer design pattern.
  mafEventSource is a class implementing the "subject" in the Subject/Observer 
  design pattern. Objective of this object is to generate events to be sent to observers.
  An observer must be registered to a subject to create the communication channel 
  between the two.
  @sa mafObserver mafObserverCallback
*/
class mafEventSource: public mafObject
{
public:
  mafEventSource(void *owner=NULL);
  virtual ~mafEventSource();

  mafTypeMacro(mafEventSource,mafObject);
 
  mafEventSource(const mafEventSource& c) {}
  //void operator=(const mafEventSource&) {}

  /** Register an observer of this subject */
  void AddObserver(mafObserver *obj, int priority=0);

  /** 
    Add as observer a callback function. This function returns  
    pointer to an observer object that must be deleted by 
    consumer after having detached it from the event source */
  mafObserverCallback *AddObserverCallback(void (*f)(void *sender,
    mafID eid, void *clientdata, void *calldata));

  /** Unregister an observer. Return false if object is not an observer */
  bool RemoveObserver(mafObserver *obj);

  /** return true if object is an observer of this subject */
  bool IsObserver(mafObserver *obj);

  /** return true if this class has observers */
  bool HasObservers();

  /** invoke an event of this subject */
  void InvokeEvent(mafEventBase &e) {InvokeEvent(&e);}

  /** invoke an event of this subject */
  void InvokeEvent(mafEventBase *e);

  /** invoke an event of this subject */
  void InvokeEvent(void *sender,void *data=NULL);

  /** return pointer to client data stored in this object */
  void *GetData();

  /** set pointer to client data to be stored in this object */
  void SetData(void *data);

  /** 
    set the owner of this object, used to create
    a delegation pattern between the event source 
    and the class who created it */
  void SetOwner(void *owner);

  /** return the owner class of this event source */
  void *GetOwner();

protected:
  void *Data;             ///< void pointer to be used to store client data 
  mafObserversList *Observers;  ///< list of observers
  void *Owner;                  ///< pointer to class owning this event source
private:
  
};

#endif /* __mafEventSource_h */
