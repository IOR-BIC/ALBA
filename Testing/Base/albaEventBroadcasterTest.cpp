/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventBroadcasterTest
 Authors: Roberto Mucci
 
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
#include "albaEventBroadcasterTest.h"

#include "albaReferenceCounted.h"
#include "albaEventBroadcaster.h"
#include "vtkALBASmartPointer.h"
#include "albaEvent.h"

#include <iostream>
#include <utility>




class myObserver: public albaObserver
{
public:
  myObserver() {m_InvokedEvent = false;};
  ~myObserver() {};

  bool m_InvokedEvent;

  void OnEvent(albaEventBase *e) {m_InvokedEvent = true;};
};


//----------------------------------------------------------------------------
void albaEventBroadcasterTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaEventBroadcasterTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaEventBroadcaster eventBrc;
}
//----------------------------------------------------------------------------
void albaEventBroadcasterTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaEventBroadcaster *eventBrc = new albaEventBroadcaster();
  cppDEL(eventBrc);
}
//----------------------------------------------------------------------------
void albaEventBroadcasterTest::TestAddObserver()
//----------------------------------------------------------------------------
{
  albaEventBroadcaster *eventBrc = new albaEventBroadcaster;

  std::vector<albaObserver *> observers;

  myObserver myOb;

  //Add an observer
  eventBrc->AddObserver(&myOb);

  eventBrc->GetObservers(observers);
  
  //Check if the first element oh the list of observers is the one added
  CPPUNIT_ASSERT(observers[0]==&myOb);

  cppDEL(eventBrc);
}
//----------------------------------------------------------------------------
void albaEventBroadcasterTest::TestRemoveObserver()
//----------------------------------------------------------------------------
{
  albaEventBroadcaster *eventBrc = new albaEventBroadcaster();

  myObserver myOb;

  //Add an observer
  eventBrc->AddObserver(&myOb);

  //Check if the observers added is observer of eventBrc
  bool check = eventBrc->IsObserver(&myOb);

  CPPUNIT_ASSERT(check);

  //Remove Observer
  eventBrc->RemoveObserver(&myOb);

  //Check if the observers added and than removed is no more an observer of eventBrc
  check = eventBrc->IsObserver(&myOb);

  CPPUNIT_ASSERT(check==false);

  cppDEL(eventBrc);
}
//----------------------------------------------------------------------------
void albaEventBroadcasterTest::TestIsObserver()
//----------------------------------------------------------------------------
{ 
  albaEventBroadcaster *eventBrc = new albaEventBroadcaster();
  std::vector<albaObserver *> observers;

  myObserver myOb;
  myObserver myOb2;

  //Add the first observer
  eventBrc->AddObserver(&myOb);

  
  CPPUNIT_ASSERT(eventBrc->IsObserver(&myOb)==true);
  CPPUNIT_ASSERT(eventBrc->IsObserver(&myOb2)==false);

  cppDEL(eventBrc);
}
//----------------------------------------------------------------------------
void albaEventBroadcasterTest::TestRemoveAllObservers()
//----------------------------------------------------------------------------
{
  albaEventBroadcaster *eventBrc = new albaEventBroadcaster();
  std::vector<albaObserver *> observers;

  myObserver myOb;
  myObserver myOb2;

  //Add 2 observers
  eventBrc->AddObserver(&myOb);
  eventBrc->AddObserver(&myOb2);

  //Check if the observers have been added
  CPPUNIT_ASSERT(eventBrc->IsObserver(&myOb)==true);
  CPPUNIT_ASSERT(eventBrc->IsObserver(&myOb2)==true);

  //Remove all observers
  eventBrc->RemoveAllObservers();

  eventBrc->GetObservers(observers);

  //Check if the observers have been removed
  CPPUNIT_ASSERT(eventBrc->IsObserver(&myOb)==false);
  CPPUNIT_ASSERT(eventBrc->IsObserver(&myOb2)==false);  
  CPPUNIT_ASSERT(observers.size()==0);

  cppDEL(eventBrc);
}
//----------------------------------------------------------------------------
void albaEventBroadcasterTest::TestHasObservers()
//----------------------------------------------------------------------------
{
  albaEventBroadcaster *eventBrc = new albaEventBroadcaster();

  myObserver myOb;

  //Check if event has no observers
  CPPUNIT_ASSERT(eventBrc->HasObservers()==false);

  //Add an observer
  eventBrc->AddObserver(&myOb);

  //Check if event has observers
  CPPUNIT_ASSERT(eventBrc->HasObservers()==true);
  
  cppDEL(eventBrc);
}
//----------------------------------------------------------------------------
void albaEventBroadcasterTest::TestInvokeEvent()
//----------------------------------------------------------------------------
{
   albaEventBroadcaster *eventBrc = new albaEventBroadcaster;
   albaEventBase *eventSend = new albaEventBase();
  

   myObserver myOb;

   //Add an observer
   eventBrc->AddObserver(&myOb);

   //Invoke OnEvent of the observer MyOb
   eventBrc->InvokeEvent(eventSend);

   //m_InvokedEvent turn to true if OnEvent of myOb has been invoked
   CPPUNIT_ASSERT(myOb.m_InvokedEvent==true);

   cppDEL(eventBrc);
   cppDEL(eventSend);
}
//----------------------------------------------------------------------------
void albaEventBroadcasterTest::TestSetChannel()
//----------------------------------------------------------------------------
{
  albaEventBroadcaster *eventBrc = new albaEventBroadcaster;

  albaID ch = MCH_UP;

  eventBrc->SetChannel(ch);

  CPPUNIT_ASSERT(eventBrc->GetChannel() == MCH_UP);

  cppDEL(eventBrc);
}
