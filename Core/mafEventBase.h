/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEventBase.h,v $
  Language:  C++
  Date:      $Date: 2005-02-20 23:24:25 $
  Version:   $Revision: 1.7 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafEventBase_h
#define __mafEventBase_h

#include "mafObject.h"
#include "mafDecl.h"

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
  Fields of the basic events are:
  - Sender: the object who sent this event
  - Source: the event source object who issue the event (NULL for non event source sent events)
  - Id: a numerical an ID identifying the type of content of this message
  - Channel: a secondary ID identifying the scope of this message.
  - Data: a void * for data to be attached to this event. (No reference counting, smart or autopointer mechanism!!!)
  - SkipFlag: is used to make an event to be skipped by broadcasting mechanism for next observers

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

  /** return channel this event is travelling through */
  mafID GetChannel();
  
  /** set channel this event is travelling through */
  void SetChannel(mafID channel);
;
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
  mafEventSource  *m_Source;
  void            *m_Data;
  mafID           m_Id;
  mafID           m_Channel;
  bool            m_SkipFlag;
};

#endif /* __mafEventBase_h */
