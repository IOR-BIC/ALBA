/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventSender
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaEventSender_h
#define __albaEventSender_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaObserver.h"
#include "albaEventBase.h"
//------------------------------------------------------------------------------
// albaEventSender
//------------------------------------------------------------------------------
/** class acting as an interface for objects using ALBA hierarchical event communication model
  This object simply defines a SetListener/GetListener functions for connecting with a listener
  object to create a hierarchy of objects. This allows to send events up in the hierarchy
  implementing what was the original ALBA event communication model: this is a unicast communication
  model very well suited for hierarchically organized objects. For sending events up in the tree 
  the albaEventMacro() or the InvokeEvent() function can be use.
  @sa albaObserver albaObserverCallback
*/
class ALBA_EXPORT albaEventSender
{
public:
  albaEventSender() {m_Listener = NULL;}

  /** Set the listener object, i.e. the object receiving events sent by this object */
  virtual void SetListener(albaObserver *o) {m_Listener = o;}

  /** Return the listener object, i.e. the object receiving events sent by this object */
	virtual albaObserver *GetListener() {return m_Listener;}

  /** return true if this class has observers */
	virtual bool HasListener() {return m_Listener!=NULL;}

  /** invoke an event of this subject */
	virtual void InvokeEvent(albaEventBase &e) {if (m_Listener) m_Listener->OnEvent(&e);}

  /** invoke an event of this subject */
	virtual void InvokeEvent(albaEventBase *e) {if (m_Listener) m_Listener->OnEvent(e);}

  /** invoke an event of this subject */
	virtual void InvokeEvent(void *sender, albaID id=ID_NO_EVENT, void *data=NULL) {if (m_Listener) m_Listener->OnEvent(&albaEventBase(sender,id,data));}

protected:
  albaObserver *m_Listener;  ///< object to which events issued by this object are sent
private:
  
};

#endif /* __albaEventSender_h */
