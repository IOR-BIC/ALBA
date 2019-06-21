/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTimeMapScalarTest
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
#include "albaTimeMapScalarTest.h"

#include "mmuTimeSet.h"
#include "albaTimeMapScalar.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(result);

/** a container specialized for double scalars. */
class albaTestSVector: public albaTimeMapScalar<double>
{
  albaTypeMacro(albaTestSVector, albaObject);
};

albaCxxTypeMacro(albaTestSVector);

//----------------------------------------------------------------------------
void albaTimeMapScalarTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaTimeMapScalarTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_SVector = new albaTestSVector();
}
//----------------------------------------------------------------------------
void albaTimeMapScalarTest::AfterTest()
//----------------------------------------------------------------------------
{
  cppDEL(m_SVector);
}
//----------------------------------------------------------------------------
void albaTimeMapScalarTest::TestAllocation()
//----------------------------------------------------------------------------
{
  albaTestSVector tvector;

  albaTestSVector *vec = new albaTestSVector();
  cppDEL(vec);

  albaTestSVector *vec2 = albaTestSVector::New();
  vec2->Delete();
}
//----------------------------------------------------------------------------
void albaTimeMapScalarTest::TestNumberOfItems()
//----------------------------------------------------------------------------
{
  albaTimeStamp tarray[6] = {1, 2.5, 1.2, 1.8, 3.6, 0.5};
  double sarray[6] = {10.0, 20.5, 10.2, 10.8, 30.6, 10.5};

  FillScalarVector(tarray, sarray, 6);

  result = m_SVector->GetNumberOfItems() == 6;

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTimeMapScalarTest::TestOrdering()
//----------------------------------------------------------------------------
{
  albaTimeStamp tarray[6]={1, 2.5, 1.2, 1.8, 3.6, 0.5};
  double sarray[6] = {10.0, 20.5, 10.2, 10.8, 30.6, 10.5};
  double sarray_sorted_by_timestamp[6] = {10.5, 10.0, 10.2, 10.8, 20.5, 30.6};

  FillScalarVector(tarray, sarray, 6);

  int s;
  double scalar;
  albaTestSVector::TimeMapScalars::iterator it;
  for (it = m_SVector->BeginScalarVector(), s = 0; it != m_SVector->EndScalarVector(); it++, s++)
  {
    scalar = it->second;
    result = albaEquals(scalar, sarray_sorted_by_timestamp[s]);
    TEST_RESULT;
  }
}
//----------------------------------------------------------------------------
void albaTimeMapScalarTest::TestFindByIndex()
//----------------------------------------------------------------------------
{
  albaTimeStamp tarray[6]={1, 2.5, 1.2, 1.8, 3.6, 0.5};
  double sarray[6] = {10.0, 20.5, 10.2, 10.8, 30.6, 10.5};

  FillScalarVector(tarray, sarray, 6);

  result = m_SVector->FindItemIndex(1.2) == 2;
  TEST_RESULT;
  result = m_SVector->FindItemIndex(.5) == 0;
  TEST_RESULT;
  result = m_SVector->FindItemIndex(3.6) == 5;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTimeMapScalarTest::TestInsertion()
//----------------------------------------------------------------------------
{
  albaTimeStamp tarray[6]={1, 2.5, 1.2, 1.8, 3.6, 0.5};
  double sarray[6] = {10.0, 20.5, 10.2, 10.8, 30.6, 10.5};

  // create a number of objects and sets their timestamps from the tarray.
  FillScalarVector(tarray, sarray, 6);

  // test insertions at begin
  m_SVector->PrependItem(.3, 10.3); // inserted in constant time (already ordered object)
  result = m_SVector->FindItemIndex(.3) == 0;
  TEST_RESULT;

  m_SVector->PrependItem(2, 20.0); // inserted in 2*log(N) time (worst case of non ordered object)
  result = m_SVector->FindItemIndex(2) == 5;
  TEST_RESULT;

  // test insertion at end
  m_SVector->AppendItem(4,40.0); // inserted in constant time (already ordered object)
  result = m_SVector->FindItemIndex(4) == 8;
  TEST_RESULT;

  m_SVector->AppendItem(.2, 10.2); // inserted in 2*log(N) time: worst case (worst case of non ordered object)
  result = m_SVector->FindItemIndex(.2) == 0;
  TEST_RESULT;

  // check we have the correct number of time stamps
  result = m_SVector->GetNumberOfItems() == 10;
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTimeMapScalarTest::TestSortIntegrity()
//----------------------------------------------------------------------------
{
  albaTimeStamp tarray[6]={1, 2.5, 1.2, 1.8, 3.6, 0.5};
  double sarray[6] = {10.0, 20.5, 10.2, 10.8, 30.6, 10.5};

  // create a number of objects and sets their timestamps from the tarray.
  FillScalarVector(tarray, sarray, 6);

  m_SVector->PrependItem(.3, 1.3);
  m_SVector->PrependItem(2,12.0);
  m_SVector->AppendItem(4,14.0);
  m_SVector->AppendItem(.2,1.2);

  albaTimeStamp final_tarray[10]={.2, .3, .5, 1, 1.2, 1.8, 2, 2.5, 3.6, 4};

  int i;
  albaTestSVector::TimeMapScalars::iterator it;
  for (i=0,it=m_SVector->BeginScalarVector(); it != m_SVector->EndScalarVector();it++,i++)
  {
    result = final_tarray[i] == it->first;
    TEST_RESULT;
  }
}
//----------------------------------------------------------------------------
void albaTimeMapScalarTest::TestFindAlgorithm()
//----------------------------------------------------------------------------
{
  albaTimeStamp final_tarray[10]={.2, .3, .5, 1, 1.2, 1.8, 2, 2.5, 3.6, 4};
  double final_sarray[10]={1.2, 1.3, 1.5, 10, 10.2, 10.8, 20, 20.5, 30.6, 40};

  FillScalarVector(final_tarray, final_sarray, 10);

  result = albaEquals(m_SVector->GetItemBefore(2.4), 20.0);
  TEST_RESULT;
  result = albaEquals(m_SVector->GetNearestItem(2.4), 20.5);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTimeMapScalarTest::TestSearchOutOfBounds()
//----------------------------------------------------------------------------
{
  albaTimeStamp final_tarray[10]={.2, .3, .5, 1, 1.2, 1.8, 2, 2.5, 3.6, 4};
  double final_sarray[10]={1.2, 1.3, 1.5, 10, 10.2, 10.8, 20, 20.5, 30.6, 40};

  FillScalarVector(final_tarray, final_sarray, 10);

  result = albaEquals(m_SVector->GetItemBefore(.1), 1.2);
  TEST_RESULT;
  result = albaEquals(m_SVector->GetNearestItem(.1), 1.2);
  TEST_RESULT;

  result = albaEquals(m_SVector->GetItemBefore(5), 40);
  TEST_RESULT;
  result = albaEquals(m_SVector->GetNearestItem(5), 40);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTimeMapScalarTest::TestItemsRemoval()
//----------------------------------------------------------------------------
{
  albaTimeStamp final_tarray[10]={.2, .3, .5, 1, 1.2, 1.8, 2, 2.5, 3.6, 4};
  double final_sarray[10]={1.2, 1.3, 1.5, 10, 10.2, 10.8, 20, 20.5, 30.6, 40};

  FillScalarVector(final_tarray, final_sarray, 10);

  albaTestSVector::TimeMapScalars::iterator it;
  it = m_SVector->FindItem(2.0);
  result = it != m_SVector->EndScalarVector();
  TEST_RESULT;

  m_SVector->RemoveItem(it);
  result = m_SVector->GetNumberOfItems() == 9;
  TEST_RESULT;

  result = m_SVector->FindItem(2.0) == m_SVector->EndScalarVector();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaTimeMapScalarTest::FillScalarVector(double *t, double *s, int n)
//----------------------------------------------------------------------------
{
  for (int i = 0; i < n; i++)
  {
    m_SVector->InsertItem(t[i], s[i]);
  }
}
