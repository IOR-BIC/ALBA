/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAgentThreadedTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h"
#include "albaInteractionTests.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaAgentThreadedTest.h"
#include "albaDeviceButtonsPadTracker.h"

#include <cppunit/config/SourcePrefix.h>

#include "albaAgentThreaded.h"


void albaAgentThreadedTest::TestFixture()
{

}

void albaAgentThreadedTest::TestConstructorDestructor()
{
  albaAgentThreaded *agentThreaded = albaAgentThreaded::New(); 
  albaDEL(agentThreaded);
}

void albaAgentThreadedTest::TestSetGetThreaded()
{
  albaAgentThreaded *agentThreaded = albaAgentThreaded::New(); 
  bool threaded = agentThreaded->GetThreaded();
  CPPUNIT_ASSERT(threaded == true);

  agentThreaded->SetThreaded(false);
  threaded = agentThreaded->GetThreaded();
  CPPUNIT_ASSERT(threaded == false);

  albaDEL(agentThreaded);
}

void albaAgentThreadedTest::TestUpdate()
{
  albaAgentThreaded *agentThreaded = albaAgentThreaded::New(); 
  int returnValue = agentThreaded->Update();
  CPPUNIT_ASSERT(returnValue == -1); // -1 for not initialized 
  albaDEL(agentThreaded);
}

void albaAgentThreadedTest::TestAsyncSendEvent()
{
  DummyObserver *dummyObserver = new DummyObserver(); 
  albaAgentThreaded *agentThreaded = albaAgentThreaded::New(); 

  albaEventBase *dummyEvent = new albaEventBase();
  albaID dummyID = 9999;
  dummyEvent->SetId(dummyID); 
  
  agentThreaded->AsyncSendEvent(dummyObserver , dummyEvent);
  
  bool dispatched = false;
  dispatched = agentThreaded->DispatchEvents();
  CPPUNIT_ASSERT(dispatched == true);

  cppDEL(dummyEvent);
  cppDEL(dummyObserver);
  albaDEL(agentThreaded);
}

void albaAgentThreadedTest::TestAsyncInvokeEvent()
{
  albaAgentThreaded *agentThreaded = albaAgentThreaded::New(); 

  albaEventBase *dummyEvent = new albaEventBase();
  albaID dummyID = 9999;
  
  dummyEvent->SetId(dummyID); 

  agentThreaded->AsyncInvokeEvent(dummyEvent);

  bool dispatched = false;
  dispatched = agentThreaded->DispatchEvents();
  CPPUNIT_ASSERT(dispatched == true);

  cppDEL(dummyEvent);
  albaDEL(agentThreaded);
}
