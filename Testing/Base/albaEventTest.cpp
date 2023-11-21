/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include "albaEventTest.h"
#include "albaBaseTests.h"
#include "albaEventBase.h"
#include "albaReferenceCounted.h"
#include <iostream>
#include <utility>

using namespace std;


void albaEventTest::MafEventTestMainTest()
{  
  typedef pair<int,const char *> test_type;
  test_type test_data(10,"ciao");
  
  albaTestObject test_object;

  // test constructor
  albaEventBase event_a(&test_object,5,&test_data);
  CPPUNIT_ASSERT(event_a.GetId()==5);
  CPPUNIT_ASSERT(event_a.GetSender()==&test_object);
  CPPUNIT_ASSERT(event_a.GetData()==&test_data);

  // test copy constructor
  albaEventBase event_b=event_a;
  CPPUNIT_ASSERT(event_b.GetId()==event_a.GetId());
  CPPUNIT_ASSERT(event_b.GetSender()==event_a.GetSender());
  CPPUNIT_ASSERT(event_b.GetData()==event_a.GetData());

  // test Set functions
  albaEventBase event_c;
  event_c.SetSender(&test_object);
  event_c.SetId(5);
  event_c.SetData(&test_data);
  CPPUNIT_ASSERT(event_c.GetId()==event_a.GetId());
  CPPUNIT_ASSERT(event_c.GetSender()==event_a.GetSender());
  CPPUNIT_ASSERT(event_c.GetData()==event_a.GetData());
  
  cout<<"Test completed successfully!"<<endl;

}
