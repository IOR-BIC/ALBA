/*=========================================================================

 Program: MAF2
 Module: mafEventTest
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "mafEventTest.h"
#include "mafBaseTests.h"
#include "mafEventBase.h"
#include "mafReferenceCounted.h"
#include <iostream>
#include <utility>

using namespace std;


void mafEventTest::MafEventTestMainTest()
{  
  typedef pair<int,const char *> test_type;
  test_type test_data(10,"ciao");
  
  mafTestObject test_object;

  // test constructor
  mafEventBase event_a(&test_object,5,&test_data);
  CPPUNIT_ASSERT(event_a.GetId()==5);
  CPPUNIT_ASSERT(event_a.GetSender()==&test_object);
  CPPUNIT_ASSERT(event_a.GetData()==&test_data);

  // test copy constructor
  mafEventBase event_b=event_a;
  CPPUNIT_ASSERT(event_b.GetId()==event_a.GetId());
  CPPUNIT_ASSERT(event_b.GetSender()==event_a.GetSender());
  CPPUNIT_ASSERT(event_b.GetData()==event_a.GetData());

  // test Set functions
  mafEventBase event_c;
  event_c.SetSender(&test_object);
  event_c.SetId(5);
  event_c.SetData(&test_data);
  CPPUNIT_ASSERT(event_c.GetId()==event_a.GetId());
  CPPUNIT_ASSERT(event_c.GetSender()==event_a.GetSender());
  CPPUNIT_ASSERT(event_c.GetData()==event_a.GetData());
  
  cout<<"Test completed successfully!"<<endl;

}
