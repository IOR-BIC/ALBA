/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGEventSource.h,v $
  Language:  C++
  Date:      $Date: 2004-11-04 20:59:27 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGEventSource_h
#define __mafGEventSource_h

#include "mafObject.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class mafGObserver;
class mafGObserversList;
class mafEventBase;
class mafGObserverCallback;

//------------------------------------------------------------------------------
// mafGEventSource
//------------------------------------------------------------------------------
/** Implementation of the Subject/Observer design pattern.
  mafGEventSource is a class implementing the "subject" in the Subject/Observer 
  design pattern. Objective of this object is to generate events to be sent to observers.
  An observer must be registered to a subject to create the communication channel 
  between the two.
  @sa mafGObserver mafGObserverCallback
*/
class mafGEventSource: public mafObject
{
public:
  mafGEventSource(void *owner=NULL);
  virtual ~mafGEventSource();

  mafTypeMacro(mafGEventSource,mafObject);
 
  mafGEventSource(const mafGEventSource& c) {}

  /** Register an observer of this subject */
  void AddObserver(mafGObserver *obj, int priority=0);

  /** 
    Add as observer a callback function. This function returns  
    pointer to an observer object that must be deleted by 
    consumer after having detached it from the event source */
  //mafGObserverCallback *AddObserverCallback(void (*f)(void *sender,
  //  mafID eid, void *clientdata, void *calldata));

  /** Unregister an observer. Return false if object is not an observer */
  bool RemoveObserver(mafGObserver *obj);

  /** return true if object is an observer of this subject */
  bool IsObserver(mafGObserver *obj);

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
  mafGObserversList *Observers;  ///< list of observers
  void *Owner;                  ///< pointer to class owning this event source
private:
  
};

#endif /* __mafGEventSource_h */
