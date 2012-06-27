/*=========================================================================

 Program: MAF2
 Module: mafMatrixTest
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
#include "mafMatrixTest.h"

#include "mafMatrix.h"
#include "mafSmartPointer.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMatrix4x4.h"

#include <iostream>

//----------------------------------------------------------------------------
void mafMatrixTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafMatrixTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafMatrixTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafMatrixTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafMatrix m;
}
//----------------------------------------------------------------------------
void mafMatrixTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafMatrix *m = new mafMatrix();
  cppDEL(m);
}
//----------------------------------------------------------------------------
void mafMatrixTest::TestAllConstructors()
//----------------------------------------------------------------------------
{
  mafMatrix m;
  m.SetElement(0,3, 5.0);

  mafMatrix *m2 = new mafMatrix(m);
  CPPUNIT_ASSERT(mafEquals(m2->GetElement(0,3), 5.0));
  cppDEL(m2);

  vtkMAFSmartPointer<vtkMatrix4x4> vtk_mat;
  vtk_mat->SetElement(0,3,5.0);
  
  mafSmartPointer<mafMatrix> smart_matrix;
  smart_matrix->SetVTKMatrix(vtk_mat);
  CPPUNIT_ASSERT(mafEquals(smart_matrix->GetElement(0,3), 5.0));

  mafMatrix *m3 = new mafMatrix(vtk_mat);
  CPPUNIT_ASSERT(mafEquals(m3->GetElement(0,3), 5.0));
  cppDEL(m3);
}
//----------------------------------------------------------------------------
void mafMatrixTest::TestCopy()
//----------------------------------------------------------------------------
{
  mafMatrix m;
  m.SetElement(0,3, 5.0);

  mafMatrix m2;
  m2 = m;

  CPPUNIT_ASSERT(mafEquals(m2.GetElement(0,3), 5.0));

  mafMatrix *m3 = new mafMatrix();
  *m3 = m;
  CPPUNIT_ASSERT(mafEquals(m3->GetElement(0,3), 5.0));
  cppDEL(m3);
}
//----------------------------------------------------------------------------
void mafMatrixTest::TestZero()
//----------------------------------------------------------------------------
{
  mafMatrix m;
  m.Zero();
  for (int r=0;r<4;r++)
  {
    for (int c=0;c<4;c++)
    {
      CPPUNIT_ASSERT(mafEquals(m.GetElement(r,c), 0.0));
    }
  }
}
//----------------------------------------------------------------------------
void mafMatrixTest::TestGetVersor()
//----------------------------------------------------------------------------
{
  double v[3];
  mafMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);
  m.GetVersor(0,v);
  CPPUNIT_ASSERT(mafEquals(v[0],3.0));
  CPPUNIT_ASSERT(mafEquals(v[1],2.0));
  CPPUNIT_ASSERT(mafEquals(v[2],0.0));

  m.GetVersor(1,v);
  CPPUNIT_ASSERT(mafEquals(v[0],0.0));
  CPPUNIT_ASSERT(mafEquals(v[1],1.0));
  CPPUNIT_ASSERT(mafEquals(v[2],0.0));

  m.GetVersor(2,v);
  CPPUNIT_ASSERT(mafEquals(v[0],0.0));
  CPPUNIT_ASSERT(mafEquals(v[1],0.0));
  CPPUNIT_ASSERT(mafEquals(v[2],5.0));
}
//----------------------------------------------------------------------------
void mafMatrixTest::TestSetVTKMatrix()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkMatrix4x4> vtk_mat;
  vtk_mat->SetElement(0,3,5.0);

  mafMatrix m;
  m.SetVTKMatrix(vtk_mat);

  CPPUNIT_ASSERT(mafEquals(m.GetElement(0,3), 5.0));

  mafMatrix *m2 = new mafMatrix();
  m2->SetVTKMatrix(vtk_mat);
  CPPUNIT_ASSERT(mafEquals(m2->GetElement(0,3), 5.0));
  cppDEL(m2);
}
//----------------------------------------------------------------------------
void mafMatrixTest::TestSetTimeStamp()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkMatrix4x4> vtk_mat;
  vtk_mat->SetElement(0,3,5.0);

  mafMatrix m;
  m.SetVTKMatrix(vtk_mat,1.0);
  CPPUNIT_ASSERT(mafEquals(m.GetTimeStamp(), 1.0));

  // Store the actual Timestamp
  unsigned long t0 = m.GetMTime();
  m.SetTimeStamp(3.0);
  CPPUNIT_ASSERT(mafEquals(m.GetTimeStamp(), 3.0));
  unsigned long t1 = m.GetMTime();
  
  // After TimeStamp is set the modified time has to be greater then before.
  CPPUNIT_ASSERT(t1 > t0);

  CPPUNIT_ASSERT(mafEquals(m.GetElement(0,3), 5.0));
}
//----------------------------------------------------------------------------
void mafMatrixTest::TestCopyRotation()
//----------------------------------------------------------------------------
{
  mafMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);

  mafMatrix *m2 = new mafMatrix();
  m2->CopyRotation(m);

  CPPUNIT_ASSERT(mafEquals(m2->GetElement(0,0), 3.0));
  CPPUNIT_ASSERT(mafEquals(m2->GetElement(1,0), 2.0));
  CPPUNIT_ASSERT(mafEquals(m2->GetElement(2,2), 5.0));

  cppDEL(m2);
}
//----------------------------------------------------------------------------
void mafMatrixTest::TestIdentity()
//----------------------------------------------------------------------------
{
  mafMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);

  m.Identity();
  double attended_result;

  for (int r=0;r<4;r++)
  {
    for (int c=0;c<4;c++)
    {
      attended_result = r == c ? 1.0 : 0.0;
      CPPUNIT_ASSERT(mafEquals(m.GetElement(r,c), attended_result));
    }
  }
}
//----------------------------------------------------------------------------
void mafMatrixTest::TestInvert()
//----------------------------------------------------------------------------
{
  mafMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);

  mafMatrix m2;
  m2 = m;

  m.Invert();

  mafMatrix mi;
  mafMatrix::Multiply4x4(m,m2,mi);
  for (int i = 0; i < 4; i++)
  {
    CPPUNIT_ASSERT(mafEquals(mi.GetElement(i,i), 1.0));
  }
}
//----------------------------------------------------------------------------
void mafMatrixTest::TestDeterminant()
//----------------------------------------------------------------------------
{
  mafMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);

  double d = m.Determinant();
  CPPUNIT_ASSERT(mafEquals(d, 15.0));
}
//----------------------------------------------------------------------------
void mafMatrixTest::TestAdjoint()
//----------------------------------------------------------------------------
{
  // From the formula:
  //     -1     
  //     A  = ___1__ adjoint A
  //         det A
  //
  //                              -1
  // will be tested that det(A) * A == adjoint(A)
  //

  mafMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);

  double d = m.Determinant();

  mafMatrix *m2 = new mafMatrix(m);
  m2->Invert();

  m.Adjoint();

  for (int r=0;r<4;r++)
  {
    for (int c=0;c<4;c++)
    {
      m2->SetElement(r,c,m2->GetElement(r,c)*d);
    }
  }
  
  CPPUNIT_ASSERT(m.Equals(m2));
  cppDEL(m2);
}
//----------------------------------------------------------------------------
void mafMatrixTest::TestTranspose()
//----------------------------------------------------------------------------
{
  mafMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);
  CPPUNIT_ASSERT(mafEquals(m.GetElement(1,0),2.0));

  m.Transpose();
  CPPUNIT_ASSERT(mafEquals(m.GetElement(0,1),2.0));
}
//----------------------------------------------------------------------------
void mafMatrixTest::TestMultiplyPoint()
//----------------------------------------------------------------------------
{
  double in_point[4] = {1.0, 2.0, 1.5, 3.0};
  double out_point[4];

  mafMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);
  m.MultiplyPoint(in_point, out_point);

  CPPUNIT_ASSERT(mafEquals(out_point[0],3.0));
  CPPUNIT_ASSERT(mafEquals(out_point[1],4.0));
  CPPUNIT_ASSERT(mafEquals(out_point[2],7.5));
  CPPUNIT_ASSERT(mafEquals(out_point[3],3.0));
}
//----------------------------------------------------------------------------
void mafMatrixTest::TestMultiply4x4()
//----------------------------------------------------------------------------
{
  mafMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);

  mafMatrix m1;
  m1.SetElement(0,0,2.0);
  m1.SetElement(1,0,3.0);
  m1.SetElement(2,2,2.0);
  m1.SetElement(3,0,5.0);

  mafMatrix m2;
  m.Multiply4x4(m,m1,m2);

  CPPUNIT_ASSERT(mafEquals(m2.GetElement(0,0), 6.0));
  CPPUNIT_ASSERT(mafEquals(m2.GetElement(1,0), 7.0));
  CPPUNIT_ASSERT(mafEquals(m2.GetElement(2,2), 10.0));
  CPPUNIT_ASSERT(mafEquals(m2.GetElement(3,0), 5.0));
}
//----------------------------------------------------------------------------
void mafMatrixTest::TestPointMultiply()
//----------------------------------------------------------------------------
{
  double matrix[16];
  double in_point[4] = {1.0, 2.0, 1.5, 3.0};
  double out_point[4];

  mafMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);

  int i = 0;
  for (int r=0;r<4;r++)
  {
    for (int c=0;c<4;c++)
    {
      matrix[i] = m.GetElement(r,c);
      i++;
    }
  }

  mafMatrix::PointMultiply(matrix, in_point, out_point);
  CPPUNIT_ASSERT(mafEquals(out_point[0],7.0));
  CPPUNIT_ASSERT(mafEquals(out_point[1],2.0));
  CPPUNIT_ASSERT(mafEquals(out_point[2],7.5));
  CPPUNIT_ASSERT(mafEquals(out_point[3],3.0));
}
