/*=========================================================================

 Program: MAF2
 Module: mafAgentTest
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

#include "mafAgentTest.h"

#include <cppunit/config/SourcePrefix.h>
#include <iostream>

#include "mafAgent.h"
#include "mafAgentEventQueue.h"
#include "mafSmartPointer.h"

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
class mafAgentRouter: public mafAgent
{
public:
  mafAgentRouter() {m_LastEventId=-1;m_LastChannelId=-1;}

  mafTypeMacro(mafAgentRouter,mafAgent);

  /** process events incoming events */
  virtual void OnEvent(mafEventBase *event);

  mafID m_LastEventId;
  mafID m_LastChannelId;
};

mafCxxTypeMacro(mafAgentRouter)

void mafAgentRouter::OnEvent(mafEventBase *event)
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
class mafTestAgent: public mafAgent
{
public:
  mafTypeMacro(mafTestAgent,mafAgent);
  mafTestAgent() {m_LastEventId=-1;m_LastChannelId=-1;}
  mafID m_LastEventId;
  mafID m_LastChannelId;

  /** process events incoming events */
  virtual void OnEvent(mafEventBase *event);

};

mafCxxTypeMacro(mafTestAgent)


void mafTestAgent::OnEvent(mafEventBase *event)
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


void mafAgentTest::BeforeTest()
{
  m_Source = mafTestAgent::New(); // used just a m_Source for events
  m_Source->SetName("m_Source");
  m_Router = mafAgentRouter::New();
  m_Router->SetName("m_Router");
  m_Sink1 = mafTestAgent::New();
  m_Sink1->SetName("m_Sink1");
  m_Sink2 = mafTestAgent::New();
  m_Sink2->SetName("m_Sink2");
  m_Sink3 = mafTestAgent::New();
  m_Sink3->SetName("m_Sink3");
  m_Sink4 = mafTestAgent::New();
  m_Sink4->SetName("m_Sink4");
}

void mafAgentTest::AfterTest()
{
  mafDEL(m_Source);
  mafDEL(m_Router);
  mafDEL(m_Sink1);
  mafDEL(m_Sink2);
  mafDEL(m_Sink3);
  mafDEL(m_Sink4);
}

void mafAgentTest::TestFixture()
{
  // just to see that the fixture has no leaks.  
  m_Router->AddObserver(m_Sink2,MCH_TEST_CHANNEL2,-10); // test priority of serving
  m_Router->AddObserver(m_Sink1,MCH_TEST_CHANNEL2);
	m_Router->AddObserver(m_Sink3,MCH_TEST_CHANNEL3);

  m_Sink4->SetListener(m_Sink4); // test SetListener() API

  mafEventBase event1(m_Source,TEST_EVENT,NULL,MCH_TEST_CHANNEL);
  mafEventBase event2(m_Source,TEST_EVENT,NULL,MCH_TEST_CHANNEL);
  mafEventBase event3(m_Source,TEST_EVENT,NULL,MCH_TEST_CHANNEL);
  mafEventBase event4(m_Source,TEST_EVENT,NULL,MCH_TEST_CHANNEL);

}


void mafAgentTest::TestConstructorDestructor()
{
  mafTestAgent *testAgent = mafTestAgent::New();
  mafDEL(testAgent);
}

void mafAgentTest::TestAddRemoveObserver()
{
  m_Router->AddObserver(m_Sink1,MCH_TEST_CHANNEL2);
  std::vector<mafObserver *> observersList;
  m_Router->GetObservers(MCH_TEST_CHANNEL2, observersList);
  CPPUNIT_ASSERT(observersList.size() == 1);
  
  m_Router->RemoveObserver(m_Sink1);
  m_Router->GetObservers(MCH_TEST_CHANNEL2, observersList);
  CPPUNIT_ASSERT(observersList.size() == 0);

};

void mafAgentTest::TestRemoveAllObservers()
{
  m_Router->AddObserver(m_Sink2,MCH_TEST_CHANNEL2,-10);
  m_Router->AddObserver(m_Sink1,MCH_TEST_CHANNEL2);

  std::vector<mafObserver *> observersList;
  m_Router->GetObservers(MCH_TEST_CHANNEL2, observersList);
  CPPUNIT_ASSERT(observersList.size() == 2);

  m_Router->RemoveAllObservers();
  m_Router->GetObservers(MCH_TEST_CHANNEL2, observersList);
  CPPUNIT_ASSERT(observersList.size() == 0);

};


void mafAgentTest::TestHasObservers()
{
  CPPUNIT_ASSERT(m_Router->HasObservers(MCH_TEST_CHANNEL2) == false);
  m_Router->AddObserver(m_Sink1,MCH_TEST_CHANNEL2);
  CPPUNIT_ASSERT(m_Router->HasObservers(MCH_TEST_CHANNEL2) == true);
};  

void mafAgentTest::TestGetObservers()
{
  m_Router->AddObserver(m_Sink1,MCH_TEST_CHANNEL2);
  
  std::vector<mafObserver *> observersList;
  m_Router->GetObservers(MCH_TEST_CHANNEL2, observersList);
  CPPUNIT_ASSERT(observersList.size() == 1);
  
  mafObserver *o1 = observersList[0] ;    
  CPPUNIT_ASSERT(o1 == m_Sink1);
};

void mafAgentTest::TestInitialize()
{
  // untestable... does nothing... this is only to show leaks if present
  m_Router->Initialize();
};

void mafAgentTest::TestShutdown()
{
  // untestable... does nothing... this is only to show leaks if present
  m_Router->Shutdown();
};

void mafAgentTest::TestIsInitialized()
{
  // untestable... does nothing... this is only to show leaks if present
  CPPUNIT_ASSERT(m_Router->IsInitialized() == false);
  m_Router->Initialize();
  
  CPPUNIT_ASSERT(m_Router->IsInitialized() == true);
 
};

void mafAgentTest::TestSetGetName()
{
  m_Router->SetName("pippo");
  CPPUNIT_ASSERT(strcmp(m_Router->GetName(),"pippo")  == 0);
};
  
