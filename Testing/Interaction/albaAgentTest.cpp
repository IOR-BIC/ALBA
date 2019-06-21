/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAgentTest
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

#include "albaAgentTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "albaAgent.h"
#include "albaAgentEventQueue.h"
#include "albaSmartPointer.h"

enum my_events 
{
  TEST_EVENT = MINID + 1,
  TEST_EVENT2,
  TEST_EVENT3,
  MCH_TEST_CHANNEL,
  MCH_TEST_CHANNEL2, 
  MCH_TEST_CHANNEL3
};


/** a concrete agent used for testing node processing and routing among events. */
class albaAgentRouter: public albaAgent
{
public:
  albaAgentRouter() {m_LastEventId=-1;m_LastChannelId=-1;}

  albaTypeMacro(albaAgentRouter,albaAgent);

  /** process events incoming events */
  virtual void OnEvent(albaEventBase *event);

  albaID m_LastEventId;
  albaID m_LastChannelId;
};

albaCxxTypeMacro(albaAgentRouter)

void albaAgentRouter::OnEvent(albaEventBase *event)
{
  m_LastChannelId = event->GetChannel();
  if (event->GetChannel() == MCH_TEST_CHANNEL)
  { 
    m_LastEventId=event->GetId();
    switch (m_LastEventId)
    {
    case TEST_EVENT:
      std::cout<<"agent m_Router received event with ID: MCH_TEST_CHANNEL"<<std::endl;
      break;
    default:
      std::cout<<"unknown event Id on MCH_TEST_CHANNEL channel, Id="<<m_LastEventId<<std::endl;
    }
    ;
  }
  else
  {
    m_LastEventId=-10; // used to identify the forwarding has been accomplished
    // pass to superclass to forward the event
    Superclass::OnEvent(event);
  }
}

/** a concrete agent used as a sink for events. */
class albaTestAgent: public albaAgent
{
public:
  albaTypeMacro(albaTestAgent,albaAgent);
  albaTestAgent() {m_LastEventId=-1;m_LastChannelId=-1;}
  albaID m_LastEventId;
  albaID m_LastChannelId;

  /** process events incoming events */
  virtual void OnEvent(albaEventBase *event);

};

albaCxxTypeMacro(albaTestAgent)


void albaTestAgent::OnEvent(albaEventBase *event)
{
  m_LastChannelId = event->GetChannel();   
  m_LastEventId=event->GetId();

  switch (m_LastEventId)
  {
  case TEST_EVENT2:
    std::cout<<"agent m_Router received event with ID: MCH_TEST_CHANNEL2"<<std::endl;
    break;
  default:
    std::cout<<"unknown event Id on MCH_TEST_CHANNEL2 channel, Id="<<m_LastEventId<<std::endl;
  }

  // do not forward to any other
}


void albaAgentTest::BeforeTest()
{
  m_Source = albaTestAgent::New(); // used just a m_Source for events
  m_Source->SetName("m_Source");
  m_Router = albaAgentRouter::New();
  m_Router->SetName("m_Router");
  m_Sink1 = albaTestAgent::New();
  m_Sink1->SetName("m_Sink1");
  m_Sink2 = albaTestAgent::New();
  m_Sink2->SetName("m_Sink2");
  m_Sink3 = albaTestAgent::New();
  m_Sink3->SetName("m_Sink3");
  m_Sink4 = albaTestAgent::New();
  m_Sink4->SetName("m_Sink4");
}

void albaAgentTest::AfterTest()
{
  albaDEL(m_Source);
  albaDEL(m_Router);
  albaDEL(m_Sink1);
  albaDEL(m_Sink2);
  albaDEL(m_Sink3);
  albaDEL(m_Sink4);
}

void albaAgentTest::TestFixture()
{
  // just to see that the fixture has no leaks.  
  m_Router->AddObserver(m_Sink2,MCH_TEST_CHANNEL2);
  m_Router->AddObserver(m_Sink1,MCH_TEST_CHANNEL2);
	m_Router->AddObserver(m_Sink3,MCH_TEST_CHANNEL3);

  m_Sink4->SetListener(m_Sink4); // test SetListener() API

  albaEventBase event1(m_Source,TEST_EVENT,NULL,MCH_TEST_CHANNEL);
  albaEventBase event2(m_Source,TEST_EVENT,NULL,MCH_TEST_CHANNEL);
  albaEventBase event3(m_Source,TEST_EVENT,NULL,MCH_TEST_CHANNEL);
  albaEventBase event4(m_Source,TEST_EVENT,NULL,MCH_TEST_CHANNEL);

}


void albaAgentTest::TestConstructorDestructor()
{
  albaTestAgent *testAgent = albaTestAgent::New();
  albaDEL(testAgent);
}

void albaAgentTest::TestAddRemoveObserver()
{
  m_Router->AddObserver(m_Sink1,MCH_TEST_CHANNEL2);
  std::vector<albaObserver *> observersList;
  m_Router->GetObservers(MCH_TEST_CHANNEL2, observersList);
  CPPUNIT_ASSERT(observersList.size() == 1);
  
  m_Router->RemoveObserver(m_Sink1);
  m_Router->GetObservers(MCH_TEST_CHANNEL2, observersList);
  CPPUNIT_ASSERT(observersList.size() == 0);

};

void albaAgentTest::TestRemoveAllObservers()
{
  m_Router->AddObserver(m_Sink2,MCH_TEST_CHANNEL2);
  m_Router->AddObserver(m_Sink1,MCH_TEST_CHANNEL2);

  std::vector<albaObserver *> observersList;
  m_Router->GetObservers(MCH_TEST_CHANNEL2, observersList);
  CPPUNIT_ASSERT(observersList.size() == 2);

  m_Router->RemoveAllObservers();
  m_Router->GetObservers(MCH_TEST_CHANNEL2, observersList);
  CPPUNIT_ASSERT(observersList.size() == 0);

};


void albaAgentTest::TestHasObservers()
{
  CPPUNIT_ASSERT(m_Router->HasObservers(MCH_TEST_CHANNEL2) == false);
  m_Router->AddObserver(m_Sink1,MCH_TEST_CHANNEL2);
  CPPUNIT_ASSERT(m_Router->HasObservers(MCH_TEST_CHANNEL2) == true);
};  

void albaAgentTest::TestGetObservers()
{
  m_Router->AddObserver(m_Sink1,MCH_TEST_CHANNEL2);
  
  std::vector<albaObserver *> observersList;
  m_Router->GetObservers(MCH_TEST_CHANNEL2, observersList);
  CPPUNIT_ASSERT(observersList.size() == 1);
  
  albaObserver *o1 = observersList[0] ;    
  CPPUNIT_ASSERT(o1 == m_Sink1);
};

void albaAgentTest::TestInitialize()
{
  // untestable... does nothing... this is only to show leaks if present
  m_Router->Initialize();
};

void albaAgentTest::TestShutdown()
{
  // untestable... does nothing... this is only to show leaks if present
  m_Router->Shutdown();
};

void albaAgentTest::TestIsInitialized()
{
  // untestable... does nothing... this is only to show leaks if present
  CPPUNIT_ASSERT(m_Router->IsInitialized() == false);
  m_Router->Initialize();
  
  CPPUNIT_ASSERT(m_Router->IsInitialized() == true);
 
};

void albaAgentTest::TestSetGetName()
{
  m_Router->SetName("pippo");
  CPPUNIT_ASSERT(strcmp(m_Router->GetName(),"pippo")  == 0);
};
  
