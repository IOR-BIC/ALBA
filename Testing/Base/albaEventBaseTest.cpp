/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventBaseTest
 Authors: Daniele Giunchi, Crimi Gianluigi
 
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
#include "albaBaseTests.h"
#include "albaEventBaseTest.h"

#include "albaReferenceCounted.h"
#include "albaEventBase.h"

#include <iostream>
#include <utility>


//----------------------------------------------------------------------------
void albaEventBaseTest::TestFixture()
{
}

//----------------------------------------------------------------------------
void albaEventBaseTest::TestStaticAllocation()
{
  albaEventBase m;
}
//----------------------------------------------------------------------------
void albaEventBaseTest::TestDynamicAllocation()
{
  albaEventBase *m = new albaEventBase();
  cppDEL(m);
}
//----------------------------------------------------------------------------
void albaEventBaseTest::TestAllConstructors()
{
  typedef std::pair<int,const char *> TestType;
  TestType testData(10,"hello");

  albaTestObject testObject;

  // test constructor
  albaEventBase eventA(&testObject,5,&testData);
  CPPUNIT_ASSERT(eventA.GetId()==5);
  CPPUNIT_ASSERT(eventA.GetSender()==&testObject);
  CPPUNIT_ASSERT(eventA.GetData()==&testData);


  // test copy constructor
  albaEventBase eventB=eventA;
  CPPUNIT_ASSERT(eventB.GetId()==eventA.GetId());
  CPPUNIT_ASSERT(eventB.GetSender()==eventA.GetSender());
  CPPUNIT_ASSERT(eventB.GetData()==eventA.GetData());
}
//----------------------------------------------------------------------------
void albaEventBaseTest::TestCopy()
{
  typedef std::pair<int,const char *> TestType;
  TestType testData(10,"helloCopy");

  albaTestObject testObject;

  albaEventBase eventA(&testObject,5,&testData);

  // test deepcopy
  albaEventBase eventB;
  eventB.DeepCopy(&eventA);
  CPPUNIT_ASSERT(eventB.GetId()==eventA.GetId());
  CPPUNIT_ASSERT(eventB.GetSender()==eventA.GetSender());
  CPPUNIT_ASSERT(eventB.GetData()==eventA.GetData());
}
//----------------------------------------------------------------------------
void albaEventBaseTest::TestSender()
{
  albaTestObject *testSender;
  albaNEW(testSender);

  // test sender field
  albaEventBase eventA;
  CPPUNIT_ASSERT(eventA.GetSender()==NULL);
  eventA.SetSender(testSender);
  CPPUNIT_ASSERT(eventA.GetSender()==testSender);

  albaDEL(testSender);
}

//----------------------------------------------------------------------------
void albaEventBaseTest::TestId()
{
  int id = 12345;

  // test id field
  albaEventBase eventA;
  CPPUNIT_ASSERT(eventA.GetId()==ID_NO_EVENT);
  eventA.SetId(id);
  CPPUNIT_ASSERT(eventA.GetId()== id);

}
//----------------------------------------------------------------------------
void albaEventBaseTest::TestChannel()
{
  int channel = MCH_DOWN;

  // test channel field
  albaEventBase eventA;
  CPPUNIT_ASSERT(eventA.GetChannel()==MCH_UP);
  eventA.SetChannel(channel);
  CPPUNIT_ASSERT(eventA.GetChannel()== channel);
}
//----------------------------------------------------------------------------
void albaEventBaseTest::TestData()
{
  typedef std::pair<int,const char *> TestType;
  TestType testData(10,"helloData");

  // test data field
  albaEventBase eventA;
  CPPUNIT_ASSERT(eventA.GetData()==NULL);
  eventA.SetData(&testData);
  CPPUNIT_ASSERT(eventA.GetData()== &testData);
}

