/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaInteractionTests_H__
#define __CPP_UNIT_albaInteractionTests_H__


//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaObserver.h"

/** testing facility to receive events */
class mockListener : public albaObserver 
{
public:

  /** Set the event receiver object*/
  void  SetListener(albaObserver *Listener) {m_Listener = Listener;};

  /** Events handling*/        
  virtual void OnEvent(albaEventBase *alba_event);

  albaEventBase *GetEvent();;

private:
  /**
  Register the event receiver object*/
  albaObserver *m_Listener;

  albaEventBase m_Event;
};

class DummyObserver : public albaObserver
{
public:

	DummyObserver() { m_LastReceivedEventID = -1;};
	~DummyObserver() {};

	virtual void OnEvent(albaEventBase *alba_event);

	int GetLastReceivedEventID();

private:
	int m_LastReceivedEventID;

};

#endif
