/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAgentEventQueueTest
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

#include "albaAgentEventQueueTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "albaAgentEventQueue.h"


void albaAgentEventQueueTest::TestFixture()
{

}

void albaAgentEventQueueTest::TestConstructorDestructor()
{
  albaAgentEventQueue *testAgentEventQueue = albaAgentEventQueue::New(); 
  albaDEL(testAgentEventQueue);
}

void albaAgentEventQueueTest::TestPushEvent()
{
  albaAgentEventQueue *testAgentEventQueue = albaAgentEventQueue::New(); 
  albaEvent dummyStaticEvent;
  albaID dummyID = 9999;
  dummyStaticEvent.SetId(dummyID);
  
  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  testAgentEventQueue->PushEvent(dummyStaticEvent);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);

  // if dispatch events is not called some leaks appears
  testAgentEventQueue->DispatchEvents();

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  albaEvent *dynamicDummyEvent = albaEvent::New();

  albaID anotherDummyID = 3333;
  dynamicDummyEvent->SetId(anotherDummyID);
  
  testAgentEventQueue->PushEvent(dynamicDummyEvent);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);
  
  testAgentEventQueue->DispatchEvents();
  
  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);
  
  albaID yetAnotherDummyID = 6666;  
  testAgentEventQueue->PushEvent(yetAnotherDummyID, this, NULL);
  
  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);
  
  testAgentEventQueue->DispatchEvents();

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  albaDEL(dynamicDummyEvent);
  albaDEL(testAgentEventQueue);  
}

void albaAgentEventQueueTest::TestPeekEvent()
{
  albaAgentEventQueue *testAgentEventQueue = albaAgentEventQueue::New(); 

  albaEvent dummyStaticEvent;

  albaEventBase *retrievedEvent = NULL;
  retrievedEvent = testAgentEventQueue->PeekEvent();
  
  CPPUNIT_ASSERT(retrievedEvent == NULL);

  albaID dummyID = 9999;
  dummyStaticEvent.SetId(dummyID);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  testAgentEventQueue->PushEvent(dummyStaticEvent);
  
  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);

  retrievedEvent = testAgentEventQueue->PeekEvent();
  
  CPPUNIT_ASSERT(retrievedEvent);
  
  CPPUNIT_ASSERT(retrievedEvent->GetId() == dummyID);

  // if dispatch events is not called some leaks appears
  testAgentEventQueue->DispatchEvents();
  
  retrievedEvent = testAgentEventQueue->PeekEvent();

  CPPUNIT_ASSERT(retrievedEvent == NULL);

  albaDEL(testAgentEventQueue);  
}

void albaAgentEventQueueTest::TestPeekLastEvent()
{
  albaAgentEventQueue *testAgentEventQueue = albaAgentEventQueue::New(); 

  albaEvent dummyStaticEvent;

  albaEventBase *retrievedEvent = NULL;
  retrievedEvent = testAgentEventQueue->PeekLastEvent();

  CPPUNIT_ASSERT(retrievedEvent == NULL);

  albaID dummyID = 9999;
  dummyStaticEvent.SetId(dummyID);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  testAgentEventQueue->PushEvent(dummyStaticEvent);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);

  retrievedEvent = testAgentEventQueue->PeekLastEvent();

  CPPUNIT_ASSERT(retrievedEvent);

  CPPUNIT_ASSERT(retrievedEvent->GetId() == dummyID);
  
  albaEvent anotherDummyStaticEvent;

  albaID anotherdummyID = 6666;
  anotherDummyStaticEvent.SetId(anotherdummyID);
  
  testAgentEventQueue->PushEvent(anotherDummyStaticEvent);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 2);

  // I have to call DispatchEvents for every event that has been pushed
  // otherwise all events seems not to be dispatched even if, from the documentation, 
  // all events should be flushed by this method since I'm in DispatchEventMode.
  // This needs further investigation
  CPPUNIT_ASSERT(testAgentEventQueue->GetPushMode() == albaAgentEventQueue::DispatchEventMode);

  testAgentEventQueue->DispatchEvents();
  testAgentEventQueue->DispatchEvents();

  retrievedEvent = testAgentEventQueue->PeekLastEvent();

  CPPUNIT_ASSERT(retrievedEvent == NULL);

  albaDEL(testAgentEventQueue);  

}

