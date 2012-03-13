/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafTagArrayTest.cpp,v $
Language:  C++
Date:      $Date: 2006-12-05 11:45:26 $
Version:   $Revision: 1.2 $
Authors:   Paolo Quadrani
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
#include "mafTagArrayTest.h"

#include "mafSmartPointer.h"
#include "mafTagArray.h"
#include "mafTagItem.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafTagArrayTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafTagArrayTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void mafTagArrayTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafTagArrayTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafTagArray ta;
}
//----------------------------------------------------------------------------
void mafTagArrayTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafTagArray *ta = new mafTagArray();
  cppDEL(ta);
}
//----------------------------------------------------------------------------
void mafTagArrayTest::TestAllConstructor()
//----------------------------------------------------------------------------
{
  mafTagArray *ta = NULL;
  ta = mafTagArray::New();

  result = ta->GetReferenceCount() == 0;
  TEST_RESULT;
  mafDEL(ta);

  mafNEW(ta);
  result = ta->GetReferenceCount() == 1;
  TEST_RESULT;
  mafDEL(ta);

  mafSmartPointer<mafTagArray> smart_ta;
  result = smart_ta->GetReferenceCount() == 1;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafTagArrayTest::TestAllCopy()
//----------------------------------------------------------------------------
{
  mafTagItem item("TestTAG","String Value");
  mafTagItem tnum("TestNUM",1235.67890123456e20);

  mafSmartPointer<mafTagArray> ta;
  ta->SetTag(item);
  mafSmartPointer<mafTagArray> ta2;
  
  ta2->DeepCopy(ta);
  result = ta2->Equals(ta);
  TEST_RESULT;

  mafTagArray *ta3;
  ta3 = ta;
  result = ta3->Equals(ta);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafTagArrayTest::TestGetTag()
//----------------------------------------------------------------------------
{
  mafTagItem item("TestTAG","String Value");
  mafTagItem tnum("TestNUM",1235.67890123456e20);

  mafSmartPointer<mafTagArray> ta;
  ta->SetTag(item);

  result = *ta->GetTag("TestTAG") == item;
  TEST_RESULT;

  mafTagItem item2;
  ta->GetTag("TestTAG", item2);
  result = item2 == item;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafTagArrayTest::TestSetTag()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafTagArray> ta;
  ta->SetTag("pippo","pluto");

  std::string stringValue;

  ta->GetTag("pippo")->GetValueAsSingleString(stringValue);
  CPPUNIT_ASSERT(stringValue == "(\"pluto\")");
}

//----------------------------------------------------------------------------
void mafTagArrayTest::TestIsTagPresent()
//----------------------------------------------------------------------------
{
  mafTagItem item("TestTAG","String Value");
  mafTagItem tnum("TestNUM",1235.67890123456e20);

  mafSmartPointer<mafTagArray> ta;
  ta->SetTag(item);

  result = ta->IsTagPresent("TestTAG");
  TEST_RESULT;

  result = !ta->IsTagPresent("TestNUM");
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafTagArrayTest::TestDeleteTag()
//----------------------------------------------------------------------------
{
  mafTagItem item("TestTAG","String Value");
  mafTagItem tnum("TestNUM",1235.67890123456e20);

  mafSmartPointer<mafTagArray> ta;
  ta->SetTag(item);

  ta->DeleteTag("TestTAG");
  result = !ta->IsTagPresent("TestTAG");
  TEST_RESULT;

  ta->DeleteTag("TestNUM"); // Tag not present -> nothing happen.
}
//----------------------------------------------------------------------------
void mafTagArrayTest::TestGetNumberOfTags()
//----------------------------------------------------------------------------
{
  mafTagItem item("TestTAG","String Value");
  mafTagItem tnum("TestNUM",1235.67890123456e20);

  mafSmartPointer<mafTagArray> ta;
  ta->SetTag(item);

  result = ta->GetNumberOfTags() == 1;
  TEST_RESULT;

  ta->DeleteTag("TestTAG");
  result = ta->GetNumberOfTags() == 0;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafTagArrayTest::TestGetTagsByType()
//----------------------------------------------------------------------------
{
  mafTagItem item("TestTAG","String Value");
  mafTagItem tnum("TestNUM",1235.67890123456e20);

  mafSmartPointer<mafTagArray> ta;
  ta->SetTag(item);
  ta->SetTag(tnum);

  std::vector<mafTagItem *> tag_list;
  ta->GetTagsByType(MAF_NUMERIC_TAG, tag_list);

  result = tag_list.size() == 1;
  TEST_RESULT;

  result = *tag_list[0] == tnum;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafTagArrayTest::TestGetTagList()
//----------------------------------------------------------------------------
{
  mafTagItem item("TestTAG","String Value");
  mafTagItem tnum("TestNUM",1235.67890123456e20);

  mafSmartPointer<mafTagArray> ta;
  ta->SetTag(item);
  ta->SetTag(tnum);

  std::vector<std::string> tag_name_list;
  ta->GetTagList(tag_name_list);

  result = tag_name_list.size() == 2;
  TEST_RESULT;
}
