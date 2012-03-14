/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafAgentEventQueueTest.cpp,v $
Language:  C++
Date:      $Date: 2009-11-27 09:35:49 $
Version:   $Revision: 1.1.2.1 $
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

#include "mafAgentEventQueueTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "mafAgentEventQueue.h"


void mafAgentEventQueueTest::setUp()
{

}

void mafAgentEventQueueTest::tearDown()
{

}

void mafAgentEventQueueTest::TestFixture()
{

}

void mafAgentEventQueueTest::TestConstructorDestructor()
{
  mafAgentEventQueue *testAgentEventQueue = mafAgentEventQueue::New(); 
  mafDEL(testAgentEventQueue);
}

void mafAgentEventQueueTest::TestPushEvent()
{
  mafAgentEventQueue *testAgentEventQueue = mafAgentEventQueue::New(); 
  mafEvent dummyStaticEvent;
  mafID dummyID = 9999;
  dummyStaticEvent.SetId(dummyID);
  
  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  testAgentEventQueue->PushEvent(dummyStaticEvent);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);

  // if dispatch events is not called some leaks appears
  testAgentEventQueue->DispatchEvents();

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  mafEvent *dynamicDummyEvent = mafEvent::New();

  mafID anotherDummyID = 3333;
  dynamicDummyEvent->SetId(anotherDummyID);
  
  testAgentEventQueue->PushEvent(dynamicDummyEvent);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);
  
  testAgentEventQueue->DispatchEvents();
  
  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);
  
  mafID yetAnotherDummyID = 6666;  
  testAgentEventQueue->PushEvent(yetAnotherDummyID, this, NULL);
  
  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);
  
  testAgentEventQueue->DispatchEvents();

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  mafDEL(dynamicDummyEvent);
  mafDEL(testAgentEventQueue);  
}

void mafAgentEventQueueTest::TestPeekEvent()
{
  mafAgentEventQueue *testAgentEventQueue = mafAgentEventQueue::New(); 

  mafEvent dummyStaticEvent;

  mafEventBase *retrievedEvent = NULL;
  retrievedEvent = testAgentEventQueue->PeekEvent();
  
  CPPUNIT_ASSERT(retrievedEvent == NULL);

  mafID dummyID = 9999;
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

  mafDEL(testAgentEventQueue);  
}

void mafAgentEventQueueTest::TestPeekLastEvent()
{
  mafAgentEventQueue *testAgentEventQueue = mafAgentEventQueue::New(); 

  mafEvent dummyStaticEvent;

  mafEventBase *retrievedEvent = NULL;
  retrievedEvent = testAgentEventQueue->PeekLastEvent();

  CPPUNIT_ASSERT(retrievedEvent == NULL);

  mafID dummyID = 9999;
  dummyStaticEvent.SetId(dummyID);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  testAgentEventQueue->PushEvent(dummyStaticEvent);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);

  retrievedEvent = testAgentEventQueue->PeekLastEvent();

  CPPUNIT_ASSERT(retrievedEvent);

  CPPUNIT_ASSERT(retrievedEvent->GetId() == dummyID);
  
  mafEvent anotherDummyStaticEvent;

  mafID anotherdummyID = 6666;
  anotherDummyStaticEvent.SetId(anotherdummyID);
  
  testAgentEventQueue->PushEvent(anotherDummyStaticEvent);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 2);

  // I have to call DispatchEvents for every event that has been pushed
  // otherwise all events seems not to be dispatched even if, from the documentation, 
  // all events should be flushed by this method since I'm in DispatchEventMode.
  // This needs further investigation
  CPPUNIT_ASSERT(testAgentEventQueue->GetPushMode() == mafAgentEventQueue::DispatchEventMode);

  testAgentEventQueue->DispatchEvents();
  testAgentEventQueue->DispatchEvents();

  retrievedEvent = testAgentEventQueue->PeekLastEvent();

  CPPUNIT_ASSERT(retrievedEvent == NULL);

  mafDEL(testAgentEventQueue);  

}

