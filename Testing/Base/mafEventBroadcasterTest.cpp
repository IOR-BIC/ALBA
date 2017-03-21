/*=========================================================================

 Program: MAF2
 Module: mafEventBroadcasterTest
 Authors: Roberto Mucci
 
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

#include <cppunit/config/SourcePrefix.h>
#include "mafEventBroadcasterTest.h"

#include "mafReferenceCounted.h"
#include "mafEventBroadcaster.h"
#include "vtkMAFSmartPointer.h"
#include "mafEvent.h"

#include <iostream>
#include <utility>




class myObserver: public mafObserver
{
public:
  myObserver() {m_InvokedEvent = false;};
  ~myObserver() {};

  bool m_InvokedEvent;

  void OnEvent(mafEventBase *e) {m_InvokedEvent = true;};
};


//----------------------------------------------------------------------------
void mafEventBroadcasterTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafEventBroadcasterTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafEventBroadcaster eventBrc;
}
//----------------------------------------------------------------------------
void mafEventBroadcasterTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafEventBroadcaster *eventBrc = new mafEventBroadcaster();
  cppDEL(eventBrc);
}
//----------------------------------------------------------------------------
void mafEventBroadcasterTest::TestAddObserver()
//----------------------------------------------------------------------------
{
  mafEventBroadcaster *eventBrc = new mafEventBroadcaster;

  std::vector<mafObserver *> observers;

  myObserver myOb;

  //Add an observer
  eventBrc->AddObserver(&myOb);

  eventBrc->GetObservers(observers);
  
  //Check if the first element oh the list of observers is the one added
  CPPUNIT_ASSERT(observers[0]==&myOb);

  cppDEL(eventBrc);
}
//----------------------------------------------------------------------------
void mafEventBroadcasterTest::TestRemoveObserver()
//----------------------------------------------------------------------------
{
  mafEventBroadcaster *eventBrc = new mafEventBroadcaster();

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
void mafEventBroadcasterTest::TestIsObserver()
//----------------------------------------------------------------------------
{ 
  mafEventBroadcaster *eventBrc = new mafEventBroadcaster();
  std::vector<mafObserver *> observers;

  myObserver myOb;
  myObserver myOb2;

  //Add the first observer
  eventBrc->AddObserver(&myOb);

  
  CPPUNIT_ASSERT(eventBrc->IsObserver(&myOb)==true);
  CPPUNIT_ASSERT(eventBrc->IsObserver(&myOb2)==false);

  cppDEL(eventBrc);
}
//----------------------------------------------------------------------------
void mafEventBroadcasterTest::TestRemoveAllObservers()
//----------------------------------------------------------------------------
{
  mafEventBroadcaster *eventBrc = new mafEventBroadcaster();
  std::vector<mafObserver *> observers;

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
void mafEventBroadcasterTest::TestHasObservers()
//----------------------------------------------------------------------------
{
  mafEventBroadcaster *eventBrc = new mafEventBroadcaster();

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
void mafEventBroadcasterTest::TestInvokeEvent()
//----------------------------------------------------------------------------
{
   mafEventBroadcaster *eventBrc = new mafEventBroadcaster;
   mafEventBase *eventSend = new mafEventBase();
  

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
void mafEventBroadcasterTest::TestSetChannel()
//----------------------------------------------------------------------------
{
  mafEventBroadcaster *eventBrc = new mafEventBroadcaster;

  mafID ch = MCH_UP;

  eventBrc->SetChannel(ch);

  CPPUNIT_ASSERT(eventBrc->GetChannel() == MCH_UP);

  cppDEL(eventBrc);
}
