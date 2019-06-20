/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventBase
 Authors: Marco Petrone, Crimi Gianluigi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaEventBase_h
#define __albaEventBase_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaObject.h"
#include "albaDecl.h"
//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// albaEventBase
//------------------------------------------------------------------------------
/** Implementation of the message object for the Subject/Observer design pattern.
  albaEventBase is a class implementing the "message" in the Subject/Observer 
  design pattern. Objective of this object is to contain the message sent by subjects
  and received by observers when an event is invoked.
  Fields of the basic events are:
  - Sender: the object who sent this event
  - Source: the event source object who issue the event (NULL for non event source sent events)
  - Id: a numerical an ID identifying the type of content of this message
  - Channel: a secondary ID identifying the scope of this message.
  - Data: a void * for data to be attached to this event. (No reference counting, smart or autopointer mechanism!!!)
  - SkipFlag: is used to make an event to be skipped by broadcasting mechanism for next observers

  More complex messages can be defined inheriting from this class. The RTTI mechanism ensure
  to be able recognized and cast the real kind of event.

  @sa albaSubject albaObserver albaEvent
*/
class ALBA_EXPORT albaEventBase: public albaObject
{
public:
  albaEventBase(void *sender=NULL,albaID id=ID_NO_EVENT,void *data=NULL, albaID channel=MCH_UP);
  virtual ~albaEventBase();

  albaTypeMacro(albaEventBase,albaObject);
 
  /** copy constructor, this makes a copy of the event */
  albaEventBase(const albaEventBase& c);

  virtual void DeepCopy(const albaEventBase *alba_event);

  /** set the sender (invoker) of this event */
  void SetSender(void *sender);

  /** return sender (invoker) of this event */
  void *GetSender();

  /** set Id for this event */
  void SetId(albaID id);

  /** return Id of this event */
  albaID GetId();

  /** return channel this event is travelling through */
  albaID GetChannel();
  
  /** set channel this event is travelling through */
  void SetChannel(albaID channel);

  /** set call data, data sent by sender (event's invoker) to all observers */
  void SetData(void *calldata);

  /** return call data, data sent by sender (event's invoker) to all observers */
  void *GetData();

protected:
  void            *m_Sender;
  void            *m_Data;
  albaID           m_Id;
  albaID           m_Channel;
};
#endif /* __albaEventBase_h */
