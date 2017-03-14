/*=========================================================================

 Program: MAF2
 Module: mafEventBase
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafEventBase_h
#define __mafEventBase_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafObject.h"
#include "mafDecl.h"
//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// mafEventBase
//------------------------------------------------------------------------------
/** Implementation of the message object for the Subject/Observer design pattern.
  mafEventBase is a class implementing the "message" in the Subject/Observer 
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

  @sa mafSubject mafObserver mafEvent
*/
class MAF_EXPORT mafEventBase: public mafObject
{
public:
  mafEventBase(void *sender=NULL,mafID id=ID_NO_EVENT,void *data=NULL, mafID channel=MCH_UP);
  virtual ~mafEventBase();

  mafTypeMacro(mafEventBase,mafObject);
 
  /** copy constructor, this makes a copy of the event */
  mafEventBase(const mafEventBase& c);

  virtual void DeepCopy(const mafEventBase *maf_event);

  /** Static function to turn On/Off the verbose logging.*/
  static void SetLogVerbose(bool verbose = true);

  /** Static function to get the verbose logging.*/
  static bool* GetLogVerbose();

  /** set the sender (invoker) of this event */
  void SetSender(void *sender);

  /** return sender (invoker) of this event */
  void *GetSender();

  /** set Id for this event */
  void SetId(mafID id);

  /** return Id of this event */
  mafID GetId();

  /** return channel this event is travelling through */
  mafID GetChannel();
  
  /** set channel this event is travelling through */
  void SetChannel(mafID channel);

  /** set call data, data sent by sender (event's invoker) to all observers */
  void SetData(void *calldata);

  /** return call data, data sent by sender (event's invoker) to all observers */
  void *GetData();

  /** return SkipFlag value. SkipFlag is used to make an event to be skipped by next observers */
  bool GetSkipFlag();

  /** set the skip flag. When true the event will be skipped by next observers */
  void SetSkipFlag(bool flag);

  /** force an event to be skipped by next observers */
  void SkipNext();

protected:
  void            *m_Sender;
  void            *m_Data;
  mafID           m_Id;
  mafID           m_Channel;
  bool            m_SkipFlag;
  // static bool     m_LogVerbose;
};
#endif /* __mafEventBase_h */
