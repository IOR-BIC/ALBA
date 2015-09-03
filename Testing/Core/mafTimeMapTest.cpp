/*=========================================================================

 Program: MAF2
 Module: mafTimeMapTest
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
#include "mafTimeMapTest.h"

#include "mmuTimeSet.h"
#include "mafReferenceCounted.h"
#include "mafTimeMap.h"
#include "mafSmartPointer.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(result);

/** an object with a "m_TimeStamp" member. */
class mafTestTObject: public mafReferenceCounted
{
public:
  mafTypeMacro(mafTestTObject,mafReferenceCounted);
  mafTestTObject():Flag(NULL),m_TimeStamp(-1) {}
  ~mafTestTObject() {if (Flag) *Flag=false;}
  mafTimeStamp GetTimeStamp() {return m_TimeStamp;}
  void SetTimeStamp(mafTimeStamp t) {m_TimeStamp=t;}
  void SetFlag(bool &flag) {Flag=&flag;}
  bool GetFlag() {return *Flag;}
  bool Equals(mafTestTObject *a){return false;};
  void DeepCopy(mafTestTObject *a){};
protected:
  bool *Flag;
  mafTimeStamp m_TimeStamp;
};

mafCxxTypeMacro(mafTestTObject);

/** a container specialized for "mafTestTObject" objects. */
class mafTestTVector: public mafTimeMap<mafTestTObject>
{
  mafTypeMacro(mafTestTVector,mafObject);
};

mafCxxTypeMacro(mafTestTVector);

