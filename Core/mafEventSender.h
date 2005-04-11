/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEventSender.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:23:15 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafEventSender_h
#define __mafEventSender_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafObserver.h"
#include "mafEventBase.h"
//------------------------------------------------------------------------------
// mafEventSender
//------------------------------------------------------------------------------
/** class acting as an interface for objects using MAF hierarchical event commuication model
  This object simply defines a SetListener/GetListener functions for connecting with a listerner
  object to create a hierarcy of objects. This allows to send events up in the hierarchy
  implementing what was the original MAF event communication model: this is a unicast communication
  model very well suited for herarchically organized objects. For sending events up in the tree 
  the mafEventMacro() or the InvokeEvent() function can be use.
  @sa mafObserver mafObserverCallback
*/
class MAF_EXPORT mafEventSender
{
public:
  mafEventSender() {m_Listener = NULL;}

  /** Set the listener object, i.e. the object receiving events sent by this object */
  void SetListener(mafObserver *o) {m_Listener = o;}

  /** Return the listener object, i.e. the object receiving events sent by this object */
  mafObserver *GetListener() {return m_Listener;}

  /** return true if this class has observers */
  bool HasListener() {return m_Listener!=NULL;}

  /** invoke an event of this subject */
  void InvokeEvent(mafEventBase &e) {if (m_Listener) m_Listener->OnEvent(&e);}

  /** invoke an event of this subject */
  void InvokeEvent(mafEventBase *e) {if (m_Listener) m_Listener->OnEvent(e);}

  /** invoke an event of this subject */
  void InvokeEvent(mafID id=ID_NO_EVENT, void *data=NULL) {mafEventBase e(this,id,data);InvokeEvent(e);}

protected:
  mafObserver *m_Listener;  ///< object to which events issued by this object are sent
private:
  
};

#endif /* __mafEventSender_h */
