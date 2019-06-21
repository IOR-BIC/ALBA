/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaObserverTest
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
#include "albaObserverTest.h"

#include "albaReferenceCounted.h" 
#include "albaEventBroadcaster.h"
#include "albaObject.h"
#include "albaObserver.h"
#include "albaEventBase.h"
#include "albaString.h"

#include "vtkALBASmartPointer.h"

#include <iostream>
#include <utility>



/** a subject class, i.e. a class with a couple of event source members observer class. */
class albaSubjectTestClass: public albaObject
{
public:
  albaTypeMacro(albaSubjectTestClass,albaObject)
  albaSubjectTestClass(const char *name=NULL):m_DummyBroadcaster(this),m_FooBroadcaster(this),m_Name(name) {}
  albaEventBroadcaster *GetDummyBroadcaster() {return &m_DummyBroadcaster;} 
	albaEventBroadcaster *GetFooBroadcaster() {return &m_FooBroadcaster;}
  albaString &GetName() {return m_Name;}

  /** some event Ids */
  enum my_ids {ID_MY_OWN=ID_NO_EVENT+1, ID_DUMMY, ID_FOO};

  void DoSomeThing() {m_DummyBroadcaster.InvokeEvent(this);}
  void DoSomeThingElse() {m_FooBroadcaster.InvokeEvent(this);}
  void Print(ostream &out) {out<<m_Name<<endl;}

protected:
	albaEventBroadcaster  m_DummyBroadcaster;
	albaEventBroadcaster  m_FooBroadcaster;
  albaString       m_Name;
};

albaCxxTypeMacro(albaSubjectTestClass);

/** an observer class used to test event reception. */
class albaObserverTestClass: public albaObject, public albaObserver
{
public:
  albaTypeMacro(albaObserverTestClass, albaObject);
  albaObserverTestClass(const char *name=NULL):Name(name) {}

  virtual void OnEvent(albaEventBase *event) {LastEvent=*event;};

  albaString     Name;
  albaEventBase  LastEvent; // used to store last receive event contents
};

albaCxxTypeMacro(albaObserverTestClass);

//----------------------------------------------------------------------------
void albaObserverTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaObserverTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaObserverTestClass observer;
}
//----------------------------------------------------------------------------
void albaObserverTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaObserverTestClass *observer = new albaObserverTestClass;
  cppDEL(observer);
}
//----------------------------------------------------------------------------
void albaObserverTest::TestObserver()
//----------------------------------------------------------------------------
{
  // create some subject objects
  albaSubjectTestClass first_subject("first_subject");
  albaSubjectTestClass second_subject("second_subject");
  albaSubjectTestClass third_subject("third_subject");
  
  // create some observer objects
  albaObserverTestClass first_observer("first_observer");
  albaObserverTestClass second_observer("second_observer");
  albaObserverTestClass third_observer("third_observer");
  
  // test multiple subject observation
  first_subject.GetDummyBroadcaster()->AddObserver(first_observer);
  second_subject.GetDummyBroadcaster()->AddObserver(first_observer);
  third_subject.GetDummyBroadcaster()->AddObserver(first_observer);
  
  second_subject.GetFooBroadcaster()->AddObserver(first_observer);
  third_subject.GetFooBroadcaster()->AddObserver(first_observer);

  // test LastEvent is not set yet
  CPPUNIT_ASSERT(first_observer.LastEvent.GetSender() == NULL);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == ID_NO_EVENT);

  // create some data to be sent with the event
  albaString first_data="First Data";
  albaString second_data="Second Data";
  albaString third_data="Third Data";
  
  // send an event from the first subject
  first_subject.GetDummyBroadcaster()->InvokeEvent(&first_subject, albaSubjectTestClass::ID_DUMMY,&first_data);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == albaSubjectTestClass::ID_DUMMY);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetData() == &first_data);
 
  // send an event from the second subject
  second_subject.GetDummyBroadcaster()->InvokeEvent(&second_subject, albaSubjectTestClass::ID_DUMMY,&second_data);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == albaSubjectTestClass::ID_DUMMY);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetSender() == &second_subject);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetData() == &second_data);
  
  // send an event from the third subject
  third_subject.GetDummyBroadcaster()->InvokeEvent(&third_subject, albaSubjectTestClass::ID_DUMMY,&third_data);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == albaSubjectTestClass::ID_DUMMY);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetSender() == &third_subject);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetData() == &third_data);

  // test multiple subjects with other events
  second_subject.GetFooBroadcaster()->InvokeEvent(&second_subject, albaSubjectTestClass::ID_FOO,&second_data);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == albaSubjectTestClass::ID_FOO);
	CPPUNIT_ASSERT(first_observer.LastEvent.GetSender() == &second_subject);
  third_subject.GetFooBroadcaster()->InvokeEvent(&third_subject, albaSubjectTestClass::ID_MY_OWN,&third_data);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == albaSubjectTestClass::ID_MY_OWN);
	CPPUNIT_ASSERT(first_observer.LastEvent.GetSender() == &third_subject);


  // test multiple observers
  first_subject.GetDummyBroadcaster()->AddObserver(second_observer); // add second observer
  first_subject.GetDummyBroadcaster()->AddObserver(third_observer); // add a third observer

  first_subject.GetDummyBroadcaster()->InvokeEvent(&first_subject, albaSubjectTestClass::ID_DUMMY,&first_data);

  CPPUNIT_ASSERT(second_observer.LastEvent.GetId() == albaSubjectTestClass::ID_DUMMY);
  CPPUNIT_ASSERT(second_observer.LastEvent.GetSender() == &first_subject);
  CPPUNIT_ASSERT(third_observer.LastEvent.GetId() == albaSubjectTestClass::ID_DUMMY);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == albaSubjectTestClass::ID_DUMMY);
}