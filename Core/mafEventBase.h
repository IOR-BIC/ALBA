/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEventBase.h,v $
  Language:  C++
  Date:      $Date: 2004-11-08 17:32:34 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafEventBase_h
#define __mafEventBase_h

#include "mafObject.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class mafEventSource;

//------------------------------------------------------------------------------
// mafEventBase
//------------------------------------------------------------------------------
/** Implementation of the Subject/Observer design pattern.
  mafEventBase is a class implementing the "observer" in the Subject/Observer 
  design pattern. Objective of this object is to listen to events rised by subjects.
  An observer must be registered into a subject to create the communication channel 
  between the two.

  @sa mafSubject mafObserver mafEvent
*/
class mafEventBase: public mafObject
{
public:
  mafEventBase(void *sender=NULL,mafID id=ID_NO_EVENT,void *data=NULL);
  virtual ~mafEventBase();

  mafTypeMacro(mafEventBase,mafObject);
 
  /** copy constructor, this makes a copy of the event */
  mafEventBase(const mafEventBase& c);

  /** set the sender (invoker) of this event */
  void SetSender(void *sender);

  /** return sender (invoker) of this event */
  void *GetSender();

  /** 
    set event source who generated this event. Used by
    mafEventSource to store its pointer in the event */
  void SetSource(mafEventSource *src);

  /** return the source to the event source who generated this event */
  mafEventSource *GetSource();

  /** set Id for this event */
  void SetId(mafID id);

  /** return Id of this event */
  mafID GetId();

  /** set call data, data sent by sender (event's invoker) to all observers */
  void SetData(void *calldata);

  /** return call data, data sent by sender (event's invoker) to all observers */
  void *GetData();

protected:
  void            *Sender;
  mafEventSource  *Source;
  void            *Data;
  mafID           Id;
};

#endif /* __mafEventBase_h */