void albaAgentEventQueueTest::TestGetQueueSize()
{
  albaAgentEventQueue *testAgentEventQueue = albaAgentEventQueue::New(); 

  albaEvent dummyStaticEvent;

  albaEventBase *retrievedEvent = NULL;
  retrievedEvent = testAgentEventQueue->PeekLastEvent();

  CPPUNIT_ASSERT(retrievedEvent == NULL);

  albaID dummyID = 9999;
  dummyStaticEvent.SetId(dummyID);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  testAgentEventQueue->PushEvent(dummyStaticEvent);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);

  retrievedEvent = testAgentEventQueue->PeekLastEvent();
  
  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);

  CPPUNIT_ASSERT(retrievedEvent);

  CPPUNIT_ASSERT(retrievedEvent->GetId() == dummyID);

  albaEvent anotherDummyStaticEvent;

  albaID anotherdummyID = 6666;
  anotherDummyStaticEvent.SetId(anotherdummyID);

  testAgentEventQueue->PushEvent(anotherDummyStaticEvent);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 2);

  // I have to call DispatchEvents for every event that has been pushed
  // otherwise all events seems not to be dispatched even if, from the documentation, 
  // all events should be flushed by this method since I'm in DispatchEventMode.
  // This needs further investigation
  CPPUNIT_ASSERT(testAgentEventQueue->GetPushMode() == albaAgentEventQueue::DispatchEventMode);

  testAgentEventQueue->DispatchEvents();

  int queueSize = testAgentEventQueue->GetQueueSize();

  CPPUNIT_ASSERT(queueSize == 1);

  testAgentEventQueue->DispatchEvents();

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  retrievedEvent = testAgentEventQueue->PeekLastEvent();

  CPPUNIT_ASSERT(retrievedEvent == NULL);

  albaDEL(testAgentEventQueue);  
  
}

void albaAgentEventQueueTest::TestIsQueueEmpty()
{
  albaAgentEventQueue *testAgentEventQueue = albaAgentEventQueue::New(); 

  CPPUNIT_ASSERT(testAgentEventQueue->IsQueueEmpty() == true);

  albaEvent dummyStaticEvent;
  albaID dummyID = 9999;
  dummyStaticEvent.SetId(dummyID);

  testAgentEventQueue->PushEvent(dummyStaticEvent);

  CPPUNIT_ASSERT(testAgentEventQueue->IsQueueEmpty() == false);

  // if dispatch events is not called some leaks appears
  testAgentEventQueue->DispatchEvents();

  CPPUNIT_ASSERT(testAgentEventQueue->IsQueueEmpty() == true);
 
  albaDEL(testAgentEventQueue);  

}

void albaAgentEventQueueTest::TestDispatchEvents()
{
  albaAgentEventQueue *testAgentEventQueue = albaAgentEventQueue::New(); 

  // trying to call DispatchEvents without pushed events
  testAgentEventQueue->DispatchEvents();
  
  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  albaEvent dummyStaticEvent;

  albaID dummyID = 9999;
  dummyStaticEvent.SetId(dummyID);

  testAgentEventQueue->PushEvent(dummyStaticEvent);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);

  // if dispatch events is not called some leaks appears
  testAgentEventQueue->DispatchEvents();

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  albaEvent *dynamicDummyEvent = albaEvent::New();

  albaID anotherDummyID = 3333;
  dynamicDummyEvent->SetId(anotherDummyID);

  testAgentEventQueue->PushEvent(dynamicDummyEvent);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);

  testAgentEventQueue->DispatchEvents();

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  albaID yetAnotherDummyID = 6666;

  testAgentEventQueue->PushEvent(yetAnotherDummyID, this, NULL);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);

  testAgentEventQueue->DispatchEvents();

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  albaDEL(dynamicDummyEvent);

  albaDEL(testAgentEventQueue);  
  
}
