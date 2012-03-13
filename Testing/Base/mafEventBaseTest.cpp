/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafEventBaseTest.cpp,v $
Language:  C++
Date:      $Date: 2007-06-07 09:23:16 $
Version:   $Revision: 1.2 $
Authors:   Daniele Giunchi
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
#include "mafEventBaseTest.h"

#include "mafReferenceCounted.h"
#include "mafEventSource.h"
#include "mafEventBase.h"

#include <iostream>
#include <utility>

//creation of test object
class mafTestObject: public mafReferenceCounted
{
public:
  mafTypeMacro(mafTestObject,mafReferenceCounted);
};

mafCxxTypeMacro(mafTestObject);

//----------------------------------------------------------------------------
void mafEventBaseTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafEventBaseTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafEventBaseTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafEventBaseTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafEventBase m;
}
//----------------------------------------------------------------------------
void mafEventBaseTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafEventBase *m = new mafEventBase();
  cppDEL(m);
}
//----------------------------------------------------------------------------
void mafEventBaseTest::TestAllConstructors()
//----------------------------------------------------------------------------
{
  typedef std::pair<int,const char *> TestType;
  TestType testData(10,"hello");

  mafTestObject testObject;

  // test constructor
  mafEventBase eventA(&testObject,5,&testData);
  CPPUNIT_ASSERT(eventA.GetId()==5);
  CPPUNIT_ASSERT(eventA.GetSender()==&testObject);
  CPPUNIT_ASSERT(eventA.GetData()==&testData);
  CPPUNIT_ASSERT(eventA.GetSource()==NULL);


  // test copy constructor
  mafEventBase eventB=eventA;
  CPPUNIT_ASSERT(eventB.GetId()==eventA.GetId());
  CPPUNIT_ASSERT(eventB.GetSender()==eventA.GetSender());
  CPPUNIT_ASSERT(eventB.GetData()==eventA.GetData());
  CPPUNIT_ASSERT(eventB.GetSource()==eventA.GetSource());
}
//----------------------------------------------------------------------------
void mafEventBaseTest::TestCopy()
//----------------------------------------------------------------------------
{
  typedef std::pair<int,const char *> TestType;
  TestType testData(10,"helloCopy");

  mafTestObject testObject;

  mafEventBase eventA(&testObject,5,&testData);

  // test deepcopy
  mafEventBase eventB;
  eventB.DeepCopy(&eventA);
  CPPUNIT_ASSERT(eventB.GetId()==eventA.GetId());
  CPPUNIT_ASSERT(eventB.GetSender()==eventA.GetSender());
  CPPUNIT_ASSERT(eventB.GetData()==eventA.GetData());
  CPPUNIT_ASSERT(eventB.GetSource()==eventA.GetSource());
}
//----------------------------------------------------------------------------
void mafEventBaseTest::TestSender()
//----------------------------------------------------------------------------
{
  mafTestObject *testSender;
  mafNEW(testSender);

  // test sender field
  mafEventBase eventA;
  CPPUNIT_ASSERT(eventA.GetSender()==NULL);
  eventA.SetSender(testSender);
  CPPUNIT_ASSERT(eventA.GetSender()==testSender);

  mafDEL(testSender);
}
//----------------------------------------------------------------------------
void mafEventBaseTest::TestSource()
//----------------------------------------------------------------------------
{
  mafEventSource testSource;
 
  // test source field
  mafEventBase eventA;
  CPPUNIT_ASSERT(eventA.GetSource()==NULL);
  eventA.SetSource(&testSource);
  CPPUNIT_ASSERT(eventA.GetSource()== &testSource);

}
//----------------------------------------------------------------------------
void mafEventBaseTest::TestId()
//----------------------------------------------------------------------------
{
  int id = 12345;

  // test id field
  mafEventBase eventA;
  CPPUNIT_ASSERT(eventA.GetId()==ID_NO_EVENT);
  eventA.SetId(id);
  CPPUNIT_ASSERT(eventA.GetId()== id);

}
//----------------------------------------------------------------------------
void mafEventBaseTest::TestChannel()
//----------------------------------------------------------------------------
{
  int channel = MCH_DOWN;

  // test channel field
  mafEventBase eventA;
  CPPUNIT_ASSERT(eventA.GetChannel()==MCH_UP);
  eventA.SetChannel(channel);
  CPPUNIT_ASSERT(eventA.GetChannel()== channel);
}
//----------------------------------------------------------------------------
void mafEventBaseTest::TestData()
//----------------------------------------------------------------------------
{
  typedef std::pair<int,const char *> TestType;
  TestType testData(10,"helloData");

  // test data field
  mafEventBase eventA;
  CPPUNIT_ASSERT(eventA.GetData()==NULL);
  eventA.SetData(&testData);
  CPPUNIT_ASSERT(eventA.GetData()== &testData);
}
//----------------------------------------------------------------------------
void mafEventBaseTest::TestSkipFlag()
//----------------------------------------------------------------------------
{
  bool skipFlag = true;

  // test skipFlag field
  mafEventBase eventA;
  CPPUNIT_ASSERT(eventA.GetSkipFlag()==false);
  eventA.SetSkipFlag(skipFlag);
  CPPUNIT_ASSERT(eventA.GetSkipFlag()== skipFlag);
}
//----------------------------------------------------------------------------
void mafEventBaseTest::TestSkipNext()
//----------------------------------------------------------------------------
{  
  bool skipFlag = true;

  // test skipnext 
  mafEventBase eventA;
  CPPUNIT_ASSERT(eventA.GetSkipFlag()==false);
  eventA.SkipNext(); //set m_SkipFlag to true
  CPPUNIT_ASSERT(eventA.GetSkipFlag()== skipFlag);
}
