/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: SparseMatrixTest
 Authors: Matteo Giacomoni
 
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
#include "SparseMatrixTest.h"

#include "vtkALBAPoissonSurfaceReconstruction.h"

//-------------------------------------------------------------------------
void SparseMatrixTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  SparseMatrix<double> *matrix1 = new SparseMatrix<double>();

  CPPUNIT_ASSERT( matrix1->UseAllocator() == false );
  CPPUNIT_ASSERT( matrix1->rows == 0 );

  delete matrix1;

  SparseMatrix<double> *matrix2 = new SparseMatrix<double>(5);

  CPPUNIT_ASSERT( matrix2->UseAllocator() == false );
  CPPUNIT_ASSERT( matrix2->rows == 5 );

  delete matrix2;
}
//-------------------------------------------------------------------------
void SparseMatrixTest::TestStaticAllocation()
//-------------------------------------------------------------------------
{
  SparseMatrix<double> matrix1;

  CPPUNIT_ASSERT( matrix1.UseAllocator() == false );
  CPPUNIT_ASSERT( matrix1.rows == 0 );

  SparseMatrix<double> matrix2 = SparseMatrix<double>(5);

  CPPUNIT_ASSERT( matrix2.UseAllocator() == false );
  CPPUNIT_ASSERT( matrix2.rows == 5 );
}
//-------------------------------------------------------------------------
void SparseMatrixTest::TestSetAllocator()
//-------------------------------------------------------------------------
{
  SparseMatrix<double> matrix;

  matrix.SetAllocator(1000);

  CPPUNIT_ASSERT( matrix.UseAllocator() == true );
}
//-------------------------------------------------------------------------
void SparseMatrixTest::TestResize()
//-------------------------------------------------------------------------
{
  SparseMatrix<double> matrix;

  matrix.Resize(100);

  CPPUNIT_ASSERT( matrix.rows == 100 );
}
//-------------------------------------------------------------------------
void SparseMatrixTest::TestSetRowSize()
//-------------------------------------------------------------------------
{
  SparseMatrix<double> matrix;

  matrix.Resize(100);
  matrix.SetRowSize(5,20);

  CPPUNIT_ASSERT( matrix.rowSizes[5] == 20 );

  matrix.SetRowSize(10,15);

  CPPUNIT_ASSERT( matrix.rowSizes[10] == 15 );
}
//-------------------------------------------------------------------------
void SparseMatrixTest::TestSetZero()
//-------------------------------------------------------------------------
{
  SparseMatrix<double> matrix;
  
  //NOT IMPLEMENTED IN THE CLASS
  // matrix.SetZero();
}
//-------------------------------------------------------------------------
void SparseMatrixTest::TestSetIdentity()
//-------------------------------------------------------------------------
{
  SparseMatrix<double> matrix;

  //NOT IMPLEMENTED IN THE CLASS
  // matrix.SetIdentity();
}
//-------------------------------------------------------------------------
void SparseMatrixTest::TestOperator1()
//-------------------------------------------------------------------------
{
  SparseMatrix<double> matrix;

  matrix.Resize(3);

  matrix.SetRowSize(0,3);
  matrix.SetRowSize(1,3);
  matrix.SetRowSize(2,3);

  matrix.m_ppElements[0][0].Value = 1;
  matrix.m_ppElements[1][0].Value = 2;
  matrix.m_ppElements[2][0].Value = 3;

  matrix.m_ppElements[0][1].Value = 4;
  matrix.m_ppElements[1][1].Value = 5;
  matrix.m_ppElements[2][1].Value = 6;

  matrix.m_ppElements[0][2].Value = 7;
  matrix.m_ppElements[1][2].Value = 8;
  matrix.m_ppElements[2][2].Value = 9;

  SparseMatrix<double> result = matrix*5;

  int count = 1;
  for (int i=0;i<3;i++)
  {
    for (int j=0;j<3;j++)
    {
      CPPUNIT_ASSERT(result.m_ppElements[j][i].Value == (count*5.0));
      count++;
    }
  }
}
//-------------------------------------------------------------------------
void SparseMatrixTest::TestOperator2()
//-------------------------------------------------------------------------
{
  SparseMatrix<double> matrix;

  matrix.Resize(3);

  matrix.SetRowSize(0,3);
  matrix.SetRowSize(1,3);
  matrix.SetRowSize(2,3);

  matrix.m_ppElements[0][0].Value = 1;
  matrix.m_ppElements[1][0].Value = 2;
  matrix.m_ppElements[2][0].Value = 3;

  matrix.m_ppElements[0][1].Value = 4;
  matrix.m_ppElements[1][1].Value = 5;
  matrix.m_ppElements[2][1].Value = 6;

  matrix.m_ppElements[0][2].Value = 7;
  matrix.m_ppElements[1][2].Value = 8;
  matrix.m_ppElements[2][2].Value = 9;

  matrix*=5;

  int count = 1;
  for (int i=0;i<3;i++)
  {
    for (int j=0;j<3;j++)
    {
      CPPUNIT_ASSERT(matrix.m_ppElements[j][i].Value == (count*5.0));
      count++;
    }
  }
}
//-------------------------------------------------------------------------
void SparseMatrixTest::TestOperator3()
//-------------------------------------------------------------------------
{
  SparseMatrix<double> matrix1;

  matrix1.Resize(3);

  matrix1.SetRowSize(0,3);
  matrix1.SetRowSize(1,3);
  matrix1.SetRowSize(2,3);

  matrix1.m_ppElements[0][0].Value = 1;
  matrix1.m_ppElements[1][0].Value = 2;
  matrix1.m_ppElements[2][0].Value = 3;

  matrix1.m_ppElements[0][1].Value = 4;
  matrix1.m_ppElements[1][1].Value = 5;
  matrix1.m_ppElements[2][1].Value = 6;

  matrix1.m_ppElements[0][2].Value = 7;
  matrix1.m_ppElements[1][2].Value = 8;
  matrix1.m_ppElements[2][2].Value = 9;

  SparseMatrix<double> matrix2;

  matrix2.Resize(3);

  matrix2.SetRowSize(0,3);
  matrix2.SetRowSize(1,3);
  matrix2.SetRowSize(2,3);

  matrix2.m_ppElements[0][0].Value = 1;
  matrix2.m_ppElements[1][0].Value = 2;
  matrix2.m_ppElements[2][0].Value = 3;

  matrix2.m_ppElements[0][1].Value = 4;
  matrix2.m_ppElements[1][1].Value = 5;
  matrix2.m_ppElements[2][1].Value = 6;

  matrix2.m_ppElements[0][2].Value = 7;
  matrix2.m_ppElements[1][2].Value = 8;
  matrix2.m_ppElements[2][2].Value = 9;

  //NOT IMPLEMENTED IN THE CLASS
  // SparseMatrix<double> result = matrix1 * matrix2;
}
//-------------------------------------------------------------------------
void SparseMatrixTest::TestMultiplyMatrix()
//-------------------------------------------------------------------------
{
  //NOT IMPLEMENTED IN THE CLASS
}
//-------------------------------------------------------------------------
void SparseMatrixTest::TestMultiplyTransposeMatrix()
//-------------------------------------------------------------------------
{
  //NOT IMPLEMENTED IN THE CLASS
}
//-------------------------------------------------------------------------
void SparseMatrixTest::TestMultiplyVector1()
//-------------------------------------------------------------------------
{
  SparseMatrix<double> matrix1;

  matrix1.Resize(3);

  matrix1.SetRowSize(0,3);
  matrix1.SetRowSize(1,3);
  matrix1.SetRowSize(2,3);

  matrix1.m_ppElements[0][0].Value = 1;
  matrix1.m_ppElements[0][0].N = 0;
  matrix1.m_ppElements[1][0].Value = 2;
  matrix1.m_ppElements[1][0].N = 1;
  matrix1.m_ppElements[2][0].Value = 3;
  matrix1.m_ppElements[2][0].N = 2;

  matrix1.m_ppElements[0][1].Value = 4;
  matrix1.m_ppElements[0][1].N = 0;
  matrix1.m_ppElements[1][1].Value = 5;
  matrix1.m_ppElements[1][1].N = 1;
  matrix1.m_ppElements[2][1].Value = 6;
  matrix1.m_ppElements[2][1].N = 2;

  matrix1.m_ppElements[0][2].Value = 7;
  matrix1.m_ppElements[0][2].N = 0;
  matrix1.m_ppElements[1][2].Value = 8;
  matrix1.m_ppElements[1][2].N = 1;
  matrix1.m_ppElements[2][2].Value = 9;
  matrix1.m_ppElements[2][2].N = 2;

  Vector<double> v(3);
  v[0] = 1;
  v[1] = 2;
  v[2] = 3;

  Vector<double> result = matrix1.Multiply(v);

  CPPUNIT_ASSERT( result[0] == 12.0 && result[1] == 30.0 && result[2] == 54.0 );

}
//-------------------------------------------------------------------------
void SparseMatrixTest::TestMultiplyVector2()
//-------------------------------------------------------------------------
{
  SparseMatrix<double> matrix1;

  matrix1.Resize(3);

  matrix1.SetRowSize(0,3);
  matrix1.SetRowSize(1,3);
  matrix1.SetRowSize(2,3);

  matrix1.m_ppElements[0][0].Value = 1;
  matrix1.m_ppElements[0][0].N = 0;
  matrix1.m_ppElements[1][0].Value = 2;
  matrix1.m_ppElements[1][0].N = 1;
  matrix1.m_ppElements[2][0].Value = 3;
  matrix1.m_ppElements[2][0].N = 2;

  matrix1.m_ppElements[0][1].Value = 4;
  matrix1.m_ppElements[0][1].N = 0;
  matrix1.m_ppElements[1][1].Value = 5;
  matrix1.m_ppElements[1][1].N = 1;
  matrix1.m_ppElements[2][1].Value = 6;
  matrix1.m_ppElements[2][1].N = 2;

  matrix1.m_ppElements[0][2].Value = 7;
  matrix1.m_ppElements[0][2].N = 0;
  matrix1.m_ppElements[1][2].Value = 8;
  matrix1.m_ppElements[1][2].N = 1;
  matrix1.m_ppElements[2][2].Value = 9;
  matrix1.m_ppElements[2][2].N = 2;

  Vector<double> v(3);
  v[0] = 1;
  v[1] = 2;
  v[2] = 3;

  Vector<double> result(3);
  matrix1.Multiply(v,result);

  CPPUNIT_ASSERT( result[0] == 12.0 && result[1] == 30.0 && result[2] == 54.0 );

}
//-------------------------------------------------------------------------
void SparseMatrixTest::TestTranspose()
//-------------------------------------------------------------------------
{
  //NOT IMPLEMENTED IN THE CLASS
}
//-------------------------------------------------------------------------
void SparseMatrixTest::TestSolve()
//-------------------------------------------------------------------------
{
  //NOT IMPLEMENTED IN THE CLASS
}