/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafObserverTest.cpp,v $
Language:  C++
Date:      $Date: 2007-07-04 07:46:13 $
Version:   $Revision: 1.2 $
Authors:   Roberto Mucci
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

#include <cppunit/config/SourcePrefix.h>
#include "mafObserverTest.h"

#include "mafReferenceCounted.h" 
#include "mafEventSource.h"
#include "mafObject.h"
#include "mafObserver.h"
#include "mafEventBase.h"
#include "mafString.h"

#include "vtkMAFSmartPointer.h"

#include <iostream>
#include <utility>

//creation of test object
class mafTestObject: public mafReferenceCounted
{
public:
  mafTypeMacro(mafTestObject,mafReferenceCounted);
};

mafCxxTypeMacro(mafTestObject);

/** a subject class, i.e. a class with a couple of event source members observer class. */
class mafSubjectTestClass: public mafObject
{
public:
  mafTypeMacro(mafSubjectTestClass,mafObject)
  mafSubjectTestClass(const char *name=NULL):EventDummy(this),EventFoo(this),Name(name) {}
  mafEventSource &GetEventDummy() {return EventDummy;} 
  mafEventSource &GetEventFoo() {return EventFoo;}
  mafString &GetName() {return Name;}

  /** some event Ids */
  enum my_ids {ID_MY_OWN=ID_NO_EVENT+1, ID_DUMMY, ID_FOO};

  void DoSomeThing() {EventDummy.InvokeEvent(this);}
  void DoSomeThingElse() {EventFoo.InvokeEvent(this);}
  void Print(ostream &out) {out<<Name<<endl;}

protected:
  mafEventSource  EventDummy;
  mafEventSource  EventFoo;
  mafString       Name;
};

mafCxxTypeMacro(mafSubjectTestClass);

/** an observer class used to test event reception. */
class mafObserverTestClass: public mafObject, public mafObserver
{
public:
  mafTypeMacro(mafObserverTestClass, mafObject);
  mafObserverTestClass(const char *name=NULL):Name(name),SkipNext(false) {}

  virtual void OnEvent(mafEventBase *event) {LastEvent=*event;if (SkipNext) event->SkipNext();};

  mafString     Name;
  mafEventBase  LastEvent; // used to store last receive event contents
  bool          SkipNext; // flag to force the object setting the skip flag in the event
};

mafCxxTypeMacro(mafObserverTestClass);

//----------------------------------------------------------------------------
void mafObserverTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafObserverTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafObserverTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafObserverTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafObserverTestClass observer;
}
//----------------------------------------------------------------------------
void mafObserverTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafObserverTestClass *observer = new mafObserverTestClass;
  cppDEL(observer);
}
//----------------------------------------------------------------------------
void mafObserverTest::TestObserver()
//----------------------------------------------------------------------------
{
  // create some subject objects
  mafSubjectTestClass first_subject("first_subject");
  mafSubjectTestClass second_subject("second_subject");
  mafSubjectTestClass third_subject("third_subject");
  
  // test the owner member variable is set correctly
  CPPUNIT_ASSERT(first_subject.GetEventDummy().GetOwner() == &first_subject);

  // create some observer objects
  mafObserverTestClass first_observer("first_observer");
  mafObserverTestClass second_observer("second_observer");
  mafObserverTestClass third_observer("third_observer");
  
  // test multiple subject observation
  first_subject.GetEventDummy().AddObserver(first_observer);
  second_subject.GetEventDummy().AddObserver(first_observer);
  third_subject.GetEventDummy().AddObserver(first_observer);
  
  second_subject.GetEventFoo().AddObserver(first_observer);
  third_subject.GetEventFoo().AddObserver(first_observer);

  // test LastEvent is not set yet
  CPPUNIT_ASSERT(first_observer.LastEvent.GetSender() == NULL);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == ID_NO_EVENT);

  // create some data to be sent with the event
  mafString first_data="First Data";
  mafString second_data="Second Data";
  mafString third_data="Third Data";
  
  // send an event from the first subject
  first_subject.GetEventDummy().InvokeEvent(&first_subject,mafSubjectTestClass::ID_DUMMY,&first_data);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == mafSubjectTestClass::ID_DUMMY);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetData() == &first_data);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetSender() == first_observer.LastEvent.GetSource()->GetOwner());
 
  // send an event from the second subject
  second_subject.GetEventDummy().InvokeEvent(&second_subject,mafSubjectTestClass::ID_DUMMY,&second_data);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == mafSubjectTestClass::ID_DUMMY);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetSender() == &second_subject);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetData() == &second_data);
  
  // send an event from the third subject
  third_subject.GetEventDummy().InvokeEvent(&third_subject,mafSubjectTestClass::ID_DUMMY,&third_data);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == mafSubjectTestClass::ID_DUMMY);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetSender() == &third_subject);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetData() == &third_data);

  // test multiple subjects with other events
  second_subject.GetEventFoo().InvokeEvent(&second_subject,mafSubjectTestClass::ID_FOO,&second_data);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == mafSubjectTestClass::ID_FOO);
  third_subject.GetEventFoo().InvokeEvent(&third_subject,mafSubjectTestClass::ID_MY_OWN,&third_data);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == mafSubjectTestClass::ID_MY_OWN);

  // test multiple observers with priority
  first_subject.GetEventDummy().AddObserver(second_observer,10); // add second high priority observer
  second_observer.SkipNext = true; // set the skip flag to make other observers to be skipped
  first_subject.GetEventDummy().AddObserver(third_observer); // add a third observer

  first_subject.GetEventDummy().InvokeEvent(&first_subject,mafSubjectTestClass::ID_DUMMY,&first_data);

  // test priority and the event skipping: only second_observer should have received the ID_DUMMY event
  CPPUNIT_ASSERT(second_observer.LastEvent.GetId() == mafSubjectTestClass::ID_DUMMY);
  CPPUNIT_ASSERT(second_observer.LastEvent.GetSender() == &first_subject);

  // these two should have been skipped
  CPPUNIT_ASSERT(third_observer.LastEvent.GetId() != mafSubjectTestClass::ID_DUMMY);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() != mafSubjectTestClass::ID_DUMMY);
}