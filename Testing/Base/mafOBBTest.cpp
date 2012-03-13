/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOBBTest.cpp,v $
Language:  C++
Date:      $Date: 2006-11-07 09:53:51 $
Version:   $Revision: 1.1 $
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
#include "mafOBBTest.h"

#include "mafMatrix.h"
#include "mafOBB.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafOBBTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOBBTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void mafOBBTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOBBTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafOBB obb;
}
//----------------------------------------------------------------------------
void mafOBBTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafOBB *obb = new mafOBB();
  cppDEL(obb);
}
//----------------------------------------------------------------------------
void mafOBBTest::TestAllConstructors()
//----------------------------------------------------------------------------
{
  double b[6] = {1.0, 2.0, 3.0, 21.0, 3.0, 10.0};
  mafOBB *obb = new mafOBB(b);
  cppDEL(obb);
}
//----------------------------------------------------------------------------
void mafOBBTest::TestEquals()
//----------------------------------------------------------------------------
{
  double b[6] = {1.0, 2.0, 3.0, 21.0, 3.0, 10.0};
  mafOBB *obb = new mafOBB(b);
  result = obb->Equals(b);
  TEST_RESULT;
  cppDEL(obb);
}
//----------------------------------------------------------------------------
void mafOBBTest::TestCopy()
//----------------------------------------------------------------------------
{
  double b[6] = {1.0, 2.0, 3.0, 21.0, 3.0, 10.0};
  mafOBB *obb = new mafOBB(b);

  mafOBB obb2;
  obb2 = *obb;

  result = obb2.Equals(obb);
  TEST_RESULT;
  cppDEL(obb);

  obb2.Reset();
  obb2.DeepCopy(b);
  result = obb2.Equals(b);
  TEST_RESULT;

  mafOBB obb3(b);
  obb2.Reset();
  obb2.DeepCopy(obb3);
  result = obb2.Equals(obb3);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafOBBTest::TestIsValid()
//----------------------------------------------------------------------------
{
  double b[6] = {1.0, 2.0, 3.0, 21.0, 3.0, 10.0};
  mafOBB obb(b);
  result = obb.IsValid();
  TEST_RESULT;

  obb.Reset();
  result = !obb.IsValid();
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafOBBTest::TestCopyTo()
//----------------------------------------------------------------------------
{
  double b[6] = {1.0, 2.0, 3.0, 21.0, 3.0, 10.0};
  double target[6];

  mafOBB obb(b);
  obb.CopyTo(target);
  result = obb.Equals(target);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafOBBTest::TestApplyTransform()
//----------------------------------------------------------------------------
{
  mafMatrix m;
  m.SetElement(2, 0, 2.0);

  double solution[6] = {1.0, 2.0, 3.0, 21.0, 5.0, 14.0};
  double b[6] = {1.0, 2.0, 3.0, 21.0, 3.0, 10.0};
  mafOBB obb(b);

  mafOBB obb2;
  obb2.m_Matrix = m;
  obb2.ApplyTransform(obb); // Apply the 'obb2.m_Matrix' to 'obb' 
                            // and set to Identity the 'obb.m_Matrix'
  result = obb.IsValid();
  TEST_RESULT;

  mafMatrix mi; // Identity matrix
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
  mafOBB::ApplyTransform(obb2.m_Matrix, obb);

  result = obb.IsValid();
  TEST_RESULT;

  result = obb.m_Matrix.Equals(&mi);
  TEST_RESULT;

  result = obb.Equals(solution);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafOBBTest::TestMergeBounds()
//----------------------------------------------------------------------------
{
  double solution[6] = {1.0, 3.0, -3.0, 21.0, 3.0, 14.0};
  double b1[6] = {1.0, 3.0, 3.0, 21.0, 5.0, 14.0};
  double b2[6] = {1.0, 2.0, -3.0, 21.0, 3.0, 10.0};

  mafOBB obb(b1);
  mafOBB obb2(b2);

  obb.MergeBounds(obb2);

  result = obb.IsValid();
  TEST_RESULT;

  result = obb.Equals(solution);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafOBBTest::TestIsInside()
//----------------------------------------------------------------------------
{
  double p_inside[3] = {-1.0, 2.0, 5.0};
  double p_outside[3] = {-2.0, 5.0, 21.0};
  double b1[6] = {-1.0, 3.0, -3.0, 21.0, -5.0, 14.0};
  mafOBB obb(b1);

  result = obb.IsInside(p_inside);
  TEST_RESULT;

  result = !obb.IsInside(p_outside);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafOBBTest::TsetGetDimensions()
//----------------------------------------------------------------------------
{
  double solution[3] = {4.0, 24.0, 19.0};
  double dim[3];
  double b1[6] = {-1.0, 3.0, -3.0, 21.0, -5.0, 14.0};
  mafOBB obb(b1);

  obb.GetDimensions(dim);
  for (int i=0;i<3;i++)
  {
    result = mafEquals(dim[i],solution[i]);
    TEST_RESULT;
  }
}
//----------------------------------------------------------------------------
void mafOBBTest::TestGetCenter()
//----------------------------------------------------------------------------
{
  double solution[3] = {1.0, 9.0, 4.5};
  double center[3];
  double b1[6] = {-1.0, 3.0, -3.0, 21.0, -5.0, 14.0};
  mafOBB obb(b1);

  obb.GetCenter(center);
  for (int i=0;i<3;i++)
  {
    result = mafEquals(center[i],solution[i]);
    TEST_RESULT;
  }
}
//----------------------------------------------------------------------------
void mafOBBTest::TestSetCenter()
//----------------------------------------------------------------------------
{
  double b1[6] = {-1.0, 3.0, -3.0, 21.0, -5.0, 14.0};
  mafOBB obb(b1);

  double new_center[3] = {2.0, 10.0, 5.0};
  obb.SetCenter(new_center);

  result = obb.IsValid();
  TEST_RESULT;

  double center[3];
  obb.GetCenter(center);
  for (int i=0;i<3;i++)
  {
    result = mafEquals(center[i],new_center[i]);
    TEST_RESULT;
  }
}
//----------------------------------------------------------------------------
void mafOBBTest::TestSetDimensions()
//----------------------------------------------------------------------------
{
  double center_preserved[3];
  double new_dims[3] = {10.0, 15.0, 22.0};
  double b1[6] = {-1.0, 3.0, -3.0, 21.0, -5.0, 14.0};
  mafOBB obb(b1);
  obb.GetCenter(center_preserved);
  obb.SetDimensions(new_dims);

  result = obb.IsValid();
  TEST_RESULT;

  double center[3];
  obb.GetCenter(center);
  for (int i=0;i<3;i++)
  {
    result = mafEquals(center[i], center_preserved[i]);
    TEST_RESULT;
  }
}
//----------------------------------------------------------------------------
void mafOBBTest::TestGetWidth()
//----------------------------------------------------------------------------
{
  double b1[6] = {-1.0, 3.0, -3.0, 21.0, -5.0, 14.0};
  mafOBB obb(b1);

  result = mafEquals(obb.GetWidth(), 4.0);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafOBBTest::TestGetHeight()
//----------------------------------------------------------------------------
{
  double b1[6] = {-1.0, 3.0, -3.0, 21.0, -5.0, 14.0};
  mafOBB obb(b1);

  result = mafEquals(obb.GetHeight(), 24.0);
  TEST_RESULT;

}
//----------------------------------------------------------------------------
void mafOBBTest::TsetGetDepth()
//----------------------------------------------------------------------------
{
  double b1[6] = {-1.0, 3.0, -3.0, 21.0, -5.0, 14.0};
  mafOBB obb(b1);

  result = mafEquals(obb.GetDepth(), 19.0);
  TEST_RESULT;

}
