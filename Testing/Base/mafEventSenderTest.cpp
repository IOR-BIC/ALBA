/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafEventSenderTest.cpp,v $
Language:  C++
Date:      $Date: 2007-06-04 16:46:09 $
Version:   $Revision: 1.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>

#include "mafEventSenderTest.h"
#include "mafEventSender.h"
#include "mafString.h"
#include <iostream>
#include "mafEventBase.h"


void mafEventSenderTest::TestSetListener()
{
  m_Sender->SetListener(m_DummyObserver);
  CPPUNIT_ASSERT(m_Sender->GetListener() == m_DummyObserver);
}

void mafEventSenderTest::TestGetListener()
{
	CPPUNIT_ASSERT(m_Sender->GetListener() == NULL);
}

void mafEventSenderTest::TestHasListener()
{
  CPPUNIT_ASSERT(m_Sender->HasListener() == false)	;
  m_Sender->SetListener(m_DummyObserver)  ;
  CPPUNIT_ASSERT(m_Sender->HasListener() == true);
}

void mafEventSenderTest::TestInvokeEvent()
{
  mafEventBase *stupidEvent = new mafEventBase()	;

  m_Sender->SetListener(m_DummyObserver);

  m_Sender->InvokeEvent(stupidEvent);
  m_Sender->InvokeEvent(*stupidEvent);
  m_Sender->InvokeEvent(ID_NO_EVENT, NULL);

  mafDEL(stupidEvent);

}

void mafEventSenderTest::setUp()
{
  m_Sender = new mafEventSender();

  m_DummyObserver = new DummyObserver();
}

void mafEventSenderTest::tearDown()
{
  cppDEL(m_Sender);
	cppDEL(m_DummyObserver);
}

void mafEventSenderTest::TestFixture()
{
	
}

void	DummyObserver::OnEvent(mafEventBase *maf_event)
{
  mafLogMessage("\nEntering DummyObserver::OnEvent...\n");
}
