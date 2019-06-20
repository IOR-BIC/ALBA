/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOBBTest
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
#include "albaOBBTest.h"

#include "albaMatrix.h"
#include "albaOBB.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaOBBTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaOBBTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaOBB obb;
}
//----------------------------------------------------------------------------
void albaOBBTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaOBB *obb = new albaOBB();
  cppDEL(obb);
}
//----------------------------------------------------------------------------
void albaOBBTest::TestAllConstructors()
//----------------------------------------------------------------------------
{
  double b[6] = {1.0, 2.0, 3.0, 21.0, 3.0, 10.0};
  albaOBB *obb = new albaOBB(b);
  cppDEL(obb);
}
//----------------------------------------------------------------------------
void albaOBBTest::TestEquals()
//----------------------------------------------------------------------------
{
  double b[6] = {1.0, 2.0, 3.0, 21.0, 3.0, 10.0};
  albaOBB *obb = new albaOBB(b);
  result = obb->Equals(b);
  TEST_RESULT;
  cppDEL(obb);
}
//----------------------------------------------------------------------------
void albaOBBTest::TestCopy()
//----------------------------------------------------------------------------
{
  double b[6] = {1.0, 2.0, 3.0, 21.0, 3.0, 10.0};
  albaOBB *obb = new albaOBB(b);

  albaOBB obb2;
  obb2 = *obb;

  result = obb2.Equals(obb);
  TEST_RESULT;
  cppDEL(obb);

  obb2.Reset();
  obb2.DeepCopy(b);
  result = obb2.Equals(b);
  TEST_RESULT;

  albaOBB obb3(b);
  obb2.Reset();
  obb2.DeepCopy(obb3);
  result = obb2.Equals(obb3);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaOBBTest::TestIsValid()
//----------------------------------------------------------------------------
{
  double b[6] = {1.0, 2.0, 3.0, 21.0, 3.0, 10.0};
  albaOBB obb(b);
  result = obb.IsValid();
  TEST_RESULT;

  obb.Reset();
  result = !obb.IsValid();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaOBBTest::TestCopyTo()
//----------------------------------------------------------------------------
{
  double b[6] = {1.0, 2.0, 3.0, 21.0, 3.0, 10.0};
  double target[6];

  albaOBB obb(b);
  obb.CopyTo(target);
  result = obb.Equals(target);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaOBBTest::TestApplyTransform()
//----------------------------------------------------------------------------
{
  albaMatrix m;
  m.SetElement(2, 0, 2.0);

  double solution[6] = {1.0, 2.0, 3.0, 21.0, 5.0, 14.0};
  double b[6] = {1.0, 2.0, 3.0, 21.0, 3.0, 10.0};
  albaOBB obb(b);

  albaOBB obb2;
  obb2.m_Matrix = m;
  obb2.ApplyTransform(obb); // Apply the 'obb2.m_Matrix' to 'obb' 
                            // and set to Identity the 'obb.m_Matrix'
  result = obb.IsValid();
  TEST_RESULT;

  albaMatrix mi; // Identity matrix
  result = obb.m_Matrix.Equals(&mi);
  TEST_RESULT;

  result = obb.Equals(solution);
  TEST_RESULT;

  // Test the static method
  obb.Reset();
  for (int i = 0; i < 6; i++)
  {
    obb.m_Bounds[i] = b[i];
  }
  albaOBB::ApplyTransform(obb2.m_Matrix, obb);

  result = obb.IsValid();
  TEST_RESULT;

  result = obb.m_Matrix.Equals(&mi);
  TEST_RESULT;

  result = obb.Equals(solution);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaOBBTest::TestMergeBounds()
//----------------------------------------------------------------------------
{
  double solution[6] = {1.0, 3.0, -3.0, 21.0, 3.0, 14.0};
  double b1[6] = {1.0, 3.0, 3.0, 21.0, 5.0, 14.0};
  double b2[6] = {1.0, 2.0, -3.0, 21.0, 3.0, 10.0};

  albaOBB obb(b1);
  albaOBB obb2(b2);

  obb.MergeBounds(obb2);

  result = obb.IsValid();
  TEST_RESULT;

  result = obb.Equals(solution);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaOBBTest::TestIsInside()
//----------------------------------------------------------------------------
{
  double p_inside[3] = {-1.0, 2.0, 5.0};
  double p_outside[3] = {-2.0, 5.0, 21.0};
  double b1[6] = {-1.0, 3.0, -3.0, 21.0, -5.0, 14.0};
  albaOBB obb(b1);

  result = obb.IsInside(p_inside);
  TEST_RESULT;

  result = !obb.IsInside(p_outside);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaOBBTest::TsetGetDimensions()
//----------------------------------------------------------------------------
{
  double solution[3] = {4.0, 24.0, 19.0};
  double dim[3];
  double b1[6] = {-1.0, 3.0, -3.0, 21.0, -5.0, 14.0};
  albaOBB obb(b1);

  obb.GetDimensions(dim);
  for (int i=0;i<3;i++)
  {
    result = albaEquals(dim[i],solution[i]);
    TEST_RESULT;
  }
}
//----------------------------------------------------------------------------
void albaOBBTest::TestGetCenter()
//----------------------------------------------------------------------------
{
  double solution[3] = {1.0, 9.0, 4.5};
  double center[3];
  double b1[6] = {-1.0, 3.0, -3.0, 21.0, -5.0, 14.0};
  albaOBB obb(b1);

  obb.GetCenter(center);
  for (int i=0;i<3;i++)
  {
    result = albaEquals(center[i],solution[i]);
    TEST_RESULT;
  }
}
//----------------------------------------------------------------------------
void albaOBBTest::TestSetCenter()
//----------------------------------------------------------------------------
{
  double b1[6] = {-1.0, 3.0, -3.0, 21.0, -5.0, 14.0};
  albaOBB obb(b1);

  double new_center[3] = {2.0, 10.0, 5.0};
  obb.SetCenter(new_center);

  result = obb.IsValid();
  TEST_RESULT;

  double center[3];
  obb.GetCenter(center);
  for (int i=0;i<3;i++)
  {
    result = albaEquals(center[i],new_center[i]);
    TEST_RESULT;
  }
}
//----------------------------------------------------------------------------
void albaOBBTest::TestSetDimensions()
//----------------------------------------------------------------------------
{
  double center_preserved[3];
  double new_dims[3] = {10.0, 15.0, 22.0};
  double b1[6] = {-1.0, 3.0, -3.0, 21.0, -5.0, 14.0};
  albaOBB obb(b1);
  obb.GetCenter(center_preserved);
  obb.SetDimensions(new_dims);

  result = obb.IsValid();
  TEST_RESULT;

  double center[3];
  obb.GetCenter(center);
  for (int i=0;i<3;i++)
  {
    result = albaEquals(center[i], center_preserved[i]);
    TEST_RESULT;
  }
}
//----------------------------------------------------------------------------
void albaOBBTest::TestGetWidth()
//----------------------------------------------------------------------------
{
  double b1[6] = {-1.0, 3.0, -3.0, 21.0, -5.0, 14.0};
  albaOBB obb(b1);

  result = albaEquals(obb.GetWidth(), 4.0);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaOBBTest::TestGetHeight()
//----------------------------------------------------------------------------
{
  double b1[6] = {-1.0, 3.0, -3.0, 21.0, -5.0, 14.0};
  albaOBB obb(b1);

  result = albaEquals(obb.GetHeight(), 24.0);
  TEST_RESULT;

}
//----------------------------------------------------------------------------
void albaOBBTest::TsetGetDepth()
//----------------------------------------------------------------------------
{
  double b1[6] = {-1.0, 3.0, -3.0, 21.0, -5.0, 14.0};
  albaOBB obb(b1);

  result = albaEquals(obb.GetDepth(), 19.0);
  TEST_RESULT;

}
