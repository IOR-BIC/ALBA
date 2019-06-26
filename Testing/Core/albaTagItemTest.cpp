/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTagItemTest
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
#include "albaTagItemTest.h"

#include "albaSmartPointer.h"
#include "albaTagItem.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaTagItemTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaTagItemTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaTagItem ti;
}
//----------------------------------------------------------------------------
void albaTagItemTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaTagItem *ti = new albaTagItem();
  cppDEL(ti);
}
//----------------------------------------------------------------------------
void albaTagItemTest::TestAllConstructor()
//----------------------------------------------------------------------------
{
  albaTagItem ti;
  ti.SetName("Test_Tag");
  ti.SetType(ALBA_STRING_TAG);
  ti.SetValue("String value");

  albaTagItem *ti1 = new albaTagItem(ti);
  cppDEL(ti1);

  std::vector<double> double_values;
  double_values.push_back(1.0);
  double_values.push_back(2.0);
  albaTagItem *ti2 = new albaTagItem("Tag name", double_values, 2);
  cppDEL(ti2);

  double tag_value = 5.0;
  albaTagItem *ti3 = new albaTagItem("Tag name", &tag_value, 1);
  cppDEL(ti3);

  albaTagItem *ti4 = new albaTagItem("Tag name", 5.0);
  cppDEL(ti4);

  std::vector<albaString> string_values;
  string_values.push_back("string 1");
  string_values.push_back("string 2");
  albaTagItem *ti5 = new albaTagItem("Tag name", string_values, 2);
  cppDEL(ti5);

  const char *tag_string_value[2] = {"string 1", "string 2"};
  albaTagItem *ti6 = new albaTagItem("Tag name", tag_string_value, 2);
  cppDEL(ti6);

  albaTagItem *ti7 = new albaTagItem("Tag name", "Tag value");
  cppDEL(ti7);
}
//----------------------------------------------------------------------------
void albaTagItemTest::TestAllCopy()
//----------------------------------------------------------------------------
{
  albaTagItem ti;
  ti.SetName("Test_Tag");
  ti.SetType(ALBA_STRING_TAG);
  ti.SetValue("String value");

  albaTagItem *item = new albaTagItem(ti);
  result = item->Equals(&ti);
  TEST_RESULT;
  cppDEL(item);

  albaTagItem ti2;
  ti2 = ti;
  result = ti2.Equals(&ti);
  TEST_RESULT;

  albaTagItem ti3;
  ti3.DeepCopy(&ti);
  result = ti3.Equals(&ti);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTagItemTest::TestEquals()
//----------------------------------------------------------------------------
{
  albaTagItem ti;
  ti.SetName("Test_Tag");
  ti.SetType(ALBA_STRING_TAG);
  ti.SetValue("String value");

  albaTagItem nti("TestNUM",1235.67890123456e20);
  albaTagItem ti2("Test_Tag", "String value");

  result = ti.Equals(&ti);
  TEST_RESULT;

  result = ti == ti2;
  TEST_RESULT;

  result = nti != ti;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTagItemTest::TestTagName()
//----------------------------------------------------------------------------
{
  albaTagItem titem("TestTAG","String Value");

  result = albaString::Equals("TestTAG",titem.GetName());
  TEST_RESULT;

  titem.SetName("Tag name");
  result = albaString::Equals("Tag name",titem.GetName());
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTagItemTest::TestTagValue()
//----------------------------------------------------------------------------
{
  albaTagItem ti;
  ti.SetName("Test_Tag");
  ti.SetType(ALBA_STRING_TAG);
  ti.SetValue("String value");

  result = albaString::Equals("String value", ti.GetValue());
  TEST_RESULT;

  ti.SetValue("new value");
  result = albaString::Equals("new value", ti.GetValue());
  TEST_RESULT;

  albaTagItem nti("NumericTag",5.0);

  result = albaEquals(nti.GetValueAsDouble(),5.0);
  TEST_RESULT;

  std::vector<albaString> str_values;
  str_values.push_back("val_1");
  str_values.push_back("val_2");
  ti.SetValues(str_values);

  result = albaString::Equals("val_1", ti.GetValue(0));
  TEST_RESULT;
  result = albaString::Equals("val_2", ti.GetValue(1));
  TEST_RESULT;

  double num_values[2] = {-1.0, 3.0};
  nti.SetValues(num_values,2);
  result = albaEquals(nti.GetValueAsDouble(0),-1.0);
  TEST_RESULT;
  result = albaEquals(nti.GetValueAsDouble(1),3.0);
  TEST_RESULT;

  const char *strings[4]={"val_1","val_2","val_3","val_4"};
  albaTagItem tmulti("MultiString",strings,4);

  albaString value;
  tmulti.GetValueAsSingleString(value);
  albaString solution = "(\"val_1\",\"val_2\",\"val_3\",\"val_4\")";
  result = value == solution;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTagItemTest::TestRemoveValue()
//----------------------------------------------------------------------------
{
  albaTagItem ti;
  ti.SetName("Test_Tag");
  ti.SetType(ALBA_STRING_TAG);
  ti.SetValue("String value");

  result = albaString::Equals("String value", ti.GetValue());
  TEST_RESULT;

  ti.SetValue("new value", 1);
  result = albaString::Equals("new value", ti.GetValue(1));
  TEST_RESULT;

  result = ti.GetNumberOfComponents() == 2;
  TEST_RESULT;
   
  ti.RemoveValue(1);
  result = ti.GetValue(1) == NULL;
  TEST_RESULT;

  result = ti.GetNumberOfComponents() == 1;
  TEST_RESULT;

  result = albaString::Equals("String value", ti.GetValue(0));
  TEST_RESULT;


  albaTagItem nti("NumericTag", 5.0);

  result = albaEquals(nti.GetValueAsDouble(),5.0);
  TEST_RESULT;

 
  

  
}
//----------------------------------------------------------------------------
void albaTagItemTest::TestTagComponent()
//----------------------------------------------------------------------------
{
  albaTagItem nti("NumericTag",5.0);
  result = nti.GetNumberOfComponents() == 1;
  TEST_RESULT;

  double num_values[2] = {-1.0, 3.0};
  nti.SetComponents(num_values,2);

  result = nti.GetNumberOfComponents() == 2;
  TEST_RESULT;

  nti.SetComponent(5.0, 0);
  result = albaEquals(nti.GetComponentAsDouble(0),5.0);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTagItemTest::TestTagType()
//----------------------------------------------------------------------------
{
  albaTagItem ti("NumericTag",5.0);

  result = ti.GetType() == ALBA_NUMERIC_TAG;
  TEST_RESULT;

  albaString value;
  ti.GetTypeAsString(value);
  result = value.Equals("NUM");
  TEST_RESULT;
}
