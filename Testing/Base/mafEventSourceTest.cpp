/*=========================================================================

 Program: MAF2
 Module: mafEventSourceTest
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
#include "mafEventSourceTest.h"

#include "mafReferenceCounted.h"
#include "mafEventSource.h"
#include "vtkMAFSmartPointer.h"
#include "mafEvent.h"

#include <iostream>
#include <utility>

//creation of test object
class mafTestObject: public mafReferenceCounted
{
public:
  mafTypeMacro(mafTestObject,mafReferenceCounted);
};

mafCxxTypeMacro(mafTestObject);

class myObserver: public mafObserver
{
public:
  myObserver() {m_InvokedEvent = false;};
  ~myObserver() {};

  bool m_InvokedEvent;

  void OnEvent(mafEventBase *e) {m_InvokedEvent = true;};
};


//----------------------------------------------------------------------------
void mafEventSourceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafEventSourceTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafEventSourceTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafEventSourceTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafEventSource eventA;
}
//----------------------------------------------------------------------------
void mafEventSourceTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafEventSource *eventA = new mafEventSource();
  cppDEL(eventA);
}
//----------------------------------------------------------------------------
void mafEventSourceTest::TestAddObserver()
//----------------------------------------------------------------------------
{
  mafEventSource *eventA = new mafEventSource;

  std::vector<mafObserver *> observers;

  myObserver myOb;

  //Add an observer
  eventA->AddObserver(&myOb);

  eventA->GetObservers(observers);
  
  //Check if the first element oh the list of observers is the one added
  CPPUNIT_ASSERT(observers[0]==&myOb);

  cppDEL(eventA);
}
//----------------------------------------------------------------------------
void mafEventSourceTest::TestRemoveObserver()
//----------------------------------------------------------------------------
{
  mafEventSource *eventA = new mafEventSource();

  myObserver myOb;

  //Add an observer
  eventA->AddObserver(&myOb);

  //Check if the observers added is observer of eventA
  bool check = eventA->IsObserver(&myOb);

  CPPUNIT_ASSERT(check);

  //Remove Observer
  eventA->RemoveObserver(&myOb);

  //Check if the observers added and than removed is no more an observer of eventA
  check = eventA->IsObserver(&myOb);

  CPPUNIT_ASSERT(check==false);

  cppDEL(eventA);
}
//----------------------------------------------------------------------------
void mafEventSourceTest::TestIsObserver()
//----------------------------------------------------------------------------
{ 
  mafEventSource *eventA = new mafEventSource();
  std::vector<mafObserver *> observers;

  myObserver myOb;
  myObserver myOb2;

  //Add the first observer
  eventA->AddObserver(&myOb);

  
  CPPUNIT_ASSERT(eventA->IsObserver(&myOb)==true);
  CPPUNIT_ASSERT(eventA->IsObserver(&myOb2)==false);

  cppDEL(eventA);
}
//----------------------------------------------------------------------------
void mafEventSourceTest::TestRemoveAllObservers()
//----------------------------------------------------------------------------
{
  mafEventSource *eventA = new mafEventSource();
  std::vector<mafObserver *> observers;

  myObserver myOb;
  myObserver myOb2;

  //Add 2 observers
  eventA->AddObserver(&myOb);
  eventA->AddObserver(&myOb2);

  //Check if the observers have been added
  CPPUNIT_ASSERT(eventA->IsObserver(&myOb)==true);
  CPPUNIT_ASSERT(eventA->IsObserver(&myOb2)==true);

  //Remove all observers
  eventA->RemoveAllObservers();

  eventA->GetObservers(observers);

  //Check if the observers have been removed
  CPPUNIT_ASSERT(eventA->IsObserver(&myOb)==false);
  CPPUNIT_ASSERT(eventA->IsObserver(&myOb2)==false);  
  CPPUNIT_ASSERT(observers.size()==0);

  cppDEL(eventA);
}
//----------------------------------------------------------------------------
void mafEventSourceTest::TestHasObservers()
//----------------------------------------------------------------------------
{
  mafEventSource *eventA = new mafEventSource();

  myObserver myOb;

  //Check if event has no observers
  CPPUNIT_ASSERT(eventA->HasObservers()==false);

  //Add an observer
  eventA->AddObserver(&myOb);

  //Check if event has observers
  CPPUNIT_ASSERT(eventA->HasObservers()==true);
  
  cppDEL(eventA);
}
//----------------------------------------------------------------------------
void mafEventSourceTest::TestInvokeEvent()
//----------------------------------------------------------------------------
{
   mafEventSource *eventA = new mafEventSource;
   mafEventBase *eventSend = new mafEventBase();
  

   myObserver myOb;

   //Add an observer
   eventA->AddObserver(&myOb);

   //Invoke OnEvent of the observer MyOb
   eventA->InvokeEvent(eventSend);

   //m_InvokedEvent turn to true if OnEvent of myOb has been invoked
   CPPUNIT_ASSERT(myOb.m_InvokedEvent==true);

   cppDEL(eventA);
   cppDEL(eventSend);
}
//----------------------------------------------------------------------------
void mafEventSourceTest::TestSetData()
//----------------------------------------------------------------------------
{
  mafEventSource *eventA = new mafEventSource;

  myObserver data;

  eventA->SetData((void*)&data);

  CPPUNIT_ASSERT(eventA->GetData() == &data);

  cppDEL(eventA);
}
//----------------------------------------------------------------------------
void mafEventSourceTest::TestSetOwner()
//----------------------------------------------------------------------------
{
  mafEventSource *eventA = new mafEventSource;

  myObserver  owner;  

  eventA->SetOwner((void*)&owner);

  CPPUNIT_ASSERT(eventA->GetOwner() == &owner);

  cppDEL(eventA);
}
//----------------------------------------------------------------------------
void mafEventSourceTest::TestSetChannel()
//----------------------------------------------------------------------------
{
  mafEventSource *eventA = new mafEventSource;

  mafID ch = MCH_UP;

  eventA->SetChannel(ch);

  CPPUNIT_ASSERT(eventA->GetChannel() == MCH_UP);

  cppDEL(eventA);
}
