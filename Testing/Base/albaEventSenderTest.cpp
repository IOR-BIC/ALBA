/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventSenderTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>

#include "albaEventSenderTest.h"
#include "albaEventSender.h"
#include "albaString.h"
#include <iostream>
#include "albaEventBase.h"


void albaEventSenderTest::TestSetListener()
{
  m_Sender->SetListener(m_DummyObserver);
  CPPUNIT_ASSERT(m_Sender->GetListener() == m_DummyObserver);
}

void albaEventSenderTest::TestGetListener()
{
	CPPUNIT_ASSERT(m_Sender->GetListener() == NULL);
}

void albaEventSenderTest::TestHasListener()
{
  CPPUNIT_ASSERT(m_Sender->HasListener() == false)	;
  m_Sender->SetListener(m_DummyObserver)  ;
  CPPUNIT_ASSERT(m_Sender->HasListener() == true);
}

void albaEventSenderTest::TestInvokeEvent()
{
  albaEventBase *stupidEvent = new albaEventBase()	;

  m_Sender->SetListener(m_DummyObserver);

  m_Sender->InvokeEvent(stupidEvent);
  m_Sender->InvokeEvent(*stupidEvent);
  m_Sender->InvokeEvent(ID_NO_EVENT, NULL);

  albaDEL(stupidEvent);

}

void albaEventSenderTest::BeforeTest()
{
  m_Sender = new albaEventSender();
  m_DummyObserver = new DummyObserver();
}

void albaEventSenderTest::AfterTest()
{
  cppDEL(m_Sender);
	cppDEL(m_DummyObserver);
}

void albaEventSenderTest::TestFixture()
{
	
}

void	DummyObserver::OnEvent(albaEventBase *alba_event)
{
  albaLogMessage("\nEntering DummyObserver::OnEvent...\n");
}
