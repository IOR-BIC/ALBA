/*=========================================================================

 Program: MAF2
 Module: mafObserverTest
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
#include "mafObserverTest.h"

#include "mafReferenceCounted.h" 
#include "mafEventBroadcaster.h"
#include "mafObject.h"
#include "mafObserver.h"
#include "mafEventBase.h"
#include "mafString.h"

#include "vtkMAFSmartPointer.h"

#include <iostream>
#include <utility>



/** a subject class, i.e. a class with a couple of event source members observer class. */
class mafSubjectTestClass: public mafObject
{
public:
  mafTypeMacro(mafSubjectTestClass,mafObject)
  mafSubjectTestClass(const char *name=NULL):m_DummyBroadcaster(this),m_FooBroadcaster(this),m_Name(name) {}
  mafEventBroadcaster *GetDummyBroadcaster() {return &m_DummyBroadcaster;} 
	mafEventBroadcaster *GetFooBroadcaster() {return &m_FooBroadcaster;}
  mafString &GetName() {return m_Name;}

  /** some event Ids */
  enum my_ids {ID_MY_OWN=ID_NO_EVENT+1, ID_DUMMY, ID_FOO};

  void DoSomeThing() {m_DummyBroadcaster.InvokeEvent(this);}
  void DoSomeThingElse() {m_FooBroadcaster.InvokeEvent(this);}
  void Print(ostream &out) {out<<m_Name<<endl;}

protected:
	mafEventBroadcaster  m_DummyBroadcaster;
	mafEventBroadcaster  m_FooBroadcaster;
  mafString       m_Name;
};

mafCxxTypeMacro(mafSubjectTestClass);

/** an observer class used to test event reception. */
class mafObserverTestClass: public mafObject, public mafObserver
{
public:
  mafTypeMacro(mafObserverTestClass, mafObject);
  mafObserverTestClass(const char *name=NULL):Name(name) {}

  virtual void OnEvent(mafEventBase *event) {LastEvent=*event;};

  mafString     Name;
  mafEventBase  LastEvent; // used to store last receive event contents
};

mafCxxTypeMacro(mafObserverTestClass);

//----------------------------------------------------------------------------
void mafObserverTest::TestFixture()
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
  
  // create some observer objects
  mafObserverTestClass first_observer("first_observer");
  mafObserverTestClass second_observer("second_observer");
  mafObserverTestClass third_observer("third_observer");
  
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
  mafString first_data="First Data";
  mafString second_data="Second Data";
  mafString third_data="Third Data";
  
  // send an event from the first subject
  first_subject.GetDummyBroadcaster()->InvokeEvent(&first_subject, mafSubjectTestClass::ID_DUMMY,&first_data);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == mafSubjectTestClass::ID_DUMMY);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetData() == &first_data);
 
  // send an event from the second subject
  second_subject.GetDummyBroadcaster()->InvokeEvent(&second_subject, mafSubjectTestClass::ID_DUMMY,&second_data);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == mafSubjectTestClass::ID_DUMMY);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetSender() == &second_subject);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetData() == &second_data);
  
  // send an event from the third subject
  third_subject.GetDummyBroadcaster()->InvokeEvent(&third_subject, mafSubjectTestClass::ID_DUMMY,&third_data);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == mafSubjectTestClass::ID_DUMMY);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetSender() == &third_subject);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetData() == &third_data);

  // test multiple subjects with other events
  second_subject.GetFooBroadcaster()->InvokeEvent(&second_subject, mafSubjectTestClass::ID_FOO,&second_data);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == mafSubjectTestClass::ID_FOO);
	CPPUNIT_ASSERT(first_observer.LastEvent.GetSender() == &second_subject);
  third_subject.GetFooBroadcaster()->InvokeEvent(&third_subject, mafSubjectTestClass::ID_MY_OWN,&third_data);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == mafSubjectTestClass::ID_MY_OWN);
	CPPUNIT_ASSERT(first_observer.LastEvent.GetSender() == &third_subject);


  // test multiple observers
  first_subject.GetDummyBroadcaster()->AddObserver(second_observer); // add second observer
  first_subject.GetDummyBroadcaster()->AddObserver(third_observer); // add a third observer

  first_subject.GetDummyBroadcaster()->InvokeEvent(&first_subject, mafSubjectTestClass::ID_DUMMY,&first_data);

  CPPUNIT_ASSERT(second_observer.LastEvent.GetId() == mafSubjectTestClass::ID_DUMMY);
  CPPUNIT_ASSERT(second_observer.LastEvent.GetSender() == &first_subject);
  CPPUNIT_ASSERT(third_observer.LastEvent.GetId() == mafSubjectTestClass::ID_DUMMY);
  CPPUNIT_ASSERT(first_observer.LastEvent.GetId() == mafSubjectTestClass::ID_DUMMY);
}