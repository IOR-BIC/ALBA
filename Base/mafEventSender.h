/*=========================================================================

 Program: MAF2
 Module: mafEventSender
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
/** class acting as an interface for objects using MAF hierarchical event communication model
  This object simply defines a SetListener/GetListener functions for connecting with a listener
  object to create a hierarchy of objects. This allows to send events up in the hierarchy
  implementing what was the original MAF event communication model: this is a unicast communication
  model very well suited for hierarchically organized objects. For sending events up in the tree 
  the mafEventMacro() or the InvokeEvent() function can be use.
  @sa mafObserver mafObserverCallback
*/
class MAF_EXPORT mafEventSender
{
public:
  mafEventSender() {m_Listener = NULL;}

  /** Set the listener object, i.e. the object receiving events sent by this object */
  virtual void SetListener(mafObserver *o) {m_Listener = o;}

  /** Return the listener object, i.e. the object receiving events sent by this object */
	virtual mafObserver *GetListener() {return m_Listener;}

  /** return true if this class has observers */
	virtual bool HasListener() {return m_Listener!=NULL;}

  /** invoke an event of this subject */
	virtual void InvokeEvent(mafEventBase &e) {if (m_Listener) m_Listener->OnEvent(&e);}

  /** invoke an event of this subject */
	virtual void InvokeEvent(mafEventBase *e) {if (m_Listener) m_Listener->OnEvent(e);}

  /** invoke an event of this subject */
	virtual void InvokeEvent(void *sender, mafID id=ID_NO_EVENT, void *data=NULL) {if (m_Listener) m_Listener->OnEvent(&mafEventBase(sender,id,data));}

protected:
  mafObserver *m_Listener;  ///< object to which events issued by this object are sent
private:
  
};

#endif /* __mafEventSender_h */