void mafAgentEventQueueTest::TestGetQueueSize()
{
  mafAgentEventQueue *testAgentEventQueue = mafAgentEventQueue::New(); 

  mafEvent dummyStaticEvent;

  mafEventBase *retrievedEvent = NULL;
  retrievedEvent = testAgentEventQueue->PeekLastEvent();

  CPPUNIT_ASSERT(retrievedEvent == NULL);

  mafID dummyID = 9999;
  dummyStaticEvent.SetId(dummyID);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  testAgentEventQueue->PushEvent(dummyStaticEvent);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);

  retrievedEvent = testAgentEventQueue->PeekLastEvent();
  
  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);

  CPPUNIT_ASSERT(retrievedEvent);

  CPPUNIT_ASSERT(retrievedEvent->GetId() == dummyID);

  mafEvent anotherDummyStaticEvent;

  mafID anotherdummyID = 6666;
  anotherDummyStaticEvent.SetId(anotherdummyID);

  testAgentEventQueue->PushEvent(anotherDummyStaticEvent);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 2);

  // I have to call DispatchEvents for every event that has been pushed
  // otherwise all events seems not to be dispatched even if, from the documentation, 
  // all events should be flushed by this method since I'm in DispatchEventMode.
  // This needs further investigation
  CPPUNIT_ASSERT(testAgentEventQueue->GetPushMode() == mafAgentEventQueue::DispatchEventMode);

  testAgentEventQueue->DispatchEvents();

  int queueSize = testAgentEventQueue->GetQueueSize();

  CPPUNIT_ASSERT(queueSize == 1);

  testAgentEventQueue->DispatchEvents();

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  retrievedEvent = testAgentEventQueue->PeekLastEvent();

  CPPUNIT_ASSERT(retrievedEvent == NULL);

  mafDEL(testAgentEventQueue);  
  
}

void mafAgentEventQueueTest::TestIsQueueEmpty()
{
  mafAgentEventQueue *testAgentEventQueue = mafAgentEventQueue::New(); 

  CPPUNIT_ASSERT(testAgentEventQueue->IsQueueEmpty() == true);

  mafEvent dummyStaticEvent;
  mafID dummyID = 9999;
  dummyStaticEvent.SetId(dummyID);

  testAgentEventQueue->PushEvent(dummyStaticEvent);

  CPPUNIT_ASSERT(testAgentEventQueue->IsQueueEmpty() == false);

  // if dispatch events is not called some leaks appears
  testAgentEventQueue->DispatchEvents();

  CPPUNIT_ASSERT(testAgentEventQueue->IsQueueEmpty() == true);
 
  mafDEL(testAgentEventQueue);  

}

void mafAgentEventQueueTest::TestDispatchEvents()
{
  mafAgentEventQueue *testAgentEventQueue = mafAgentEventQueue::New(); 

  // trying to call DispatchEvents without pushed events
  testAgentEventQueue->DispatchEvents();
  
  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  mafEvent dummyStaticEvent;

  mafID dummyID = 9999;
  dummyStaticEvent.SetId(dummyID);

  testAgentEventQueue->PushEvent(dummyStaticEvent);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);

  // if dispatch events is not called some leaks appears
  testAgentEventQueue->DispatchEvents();

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  mafEvent *dynamicDummyEvent = mafEvent::New();

  mafID anotherDummyID = 3333;
  dynamicDummyEvent->SetId(anotherDummyID);

  testAgentEventQueue->PushEvent(dynamicDummyEvent);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);

  testAgentEventQueue->DispatchEvents();

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  mafID yetAnotherDummyID = 6666;

  testAgentEventQueue->PushEvent(yetAnotherDummyID, this, NULL);

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 1);

  testAgentEventQueue->DispatchEvents();

  CPPUNIT_ASSERT(testAgentEventQueue->GetQueueSize() == 0);

  mafDEL(dynamicDummyEvent);

  mafDEL(testAgentEventQueue);  
  
}
