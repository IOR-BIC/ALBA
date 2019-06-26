/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMatrixTest
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
#include "albaMatrixTest.h"

#include "albaMatrix.h"
#include "albaSmartPointer.h"

#include "vtkALBASmartPointer.h"
#include "vtkMatrix4x4.h"

#include <iostream>

//----------------------------------------------------------------------------
void albaMatrixTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaMatrixTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaMatrix m;
}
//----------------------------------------------------------------------------
void albaMatrixTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaMatrix *m = new albaMatrix();
  cppDEL(m);
}
//----------------------------------------------------------------------------
void albaMatrixTest::TestAllConstructors()
//----------------------------------------------------------------------------
{
  albaMatrix m;
  m.SetElement(0,3, 5.0);

  albaMatrix *m2 = new albaMatrix(m);
  CPPUNIT_ASSERT(albaEquals(m2->GetElement(0,3), 5.0));
  cppDEL(m2);

  vtkALBASmartPointer<vtkMatrix4x4> vtk_mat;
  vtk_mat->SetElement(0,3,5.0);
  
  albaSmartPointer<albaMatrix> smart_matrix;
  smart_matrix->SetVTKMatrix(vtk_mat);
  CPPUNIT_ASSERT(albaEquals(smart_matrix->GetElement(0,3), 5.0));

  albaMatrix *m3 = new albaMatrix(vtk_mat);
  CPPUNIT_ASSERT(albaEquals(m3->GetElement(0,3), 5.0));
  cppDEL(m3);
}
//----------------------------------------------------------------------------
void albaMatrixTest::TestCopy()
//----------------------------------------------------------------------------
{
  albaMatrix m;
  m.SetElement(0,3, 5.0);

  albaMatrix m2;
  m2 = m;

  CPPUNIT_ASSERT(albaEquals(m2.GetElement(0,3), 5.0));

  albaMatrix *m3 = new albaMatrix();
  *m3 = m;
  CPPUNIT_ASSERT(albaEquals(m3->GetElement(0,3), 5.0));
  cppDEL(m3);
}
//----------------------------------------------------------------------------
void albaMatrixTest::TestZero()
//----------------------------------------------------------------------------
{
  albaMatrix m;
  m.Zero();
  for (int r=0;r<4;r++)
  {
    for (int c=0;c<4;c++)
    {
      CPPUNIT_ASSERT(albaEquals(m.GetElement(r,c), 0.0));
    }
  }
}
//----------------------------------------------------------------------------
void albaMatrixTest::TestGetVersor()
//----------------------------------------------------------------------------
{
  double v[3];
  albaMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);
  m.GetVersor(0,v);
  CPPUNIT_ASSERT(albaEquals(v[0],3.0));
  CPPUNIT_ASSERT(albaEquals(v[1],2.0));
  CPPUNIT_ASSERT(albaEquals(v[2],0.0));

  m.GetVersor(1,v);
  CPPUNIT_ASSERT(albaEquals(v[0],0.0));
  CPPUNIT_ASSERT(albaEquals(v[1],1.0));
  CPPUNIT_ASSERT(albaEquals(v[2],0.0));

  m.GetVersor(2,v);
  CPPUNIT_ASSERT(albaEquals(v[0],0.0));
  CPPUNIT_ASSERT(albaEquals(v[1],0.0));
  CPPUNIT_ASSERT(albaEquals(v[2],5.0));
}
//----------------------------------------------------------------------------
void albaMatrixTest::TestSetVTKMatrix()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkMatrix4x4> vtk_mat;
  vtk_mat->SetElement(0,3,5.0);

  albaMatrix m;
  m.SetVTKMatrix(vtk_mat);

  CPPUNIT_ASSERT(albaEquals(m.GetElement(0,3), 5.0));

  albaMatrix *m2 = new albaMatrix();
  m2->SetVTKMatrix(vtk_mat);
  CPPUNIT_ASSERT(albaEquals(m2->GetElement(0,3), 5.0));
  cppDEL(m2);
}
//----------------------------------------------------------------------------
void albaMatrixTest::TestSetTimeStamp()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkMatrix4x4> vtk_mat;
  vtk_mat->SetElement(0,3,5.0);

  albaMatrix m;
  m.SetVTKMatrix(vtk_mat,1.0);
  CPPUNIT_ASSERT(albaEquals(m.GetTimeStamp(), 1.0));

  // Store the actual Timestamp
  unsigned long t0 = m.GetMTime();
  m.SetTimeStamp(3.0);
  CPPUNIT_ASSERT(albaEquals(m.GetTimeStamp(), 3.0));
  unsigned long t1 = m.GetMTime();
  
  // After TimeStamp is set the modified time has to be greater then before.
  CPPUNIT_ASSERT(t1 > t0);

  CPPUNIT_ASSERT(albaEquals(m.GetElement(0,3), 5.0));
}
//----------------------------------------------------------------------------
void albaMatrixTest::TestCopyRotation()
//----------------------------------------------------------------------------
{
  albaMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);

  albaMatrix *m2 = new albaMatrix();
  m2->CopyRotation(m);

  CPPUNIT_ASSERT(albaEquals(m2->GetElement(0,0), 3.0));
  CPPUNIT_ASSERT(albaEquals(m2->GetElement(1,0), 2.0));
  CPPUNIT_ASSERT(albaEquals(m2->GetElement(2,2), 5.0));

  cppDEL(m2);
}
//----------------------------------------------------------------------------
void albaMatrixTest::TestIdentity()
//----------------------------------------------------------------------------
{
  albaMatrix m;
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
      CPPUNIT_ASSERT(albaEquals(m.GetElement(r,c), attended_result));
    }
  }
}
//----------------------------------------------------------------------------
void albaMatrixTest::TestInvert()
//----------------------------------------------------------------------------
{
  albaMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);

  albaMatrix m2;
  m2 = m;

  m.Invert();

  albaMatrix mi;
  albaMatrix::Multiply4x4(m,m2,mi);
  for (int i = 0; i < 4; i++)
  {
    CPPUNIT_ASSERT(albaEquals(mi.GetElement(i,i), 1.0));
  }
}
//----------------------------------------------------------------------------
void albaMatrixTest::TestDeterminant()
//----------------------------------------------------------------------------
{
  albaMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);

  double d = m.Determinant();
  CPPUNIT_ASSERT(albaEquals(d, 15.0));
}
//----------------------------------------------------------------------------
void albaMatrixTest::TestAdjoint()
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

  albaMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);

  double d = m.Determinant();

  albaMatrix *m2 = new albaMatrix(m);
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
void albaMatrixTest::TestTranspose()
//----------------------------------------------------------------------------
{
  albaMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);
  CPPUNIT_ASSERT(albaEquals(m.GetElement(1,0),2.0));

  m.Transpose();
  CPPUNIT_ASSERT(albaEquals(m.GetElement(0,1),2.0));
}
//----------------------------------------------------------------------------
void albaMatrixTest::TestMultiplyPoint()
//----------------------------------------------------------------------------
{
  double in_point[4] = {1.0, 2.0, 1.5, 3.0};
  double out_point[4];

  albaMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);
  m.MultiplyPoint(in_point, out_point);

  CPPUNIT_ASSERT(albaEquals(out_point[0],3.0));
  CPPUNIT_ASSERT(albaEquals(out_point[1],4.0));
  CPPUNIT_ASSERT(albaEquals(out_point[2],7.5));
  CPPUNIT_ASSERT(albaEquals(out_point[3],3.0));
}
//----------------------------------------------------------------------------
void albaMatrixTest::TestMultiply4x4()
//----------------------------------------------------------------------------
{
  albaMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);

  albaMatrix m1;
  m1.SetElement(0,0,2.0);
  m1.SetElement(1,0,3.0);
  m1.SetElement(2,2,2.0);
  m1.SetElement(3,0,5.0);

  albaMatrix m2;
  m.Multiply4x4(m,m1,m2);

  CPPUNIT_ASSERT(albaEquals(m2.GetElement(0,0), 6.0));
  CPPUNIT_ASSERT(albaEquals(m2.GetElement(1,0), 7.0));
  CPPUNIT_ASSERT(albaEquals(m2.GetElement(2,2), 10.0));
  CPPUNIT_ASSERT(albaEquals(m2.GetElement(3,0), 5.0));
}
//----------------------------------------------------------------------------
void albaMatrixTest::TestPointMultiply()
//----------------------------------------------------------------------------
{
  double matrix[16];
  double in_point[4] = {1.0, 2.0, 1.5, 3.0};
  double out_point[4];

  albaMatrix m;
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

  albaMatrix::PointMultiply(matrix, in_point, out_point);
  CPPUNIT_ASSERT(albaEquals(out_point[0],7.0));
  CPPUNIT_ASSERT(albaEquals(out_point[1],2.0));
  CPPUNIT_ASSERT(albaEquals(out_point[2],7.5));
  CPPUNIT_ASSERT(albaEquals(out_point[3],3.0));
}

