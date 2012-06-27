/*=========================================================================

 Program: MAF2
 Module: mafAgentThreadedTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafAgentThreadedTest.h"
#include "mafDeviceButtonsPadTracker.h"

#include <cppunit/config/SourcePrefix.h>

#include "mafAgentThreaded.h"

void mafAgentThreadedTest::setUp()
{

}

void mafAgentThreadedTest::tearDown()
{

}

void mafAgentThreadedTest::TestFixture()
{

}

void mafAgentThreadedTest::TestConstructorDestructor()
{
  mafAgentThreaded *agentThreaded = mafAgentThreaded::New(); 
  mafDEL(agentThreaded);
}

void mafAgentThreadedTest::TestSetGetThreaded()
{
  mafAgentThreaded *agentThreaded = mafAgentThreaded::New(); 
  bool threaded = agentThreaded->GetThreaded();
  CPPUNIT_ASSERT(threaded == TRUE);

  agentThreaded->SetThreaded(FALSE);
  threaded = agentThreaded->GetThreaded();
  CPPUNIT_ASSERT(threaded == FALSE);

  mafDEL(agentThreaded);
}

void mafAgentThreadedTest::TestUpdate()
{
  mafAgentThreaded *agentThreaded = mafAgentThreaded::New(); 
  int returnValue = agentThreaded->Update();
  CPPUNIT_ASSERT(returnValue == -1); // -1 for not initialized 
  mafDEL(agentThreaded);
}

void mafAgentThreadedTest::TestAsyncSendEvent()
{
  DummyObserver *dummyObserver = new DummyObserver(); 
  mafAgentThreaded *agentThreaded = mafAgentThreaded::New(); 

  mafEventBase *dummyEvent = new mafEventBase();
  mafID dummyID = 9999;
  dummyEvent->SetId(dummyID); 
  
  agentThreaded->AsyncSendEvent(dummyObserver , dummyEvent);
  
  bool dispatched = false;
  dispatched = agentThreaded->DispatchEvents();
  CPPUNIT_ASSERT(dispatched == true);

  cppDEL(dummyEvent);
  cppDEL(dummyObserver);
  mafDEL(agentThreaded);
}

void mafAgentThreadedTest::TestAsyncInvokeEvent()
{
  mafAgentThreaded *agentThreaded = mafAgentThreaded::New(); 

  mafEventBase *dummyEvent = new mafEventBase();
  mafID dummyID = 9999;
  
  dummyEvent->SetId(dummyID); 

  agentThreaded->AsyncInvokeEvent(dummyEvent);

  bool dispatched = false;
  dispatched = agentThreaded->DispatchEvents();
  CPPUNIT_ASSERT(dispatched == true);

  cppDEL(dummyEvent);
  mafDEL(agentThreaded);
}

void	DummyObserver::OnEvent(mafEventBase *maf_event)
{
  mafLogMessage("\nEntering DummyObserver::OnEvent...\n");
}