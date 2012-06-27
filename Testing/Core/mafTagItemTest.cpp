/*=========================================================================

 Program: MAF2
 Module: mafTagItemTest
 Authors: Paolo Quadrani
 
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
#include "mafTagItemTest.h"

#include "mafSmartPointer.h"
#include "mafTagItem.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafTagItemTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafTagItemTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void mafTagItemTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafTagItemTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafTagItem ti;
}
//----------------------------------------------------------------------------
void mafTagItemTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafTagItem *ti = new mafTagItem();
  cppDEL(ti);
}
//----------------------------------------------------------------------------
void mafTagItemTest::TestAllConstructor()
//----------------------------------------------------------------------------
{
  mafTagItem ti;
  ti.SetName("Test_Tag");
  ti.SetType(MAF_STRING_TAG);
  ti.SetValue("String value");

  mafTagItem *ti1 = new mafTagItem(ti);
  cppDEL(ti1);

  std::vector<double> double_values;
  double_values.push_back(1.0);
  double_values.push_back(2.0);
  mafTagItem *ti2 = new mafTagItem("Tag name", double_values, 2);
  cppDEL(ti2);

  double tag_value = 5.0;
  mafTagItem *ti3 = new mafTagItem("Tag name", &tag_value, 1);
  cppDEL(ti3);

  mafTagItem *ti4 = new mafTagItem("Tag name", 5.0);
  cppDEL(ti4);

  std::vector<mafString> string_values;
  string_values.push_back("string 1");
  string_values.push_back("string 2");
  mafTagItem *ti5 = new mafTagItem("Tag name", string_values, 2);
  cppDEL(ti5);

  const char *tag_string_value[2] = {"string 1", "string 2"};
  mafTagItem *ti6 = new mafTagItem("Tag name", tag_string_value, 2);
  cppDEL(ti6);

  mafTagItem *ti7 = new mafTagItem("Tag name", "Tag value");
  cppDEL(ti7);
}
//----------------------------------------------------------------------------
void mafTagItemTest::TestAllCopy()
//----------------------------------------------------------------------------
{
  mafTagItem ti;
  ti.SetName("Test_Tag");
  ti.SetType(MAF_STRING_TAG);
  ti.SetValue("String value");

  mafTagItem *item = new mafTagItem(ti);
  result = item->Equals(&ti);
  TEST_RESULT;
  cppDEL(item);

  mafTagItem ti2;
  ti2 = ti;
  result = ti2.Equals(&ti);
  TEST_RESULT;

  mafTagItem ti3;
  ti3.DeepCopy(&ti);
  result = ti3.Equals(&ti);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafTagItemTest::TestEquals()
//----------------------------------------------------------------------------
{
  mafTagItem ti;
  ti.SetName("Test_Tag");
  ti.SetType(MAF_STRING_TAG);
  ti.SetValue("String value");

  mafTagItem nti("TestNUM",1235.67890123456e20);
  mafTagItem ti2("Test_Tag", "String value");

  result = ti.Equals(&ti);
  TEST_RESULT;

  result = ti == ti2;
  TEST_RESULT;

  result = nti != ti;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafTagItemTest::TestTagName()
//----------------------------------------------------------------------------
{
  mafTagItem titem("TestTAG","String Value");

  result = mafString::Equals("TestTAG",titem.GetName());
  TEST_RESULT;

  titem.SetName("Tag name");
  result = mafString::Equals("Tag name",titem.GetName());
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafTagItemTest::TestTagValue()
//----------------------------------------------------------------------------
{
  mafTagItem ti;
  ti.SetName("Test_Tag");
  ti.SetType(MAF_STRING_TAG);
  ti.SetValue("String value");

  result = mafString::Equals("String value", ti.GetValue());
  TEST_RESULT;

  ti.SetValue("new value");
  result = mafString::Equals("new value", ti.GetValue());
  TEST_RESULT;

  mafTagItem nti("NumericTag",5.0);

  result = mafEquals(nti.GetValueAsDouble(),5.0);
  TEST_RESULT;

  std::vector<mafString> str_values;
  str_values.push_back("val_1");
  str_values.push_back("val_2");
  ti.SetValues(str_values);

  result = mafString::Equals("val_1", ti.GetValue(0));
  TEST_RESULT;
  result = mafString::Equals("val_2", ti.GetValue(1));
  TEST_RESULT;

  double num_values[2] = {-1.0, 3.0};
  nti.SetValues(num_values,2);
  result = mafEquals(nti.GetValueAsDouble(0),-1.0);
  TEST_RESULT;
  result = mafEquals(nti.GetValueAsDouble(1),3.0);
  TEST_RESULT;

  const char *strings[4]={"val_1","val_2","val_3","val_4"};
  mafTagItem tmulti("MultiString",strings,4);

  mafString value;
  tmulti.GetValueAsSingleString(value);
  mafString solution = "(\"val_1\",\"val_2\",\"val_3\",\"val_4\")";
  result = value == solution;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafTagItemTest::TestRemoveValue()
//----------------------------------------------------------------------------
{
  mafTagItem ti;
  ti.SetName("Test_Tag");
  ti.SetType(MAF_STRING_TAG);
  ti.SetValue("String value");

  result = mafString::Equals("String value", ti.GetValue());
  TEST_RESULT;

  ti.SetValue("new value", 1);
  result = mafString::Equals("new value", ti.GetValue(1));
  TEST_RESULT;

  result = ti.GetNumberOfComponents() == 2;
  TEST_RESULT;
   
  ti.RemoveValue(1);
  result = ti.GetValue(1) == NULL;
  TEST_RESULT;

  result = ti.GetNumberOfComponents() == 1;
  TEST_RESULT;

  result = mafString::Equals("String value", ti.GetValue(0));
  TEST_RESULT;


  mafTagItem nti("NumericTag", 5.0);

  result = mafEquals(nti.GetValueAsDouble(),5.0);
  TEST_RESULT;

 
  

  
}
//----------------------------------------------------------------------------
void mafTagItemTest::TestTagComponent()
//----------------------------------------------------------------------------
{
  mafTagItem nti("NumericTag",5.0);
  result = nti.GetNumberOfComponents() == 1;
  TEST_RESULT;

  double num_values[2] = {-1.0, 3.0};
  nti.SetComponents(num_values,2);

  result = nti.GetNumberOfComponents() == 2;
  TEST_RESULT;

  nti.SetComponent(5.0, 0);
  result = mafEquals(nti.GetComponentAsDouble(0),5.0);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafTagItemTest::TestTagType()
//----------------------------------------------------------------------------
{
  mafTagItem ti("NumericTag",5.0);

  result = ti.GetType() == MAF_NUMERIC_TAG;
  TEST_RESULT;

  mafString value;
  ti.GetTypeAsString(value);
  result = value.Equals("NUM");
  TEST_RESULT;
}