//----------------------------------------------------------------------------
void mafTimeMapTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafTimeMapTest::TestAllocation()
//----------------------------------------------------------------------------
{
  mafTestTVector tvector;

  mafTestTVector *vec = new mafTestTVector();
  cppDEL(vec);

  mafTestTVector *vec2 = mafTestTVector::New();
  vec2->Delete();
}
//----------------------------------------------------------------------------
void mafTimeMapTest::TestNumberOfItems()
//----------------------------------------------------------------------------
{
  mafTimeStamp tarray[6]={1, 2.5, 1.2, 1.8, 3.6, 0.5};
  mafTestTVector tvector;

  // create a number of objects and sets their timestamps from the tarray.
  int i;
  for (i = 0; i < 6; i++)
  {
    mafSmartPointer<mafTestTObject> tmp;
    tmp->SetTimeStamp(tarray[i]);
    tvector.InsertItem(tmp);
  }
  result = tvector.GetNumberOfItems() == 6;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafTimeMapTest::TestOrdering()
//----------------------------------------------------------------------------
{
  mafTimeStamp tarray[6]={1, 2.5, 1.2, 1.8, 3.6, 0.5};
  mafTestTVector tvector;

  // create a number of objects and sets their timestamps from the tarray.
  int i;
  for (i = 0; i < 6; i++)
  {
    mafSmartPointer<mafTestTObject> tmp;
    tmp->SetTimeStamp(tarray[i]);
    tvector.InsertItem(tmp);
  }

  mafTimeStamp old_t;
  mafTestTVector::TimeMap::iterator it;
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
void mafTimeMapTest::TestFindByIndex()
//----------------------------------------------------------------------------
{
  mafTimeStamp tarray[6]={1, 2.5, 1.2, 1.8, 3.6, 0.5};
  mafTestTVector tvector;

  // create a number of objects and sets their timestamps from the tarray.
  int i;
  for (i = 0; i < 6; i++)
  {
    mafSmartPointer<mafTestTObject> tmp;
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
void mafTimeMapTest::TestInsertion()
//----------------------------------------------------------------------------
{
  mafTimeStamp tarray[6]={1, 2.5, 1.2, 1.8, 3.6, 0.5};
  mafTestTVector tvector;

  // create a number of objects and sets their timestamps from the tarray.
  int i;
  for (i = 0; i < 6; i++)
  {
    mafSmartPointer<mafTestTObject> tmp;
    tmp->SetTimeStamp(tarray[i]);
    tvector.InsertItem(tmp);
  }
  mafSmartPointer<mafTestTObject> t0;
  t0->SetTimeStamp(.3);

  // test insertions at begin
  tvector.PrependItem(t0); // inserted in constant time (already ordered object)
  result = tvector.FindItemIndex(.3)==0;
  TEST_RESULT;

  mafSmartPointer<mafTestTObject> t5;
  t5->SetTimeStamp(2);
  tvector.PrependItem(t5); // inserted in 2*log(N) time (worst case of non ordered object)
  result = tvector.FindItemIndex(2)==5;
  TEST_RESULT;

  // test insertion at end
  mafSmartPointer<mafTestTObject> t8;
  t8->SetTimeStamp(4);
  tvector.AppendItem(t8); // inserted in constant time (already ordered object)
  result = tvector.FindItemIndex(4)==8;
  TEST_RESULT;

  mafSmartPointer<mafTestTObject> t0b;
  t0b->SetTimeStamp(.2);
  tvector.AppendItem(t0b); // inserted in 2*log(N) time: worst case (worst case of non ordered object)
  result = tvector.FindItemIndex(.2)==0;
  TEST_RESULT;

  // check we have the correct number of time stamps
  result = tvector.GetNumberOfItems()==10;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafTimeMapTest::TestSortIntegrity()
//----------------------------------------------------------------------------
{
  mafTimeStamp tarray[6]={1, 2.5, 1.2, 1.8, 3.6, 0.5};
  mafTestTVector tvector;

  // create a number of objects and sets their timestamps from the tarray.
  int i;
  for (i = 0; i < 6; i++)
  {
    mafSmartPointer<mafTestTObject> tmp;
    tmp->SetTimeStamp(tarray[i]);
    tvector.InsertItem(tmp);
  }
  mafSmartPointer<mafTestTObject> t0;
  t0->SetTimeStamp(.3);

  tvector.PrependItem(t0);
  mafSmartPointer<mafTestTObject> t5;
  t5->SetTimeStamp(2);
  tvector.PrependItem(t5);
  mafSmartPointer<mafTestTObject> t8;
  t8->SetTimeStamp(4);
  tvector.AppendItem(t8);
  mafSmartPointer<mafTestTObject> t0b;
  t0b->SetTimeStamp(.2);
  tvector.AppendItem(t0b);

  mafTimeStamp final_tarray[10]={.2, .3, .5, 1, 1.2, 1.8, 2, 2.5, 3.6, 4};

  mafTestTVector::TimeMap::iterator it;
  for (i=0,it=tvector.Begin();it!=tvector.End();it++,i++)
  {
    result = final_tarray[i] == it->first;
    TEST_RESULT;
  }
}
//----------------------------------------------------------------------------
void mafTimeMapTest::TestFindAlgorithm()
//----------------------------------------------------------------------------
{
  mafTimeStamp final_tarray[10]={.2, .3, .5, 1, 1.2, 1.8, 2, 2.5, 3.6, 4};
  mafTestTVector tvector;

  for (int i = 0; i < 10; i++)
  {
    mafSmartPointer<mafTestTObject> tmp;
    tmp->SetTimeStamp(final_tarray[i]);
    tvector.InsertItem(tmp);
  }
  result = tvector.GetItemBefore(2.4)->GetTimeStamp() == 2.0;
  TEST_RESULT;
  result = tvector.GetNearestItem(2.4)->GetTimeStamp() == 2.5;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafTimeMapTest::TestSearchOutOfBounds()
//----------------------------------------------------------------------------
{
  mafTimeStamp final_tarray[10]={.2, .3, .5, 1, 1.2, 1.8, 2, 2.5, 3.6, 4};
  mafTestTVector tvector;

  for (int i = 0; i < 10; i++)
  {
    mafSmartPointer<mafTestTObject> tmp;
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
void mafTimeMapTest::TestItemsRemoval()
//----------------------------------------------------------------------------
{
  mafTimeStamp final_tarray[10]={.2, .3, .5, 1, 1.2, 1.8, 2, 2.5, 3.6, 4};
  mafTestTVector tvector;

  for (int i = 0; i < 10; i++)
  {
    mafSmartPointer<mafTestTObject> tmp;
    tmp->SetTimeStamp(final_tarray[i]);
    tvector.InsertItem(tmp);
  }
  mafTestTVector::TimeMap::iterator it;
  it = tvector.FindItem(2.0);
  result = it != tvector.End();
  TEST_RESULT;

  tvector.RemoveItem(it);
  result = tvector.GetNumberOfItems() == 9;
  TEST_RESULT;

  result = tvector.FindItem(2.0) == tvector.End();
  TEST_RESULT;
}
