/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTimeMapTest
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
#include "albaTimeMapTest.h"

#include "mmuTimeSet.h"
#include "albaReferenceCounted.h"
#include "albaTimeMap.h"
#include "albaSmartPointer.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(result);

/** an object with a "m_TimeStamp" member. */
class albaTestTObject: public albaReferenceCounted
{
public:
  albaTypeMacro(albaTestTObject,albaReferenceCounted);
  albaTestTObject():Flag(NULL),m_TimeStamp(-1) {}
  ~albaTestTObject() {if (Flag) *Flag=false;}
  albaTimeStamp GetTimeStamp() {return m_TimeStamp;}
  void SetTimeStamp(albaTimeStamp t) {m_TimeStamp=t;}
  void SetFlag(bool &flag) {Flag=&flag;}
  bool GetFlag() {return *Flag;}
  bool Equals(albaTestTObject *a){return false;};
  void DeepCopy(albaTestTObject *a){};
protected:
  bool *Flag;
  albaTimeStamp m_TimeStamp;
};

albaCxxTypeMacro(albaTestTObject);

/** a container specialized for "albaTestTObject" objects. */
class albaTestTVector: public albaTimeMap<albaTestTObject>
{
  albaTypeMacro(albaTestTVector,albaObject);
};

albaCxxTypeMacro(albaTestTVector);