//-----------------------------------------------------------
void albaMatrixTest::SetFromDirectionCosinesTest()
{
	double patientOri[6] = {0.0,1.0,2.0,3.0,4.0,5.0};

	double dircos_x = patientOri[1] * patientOri[5] - patientOri[2] * patientOri[4];
	double dircos_y = patientOri[2] * patientOri[3] - patientOri[0] * patientOri[5];
	double dircos_z = patientOri[0] * patientOri[4] - patientOri[1] * patientOri[3];

	albaMatrix matrix;
	matrix.SetFromDirectionCosines(patientOri);

	double Vx0, Vx1, Vx2, Vy0, Vy1, Vy2, Vz0, Vz1, Vz2;

	Vx0 = matrix.GetElement(0, 0);
	Vx1 = matrix.GetElement(1, 0);
	Vx2 = matrix.GetElement(2, 0);
	Vy0 = matrix.GetElement(0, 1);
	Vy1 = matrix.GetElement(1, 1);
	Vy2 = matrix.GetElement(2, 1);
	Vz0 = matrix.GetElement(0, 2);
	Vz1 = matrix.GetElement(1, 2);
	Vz2 = matrix.GetElement(2, 2);

	CPPUNIT_ASSERT(Vx0 == patientOri[0]);
	CPPUNIT_ASSERT(Vx1 == patientOri[1]);
	CPPUNIT_ASSERT(Vx2 == patientOri[2]);
	CPPUNIT_ASSERT(Vy0 == patientOri[3]);
	CPPUNIT_ASSERT(Vy1 == patientOri[4]);
	CPPUNIT_ASSERT(Vy2 == patientOri[5]);
	CPPUNIT_ASSERT(Vz0 == dircos_x);
	CPPUNIT_ASSERT(Vz1 == dircos_y);
	CPPUNIT_ASSERT(Vz2 == dircos_z);
}
