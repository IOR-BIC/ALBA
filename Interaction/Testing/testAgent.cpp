#include "mafAgent.h"
#include "mafAgentEventQueue.h"
#include "mafSmartPointer.h"

#include <iostream>

enum my_events 
{
  TEST_EVENT = MINID + 1,
  TEST_EVENT2,
  TEST_EVENT3,
  MCH_TEST_CHANNEL,
  MCH_TEST_CHANNEL2, 
  MCH_TEST_CHANNEL3
};

//-------------------------------------------------------------------------
/** a concrete agent used for testing node processing and routing among events. */
class mafAgentRouter: public mafAgent
//-------------------------------------------------------------------------
{
public:
  mafAgentRouter() {m_LastEventId=-1;m_LastChannelId=-1;}
  
  mafTypeMacro(mafAgentRouter,mafAgent);
  
  /** process events incoming events */
  virtual void OnEvent(mafEventBase *event);

  mafID m_LastEventId;
  mafID m_LastChannelId;
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafAgentRouter)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void mafAgentRouter::OnEvent(mafEventBase *event)
//-------------------------------------------------------------------------
{
  m_LastChannelId = event->GetChannel();
  if (event->GetChannel() == MCH_TEST_CHANNEL)
  { 
    m_LastEventId=event->GetId();
    switch (m_LastEventId)
    {
      case TEST_EVENT:
        std::cerr<<"agent router received event with ID: MCH_TEST_CHANNEL"<<std::endl;
        break;
      default:
        std::cerr<<"unknown event Id on MCH_TEST_CHANNEL channel, Id="<<m_LastEventId<<std::endl;
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

//-------------------------------------------------------------------------
/** a concrete agent used as a sink for events. */
class mafTestAgent: public mafAgent
//-------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafTestAgent,mafAgent);
  mafTestAgent() {m_LastEventId=-1;m_LastChannelId=-1;}
  mafID m_LastEventId;
  mafID m_LastChannelId;

  /** process events incoming events */
  virtual void OnEvent(mafEventBase *event);
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafTestAgent)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void mafTestAgent::OnEvent(mafEventBase *event)
//-------------------------------------------------------------------------
{
  m_LastChannelId = event->GetChannel();   
  m_LastEventId=event->GetId();

  switch (m_LastEventId)
  {
    case TEST_EVENT2:
      std::cerr<<"agent router received event with ID: MCH_TEST_CHANNEL2"<<std::endl;
      break;
    default:
      std::cerr<<"unknown event Id on MCH_TEST_CHANNEL2 channel, Id="<<m_LastEventId<<std::endl;
  }

  // do not forward to any other
}


//-------------------------------------------------------------------------
int main()
//-------------------------------------------------------------------------
{
  mafSmartPointer<mafTestAgent> source; // used just a source for events
  source->SetName("source");
  mafSmartPointer<mafAgentRouter> router;
  router->SetName("router");
  mafSmartPointer<mafTestAgent> sink1;
  sink1->SetName("sink1");
  mafSmartPointer<mafTestAgent> sink2;
  sink2->SetName("sink2");
  mafSmartPointer<mafAgentEventQueue> sink3;
  sink3->SetName("sink3");
  mafSmartPointer<mafTestAgent> sink4;
  sink4->SetName("sink4");

  router->AddObserver(sink2,MCH_TEST_CHANNEL2,-10); // test priority of serving
  router->AddObserver(sink1,MCH_TEST_CHANNEL2);

  sink3->PlugEventSource(router,MCH_TEST_CHANNEL3); // test PlugEventSource API in place of AddObserver()

  sink4->SetListener(sink4); // test SetListener() API

  mafEventBase event1(source,TEST_EVENT,NULL,MCH_TEST_CHANNEL);
  mafEventBase event2(source,TEST_EVENT,NULL,MCH_TEST_CHANNEL);
  mafEventBase event3(source,TEST_EVENT,NULL,MCH_TEST_CHANNEL);
  mafEventBase event4(source,TEST_EVENT,NULL,MCH_TEST_CHANNEL);

  
  
  std::cerr<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
