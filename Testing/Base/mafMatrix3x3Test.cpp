/*=========================================================================

 Program: MAF2
 Module: mafMatrix3x3Test
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
#include "mafMatrix3x3Test.h"

#include "mafMatrix3x3.h"

#include "mafSmartPointer.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafMatrix3x3Test::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafMatrix3x3Test::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void mafMatrix3x3Test::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafMatrix3x3Test::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafMatrix3x3 m;
}
//----------------------------------------------------------------------------
void mafMatrix3x3Test::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafMatrix3x3 *m = new mafMatrix3x3();
  cppDEL(m);
}
//----------------------------------------------------------------------------
void mafMatrix3x3Test::TestCopy()
//----------------------------------------------------------------------------
{
  mafMatrix3x3 m;
  m.SetElement(0,2, 5.0);

  mafMatrix3x3 m2;
  m2 = m;

  result = mafEquals(m2.GetElement(0,2), 5.0);
  TEST_RESULT;

  mafMatrix3x3 *m3 = new mafMatrix3x3();
  *m3 = m;
  result = mafEquals(m3->GetElement(0,2), 5.0);
  TEST_RESULT;
  cppDEL(m3);

  mafMatrix3x3 *m4 = new mafMatrix3x3(m);
  result = mafEquals(m4->GetElement(0,2), 5.0);
  TEST_RESULT;
  cppDEL(m4);
}
//----------------------------------------------------------------------------
void mafMatrix3x3Test::TestZero()
//----------------------------------------------------------------------------
{
  int r,c;
  mafMatrix3x3 m; // 3x3 matrix is initialized to '0' 
                  // differently from mafMatrix that is initialized to the Identity matrix
  for (r=0;r<3;r++)
  {
    for (c=0;c<3;c++)
    {
      result = mafEquals(m.GetElement(r,c), 0.0);
      TEST_RESULT;
    }
  }

  m.SetElement(0,2, 5.0);
  m.Zero();
  // Test again
  for (r=0;r<3;r++)
  {
    for (c=0;c<3;c++)
    {
      result = mafEquals(m.GetElement(r,c), 0.0);
      TEST_RESULT;
    }
  }
}
//----------------------------------------------------------------------------
void mafMatrix3x3Test::TestGetVersor()
//----------------------------------------------------------------------------
{
  double v[3];
  mafMatrix3x3 m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);
  m.GetVersor(0,v);
  result = mafEquals(v[0],3.0);
  TEST_RESULT;
  result = mafEquals(v[1],2.0);
  TEST_RESULT;
  result = mafEquals(v[2],0.0);
  TEST_RESULT;

  m.GetVersor(1,v);
  result = mafEquals(v[0],0.0);
  TEST_RESULT;
  result = mafEquals(v[1],0.0);
  TEST_RESULT;
  result = mafEquals(v[2],0.0);
  TEST_RESULT;

  m.GetVersor(2,v);
  result = mafEquals(v[0],0.0);
  TEST_RESULT;
  result = mafEquals(v[1],0.0);
  TEST_RESULT;
  result = mafEquals(v[2],5.0);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafMatrix3x3Test::TestIdentity()
//----------------------------------------------------------------------------
{
  mafMatrix3x3 m;
  m.Identity();
  for (int i = 0; i < 3; i++)
  {
    result = mafEquals(m.GetElement(i,i), 1.0);
    TEST_RESULT;
  }
}
//----------------------------------------------------------------------------
void mafMatrix3x3Test::TestMultiplyVector()
//----------------------------------------------------------------------------
{
  double in_vect[3] = {1.0, 2.0, -2.0};
  double out_vect[3];
  mafMatrix3x3 m;
  m.Identity();
  m.SetElement(1,0,2.0);
  m.MultiplyVector(in_vect,out_vect);
  
  result = mafEquals(out_vect[0], 1.0);
  TEST_RESULT;
  result = mafEquals(out_vect[1], 4.0);
  TEST_RESULT;
  result = mafEquals(out_vect[2], -2.0);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafMatrix3x3Test::TestMultiply()
//----------------------------------------------------------------------------
{
  double solution[9] = {1.0, 0.0, 3.0, 2.0, 1.0, 6.0, 2.0, 0.0, 1.0};

  mafMatrix3x3 m1;
  m1.Identity();
  m1.SetElement(1,0,2.0);

  mafMatrix3x3 m2;
  m2.Identity();
  m2.SetElement(2,0,2.0);
  m2.SetElement(0,2,3.0);

  mafMatrix3x3 m3;
  mafMatrix3x3::Multiply(m1,m2,m3);
  
  int i = 0;
  for (int r = 0; r < 3; r++)
  {
    for (int c = 0; c < 3; c++, i++)
    {
      result = mafEquals(m3.GetElement(r,c), solution[i]);
      TEST_RESULT;
    }
  }
}
//----------------------------------------------------------------------------
void mafMatrix3x3Test::TestTranspose()
//----------------------------------------------------------------------------
{
  mafMatrix3x3 m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);
  result = mafEquals(m.GetElement(1,0),2.0);
  TEST_RESULT;

  m.Transpose();
  result = mafEquals(m.GetElement(0,1),2.0);
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafMatrix3x3Test::TestInvert()
//----------------------------------------------------------------------------
{
  mafMatrix3x3 m;
  m.Identity();
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);

  mafMatrix3x3 m2;
  
  // Test the static method.
  mafMatrix3x3::Invert(m.GetElements(),m2.GetElements());

  mafMatrix3x3 mi;
  mafMatrix3x3::Multiply(m,m2,mi);
  int i;
  for (i = 0; i < 3; i++)
  {
    result = mafEquals(mi.GetElement(i,i), 1.0);
    TEST_RESULT;
  }

  m2 = m;
  // Test the in-place Invert method.
  m.Invert();
  mafMatrix3x3::Multiply(m,m2,mi);
  for (i = 0; i < 3; i++)
  {
    result = mafEquals(mi.GetElement(i,i), 1.0);
    TEST_RESULT;
  }
}
//----------------------------------------------------------------------------
void mafMatrix3x3Test::TestDeterminant()
//----------------------------------------------------------------------------
{
  mafMatrix3x3 m;
  m.Identity();
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);

  double d = m.Determinant();
  result = mafEquals(d, 15.0);
  TEST_RESULT;
}
