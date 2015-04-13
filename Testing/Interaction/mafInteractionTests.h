/*=========================================================================

 Program: MAF2
 Module: mafWizardTest
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafInteractionTests_H__
#define __CPP_UNIT_mafInteractionTests_H__


//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafObserver.h"

/** testing facility to receive events */
class mockListener : public mafObserver 
{
public:

  /** Set the event receiver object*/
  void  SetListener(mafObserver *Listener) {m_Listener = Listener;};

  /** Events handling*/        
  virtual void OnEvent(mafEventBase *maf_event);

  mafEventBase *GetEvent();;

private:
  /**
  Register the event receiver object*/
  mafObserver *m_Listener;

  mafEventBase m_Event;
};

class DummyObserver : public mafObserver
{
public:

	DummyObserver() { m_LastReceivedEventID = -1;};
	~DummyObserver() {};

	virtual void OnEvent(mafEventBase *maf_event);

	int GetLastReceivedEventID();

private:
	int m_LastReceivedEventID;

};

#endif