//----------------------------------------------------------------------------
void albaTimeMapTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaTimeMapTest::TestAllocation()
//----------------------------------------------------------------------------
{
  albaTestTVector tvector;

  albaTestTVector *vec = new albaTestTVector();
  cppDEL(vec);

  albaTestTVector *vec2 = albaTestTVector::New();
  vec2->Delete();
}
//----------------------------------------------------------------------------
void albaTimeMapTest::TestNumberOfItems()
//----------------------------------------------------------------------------
{
  albaTimeStamp tarray[6]={1, 2.5, 1.2, 1.8, 3.6, 0.5};
  albaTestTVector tvector;

  // create a number of objects and sets their timestamps from the tarray.
  int i;
  for (i = 0; i < 6; i++)
  {
    albaSmartPointer<albaTestTObject> tmp;
    tmp->SetTimeStamp(tarray[i]);
    tvector.InsertItem(tmp);
  }
  result = tvector.GetNumberOfItems() == 6;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTimeMapTest::TestOrdering()
//----------------------------------------------------------------------------
{
  albaTimeStamp tarray[6]={1, 2.5, 1.2, 1.8, 3.6, 0.5};
  albaTestTVector tvector;

  // create a number of objects and sets their timestamps from the tarray.
  int i;
  for (i = 0; i < 6; i++)
  {
    albaSmartPointer<albaTestTObject> tmp;
    tmp->SetTimeStamp(tarray[i]);
    tvector.InsertItem(tmp);
  }

  albaTimeStamp old_t;
  albaTestTVector::TimeMap::iterator it;
  for (old_t = 0, it = tvector.Begin(); it != tvector.End(); it++)
  {
    result = it->second->GetTimeStamp() == it->first;
    TEST_RESULT;
    result = old_t < it->second->GetTimeStamp();
    TEST_RESULT;

    old_t = it->first;
    result = it->second->GetReferenceCount() == 1;
    TEST_RESULT;
  }
}
//----------------------------------------------------------------------------
void albaTimeMapTest::TestFindByIndex()
//----------------------------------------------------------------------------
{
  albaTimeStamp tarray[6]={1, 2.5, 1.2, 1.8, 3.6, 0.5};
  albaTestTVector tvector;

  // create a number of objects and sets their timestamps from the tarray.
  int i;
  for (i = 0; i < 6; i++)
  {
    albaSmartPointer<albaTestTObject> tmp;
    tmp->SetTimeStamp(tarray[i]);
    tvector.InsertItem(tmp);
  }
  result = tvector.FindItemIndex(1.2)==2;
  TEST_RESULT;
  result = tvector.FindItemIndex(.5)==0;
  TEST_RESULT;
  result = tvector.FindItemIndex(3.6)==5;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTimeMapTest::TestInsertion()
//----------------------------------------------------------------------------
{
  albaTimeStamp tarray[6]={1, 2.5, 1.2, 1.8, 3.6, 0.5};
  albaTestTVector tvector;

  // create a number of objects and sets their timestamps from the tarray.
  int i;
  for (i = 0; i < 6; i++)
  {
    albaSmartPointer<albaTestTObject> tmp;
    tmp->SetTimeStamp(tarray[i]);
    tvector.InsertItem(tmp);
  }
  albaSmartPointer<albaTestTObject> t0;
  t0->SetTimeStamp(.3);

  // test insertions at begin
  tvector.PrependItem(t0); // inserted in constant time (already ordered object)
  result = tvector.FindItemIndex(.3)==0;
  TEST_RESULT;

  albaSmartPointer<albaTestTObject> t5;
  t5->SetTimeStamp(2);
  tvector.PrependItem(t5); // inserted in 2*log(N) time (worst case of non ordered object)
  result = tvector.FindItemIndex(2)==5;
  TEST_RESULT;

  // test insertion at end
  albaSmartPointer<albaTestTObject> t8;
  t8->SetTimeStamp(4);
  tvector.AppendItem(t8); // inserted in constant time (already ordered object)
  result = tvector.FindItemIndex(4)==8;
  TEST_RESULT;

  albaSmartPointer<albaTestTObject> t0b;
  t0b->SetTimeStamp(.2);
  tvector.AppendItem(t0b); // inserted in 2*log(N) time: worst case (worst case of non ordered object)
  result = tvector.FindItemIndex(.2)==0;
  TEST_RESULT;

  // check we have the correct number of time stamps
  result = tvector.GetNumberOfItems()==10;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTimeMapTest::TestSortIntegrity()
//----------------------------------------------------------------------------
{
  albaTimeStamp tarray[6]={1, 2.5, 1.2, 1.8, 3.6, 0.5};
  albaTestTVector tvector;

  // create a number of objects and sets their timestamps from the tarray.
  int i;
  for (i = 0; i < 6; i++)
  {
    albaSmartPointer<albaTestTObject> tmp;
    tmp->SetTimeStamp(tarray[i]);
    tvector.InsertItem(tmp);
  }
  albaSmartPointer<albaTestTObject> t0;
  t0->SetTimeStamp(.3);

  tvector.PrependItem(t0);
  albaSmartPointer<albaTestTObject> t5;
  t5->SetTimeStamp(2);
  tvector.PrependItem(t5);
  albaSmartPointer<albaTestTObject> t8;
  t8->SetTimeStamp(4);
  tvector.AppendItem(t8);
  albaSmartPointer<albaTestTObject> t0b;
  t0b->SetTimeStamp(.2);
  tvector.AppendItem(t0b);

  albaTimeStamp final_tarray[10]={.2, .3, .5, 1, 1.2, 1.8, 2, 2.5, 3.6, 4};

  albaTestTVector::TimeMap::iterator it;
  for (i=0,it=tvector.Begin();it!=tvector.End();it++,i++)
  {
    result = final_tarray[i] == it->first;
    TEST_RESULT;
  }
}
//----------------------------------------------------------------------------
void albaTimeMapTest::TestFindAlgorithm()
//----------------------------------------------------------------------------
{
  albaTimeStamp final_tarray[10]={.2, .3, .5, 1, 1.2, 1.8, 2, 2.5, 3.6, 4};
  albaTestTVector tvector;

  for (int i = 0; i < 10; i++)
  {
    albaSmartPointer<albaTestTObject> tmp;
    tmp->SetTimeStamp(final_tarray[i]);
    tvector.InsertItem(tmp);
  }
  result = tvector.GetItemBefore(2.4)->GetTimeStamp() == 2.0;
  TEST_RESULT;
  result = tvector.GetNearestItem(2.4)->GetTimeStamp() == 2.5;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTimeMapTest::TestSearchOutOfBounds()
//----------------------------------------------------------------------------
{
  albaTimeStamp final_tarray[10]={.2, .3, .5, 1, 1.2, 1.8, 2, 2.5, 3.6, 4};
  albaTestTVector tvector;

  for (int i = 0; i < 10; i++)
  {
    albaSmartPointer<albaTestTObject> tmp;
    tmp->SetTimeStamp(final_tarray[i]);
    tvector.InsertItem(tmp);
  }
  result = tvector.GetItemBefore(.1)->GetTimeStamp() == .2;
  TEST_RESULT;
  result = tvector.GetNearestItem(.1)->GetTimeStamp() == .2;
  TEST_RESULT;

  result = tvector.GetItemBefore(5)->GetTimeStamp() == 4;
  TEST_RESULT;
  result = tvector.GetNearestItem(5)->GetTimeStamp() == 4;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTimeMapTest::TestItemsRemoval()
//----------------------------------------------------------------------------
{
  albaTimeStamp final_tarray[10]={.2, .3, .5, 1, 1.2, 1.8, 2, 2.5, 3.6, 4};
  albaTestTVector tvector;

  for (int i = 0; i < 10; i++)
  {
    albaSmartPointer<albaTestTObject> tmp;
    tmp->SetTimeStamp(final_tarray[i]);
    tvector.InsertItem(tmp);
  }
  albaTestTVector::TimeMap::iterator it;
  it = tvector.FindItem(2.0);
  result = it != tvector.End();
  TEST_RESULT;

  tvector.RemoveItem(it);
  result = tvector.GetNumberOfItems() == 9;
  TEST_RESULT;

  result = tvector.FindItem(2.0) == tvector.End();
  TEST_RESULT;
}
