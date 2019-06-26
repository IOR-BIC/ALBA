/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTagArrayTest
 Authors: Paolo Quadrani
 
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
#include "albaTagArrayTest.h"

#include "albaSmartPointer.h"
#include "albaTagArray.h"
#include "albaTagItem.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaTagArrayTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaTagArrayTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaTagArray ta;
}
//----------------------------------------------------------------------------
void albaTagArrayTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaTagArray *ta = new albaTagArray();
  cppDEL(ta);
}
//----------------------------------------------------------------------------
void albaTagArrayTest::TestAllConstructor()
//----------------------------------------------------------------------------
{
  albaTagArray *ta = NULL;
  ta = albaTagArray::New();

  result = ta->GetReferenceCount() == 0;
  TEST_RESULT;
  albaDEL(ta);

  albaNEW(ta);
  result = ta->GetReferenceCount() == 1;
  TEST_RESULT;
  albaDEL(ta);

  albaSmartPointer<albaTagArray> smart_ta;
  result = smart_ta->GetReferenceCount() == 1;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTagArrayTest::TestAllCopy()
//----------------------------------------------------------------------------
{
  albaTagItem item("TestTAG","String Value");
  albaTagItem tnum("TestNUM",1235.67890123456e20);

  albaSmartPointer<albaTagArray> ta;
  ta->SetTag(item);
  albaSmartPointer<albaTagArray> ta2;
  
  ta2->DeepCopy(ta);
  result = ta2->Equals(ta);
  TEST_RESULT;

  albaTagArray *ta3;
  ta3 = ta;
  result = ta3->Equals(ta);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTagArrayTest::TestGetTag()
//----------------------------------------------------------------------------
{
  albaTagItem item("TestTAG","String Value");
  albaTagItem tnum("TestNUM",1235.67890123456e20);

  albaSmartPointer<albaTagArray> ta;
  ta->SetTag(item);

  result = *ta->GetTag("TestTAG") == item;
  TEST_RESULT;

  albaTagItem item2;
  ta->GetTag("TestTAG", item2);
  result = item2 == item;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTagArrayTest::TestSetTag()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaTagArray> ta;
  ta->SetTag("pippo","pluto");

  std::string stringValue;

  ta->GetTag("pippo")->GetValueAsSingleString(stringValue);
  CPPUNIT_ASSERT(stringValue == "(\"pluto\")");
}

//----------------------------------------------------------------------------
void albaTagArrayTest::TestIsTagPresent()
//----------------------------------------------------------------------------
{
  albaTagItem item("TestTAG","String Value");
  albaTagItem tnum("TestNUM",1235.67890123456e20);

  albaSmartPointer<albaTagArray> ta;
  ta->SetTag(item);

  result = ta->IsTagPresent("TestTAG");
  TEST_RESULT;

  result = !ta->IsTagPresent("TestNUM");
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTagArrayTest::TestDeleteTag()
//----------------------------------------------------------------------------
{
  albaTagItem item("TestTAG","String Value");
  albaTagItem tnum("TestNUM",1235.67890123456e20);

  albaSmartPointer<albaTagArray> ta;
  ta->SetTag(item);

  ta->DeleteTag("TestTAG");
  result = !ta->IsTagPresent("TestTAG");
  TEST_RESULT;

  ta->DeleteTag("TestNUM"); // Tag not present -> nothing happen.
}
//----------------------------------------------------------------------------
void albaTagArrayTest::TestGetNumberOfTags()
//----------------------------------------------------------------------------
{
  albaTagItem item("TestTAG","String Value");
  albaTagItem tnum("TestNUM",1235.67890123456e20);

  albaSmartPointer<albaTagArray> ta;
  ta->SetTag(item);

  result = ta->GetNumberOfTags() == 1;
  TEST_RESULT;

  ta->DeleteTag("TestTAG");
  result = ta->GetNumberOfTags() == 0;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTagArrayTest::TestGetTagsByType()
//----------------------------------------------------------------------------
{
  albaTagItem item("TestTAG","String Value");
  albaTagItem tnum("TestNUM",1235.67890123456e20);

  albaSmartPointer<albaTagArray> ta;
  ta->SetTag(item);
  ta->SetTag(tnum);

  std::vector<albaTagItem *> tag_list;
  ta->GetTagsByType(ALBA_NUMERIC_TAG, tag_list);

  result = tag_list.size() == 1;
  TEST_RESULT;

  result = *tag_list[0] == tnum;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTagArrayTest::TestGetTagList()
//----------------------------------------------------------------------------
{
  albaTagItem item("TestTAG","String Value");
  albaTagItem tnum("TestNUM",1235.67890123456e20);

  albaSmartPointer<albaTagArray> ta;
  ta->SetTag(item);
  ta->SetTag(tnum);

  std::vector<std::string> tag_name_list;
  ta->GetTagList(tag_name_list);

  result = tag_name_list.size() == 2;
  TEST_RESULT;
}
