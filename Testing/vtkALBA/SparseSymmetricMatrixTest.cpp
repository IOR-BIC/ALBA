/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)Medical
 Module: SparseSymmetricMatrixTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "SparseSymmetricMatrixTest.h"

#include "vtkMEDPoissonSurfaceReconstruction.h"
#include "vtkMEDPoissonSurfaceReconstruction.cxx"

//-------------------------------------------------------------------------
void SparseSymmetricMatrixTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
    SparseSymmetricMatrix<double> *matrix1 = new SparseSymmetricMatrix<double>();

    CPPUNIT_ASSERT( matrix1->UseAllocator() == false );
    CPPUNIT_ASSERT( matrix1->rows == 0 );

    delete matrix1;
}
//-------------------------------------------------------------------------
void SparseSymmetricMatrixTest::TestStaticAllocation()
//-------------------------------------------------------------------------
{
    SparseSymmetricMatrix<double> matrix1;

    CPPUNIT_ASSERT( matrix1.UseAllocator() == false );
    CPPUNIT_ASSERT( matrix1.rows == 0 );
}
//-------------------------------------------------------------------------
void SparseSymmetricMatrixTest::TestOperatorStar()
//-------------------------------------------------------------------------
{
    SparseSymmetricMatrix<double> matrix;

    matrix.Resize(3);

    matrix.SetRowSize(0,3);
    matrix.SetRowSize(1,3);
    matrix.SetRowSize(2,3);

    matrix.m_ppElements[0][0].Value = 1.;
    matrix.m_ppElements[0][0].N = 0;
    matrix.m_ppElements[1][0].Value = 2.;
    matrix.m_ppElements[1][0].N = 1;
    matrix.m_ppElements[2][0].Value = 0.;
    matrix.m_ppElements[2][0].N = 2;

    matrix.m_ppElements[0][1].Value = 2.;
    matrix.m_ppElements[0][1].N = 0;
    matrix.m_ppElements[1][1].Value = 1.;
    matrix.m_ppElements[1][1].N = 1;
    matrix.m_ppElements[2][1].Value = 0.;
    matrix.m_ppElements[2][1].N = 2;

    matrix.m_ppElements[0][2].Value = 0.;
    matrix.m_ppElements[0][2].N = 0;
    matrix.m_ppElements[1][2].Value = 0.;
    matrix.m_ppElements[1][2].N = 1;
    matrix.m_ppElements[2][2].Value = 1.;
    matrix.m_ppElements[2][2].N = 2;
    Vector<double> v(3);
    v[0] = 1;
    v[1] = 0;
    v[2] = 0;
    Vector<double> result = matrix * v;

    CPPUNIT_ASSERT( result[0] == 6.0 && result[1] == 0. && result[2] == 0. );
}

//-------------------------------------------------------------------------
void SparseSymmetricMatrixTest::TestMultiplyVector1()
//-------------------------------------------------------------------------
{
    SparseSymmetricMatrix<double> matrix;

    matrix.Resize(3);

    matrix.SetRowSize(0,3);
    matrix.SetRowSize(1,3);
    matrix.SetRowSize(2,3);

    matrix.m_ppElements[0][0].Value = 1.;
    matrix.m_ppElements[0][0].N = 0;
    matrix.m_ppElements[1][0].Value = 2.;
    matrix.m_ppElements[1][0].N = 1;
    matrix.m_ppElements[2][0].Value = 0.;
    matrix.m_ppElements[2][0].N = 2;

    matrix.m_ppElements[0][1].Value = 2.;
    matrix.m_ppElements[0][1].N = 0;
    matrix.m_ppElements[1][1].Value = 1.;
    matrix.m_ppElements[1][1].N = 1;
    matrix.m_ppElements[2][1].Value = 0.;
    matrix.m_ppElements[2][1].N = 2;

    matrix.m_ppElements[0][2].Value = 0.;
    matrix.m_ppElements[0][2].N = 0;
    matrix.m_ppElements[1][2].Value = 0.;
    matrix.m_ppElements[1][2].N = 1;
    matrix.m_ppElements[2][2].Value = 1.;
    matrix.m_ppElements[2][2].N = 2;
    Vector<double> v(3);
    v[0] = 1;
    v[1] = 0;
    v[2] = 0;

    Vector<double> result = matrix.Multiply(v);

    CPPUNIT_ASSERT( result[0] == 6.0 && result[1] == 0.0 && result[2] == 0.0 );

}
//-------------------------------------------------------------------------
void SparseSymmetricMatrixTest::TestMultiplyVector2()
//-------------------------------------------------------------------------
{
    SparseSymmetricMatrix<double> matrix;

    matrix.Resize(3);

    matrix.SetRowSize(0,3);
    matrix.SetRowSize(1,3);
    matrix.SetRowSize(2,3);

    matrix.m_ppElements[0][0].Value = 1.;
    matrix.m_ppElements[0][0].N = 0;
    matrix.m_ppElements[1][0].Value = 2.;
    matrix.m_ppElements[1][0].N = 1;
    matrix.m_ppElements[2][0].Value = 0.;
    matrix.m_ppElements[2][0].N = 2;

    matrix.m_ppElements[0][1].Value = 2.;
    matrix.m_ppElements[0][1].N = 0;
    matrix.m_ppElements[1][1].Value = 1.;
    matrix.m_ppElements[1][1].N = 1;
    matrix.m_ppElements[2][1].Value = 0.;
    matrix.m_ppElements[2][1].N = 2;

    matrix.m_ppElements[0][2].Value = 0.;
    matrix.m_ppElements[0][2].N = 0;
    matrix.m_ppElements[1][2].Value = 0.;
    matrix.m_ppElements[1][2].N = 1;
    matrix.m_ppElements[2][2].Value = 1.;
    matrix.m_ppElements[2][2].N = 2;
    Vector<double> v(3);
    v[0] = 1;
    v[1] = 0;
    v[2] = 0;

    Vector<double> result(3);
    matrix.Multiply(v, result);

    CPPUNIT_ASSERT( result[0] == 6.0 && result[1] == 0. && result[2] == 0.0 );

}

//-------------------------------------------------------------------------
void SparseSymmetricMatrixTest::TestSolve1()
//-------------------------------------------------------------------------
{
    SparseSymmetricMatrix<double> matrix;

    matrix.Resize(3);

    matrix.SetRowSize(0,3);
    matrix.SetRowSize(1,3);
    matrix.SetRowSize(2,3);

    matrix.m_ppElements[0][0].Value = 1.;
    matrix.m_ppElements[0][0].N = 0;
    matrix.m_ppElements[1][0].Value = 2.;
    matrix.m_ppElements[1][0].N = 1;
    matrix.m_ppElements[2][0].Value = 0.;
    matrix.m_ppElements[2][0].N = 2;

    matrix.m_ppElements[0][1].Value = 2.;
    matrix.m_ppElements[0][1].N = 0;
    matrix.m_ppElements[1][1].Value = 1.;
    matrix.m_ppElements[1][1].N = 1;
    matrix.m_ppElements[2][1].Value = 0.;
    matrix.m_ppElements[2][1].N = 2;

    matrix.m_ppElements[0][2].Value = 0.;
    matrix.m_ppElements[0][2].N = 0;
    matrix.m_ppElements[1][2].Value = 0.;
    matrix.m_ppElements[1][2].N = 1;
    matrix.m_ppElements[2][2].Value = 1.;
    matrix.m_ppElements[2][2].N = 2;
    
    Vector<double> b(3);
    b[0] = 1;
    b[1] = 2;
    b[2] = 3;

    Vector<double> x(3);

    const int iters = 3;

    const SparseSymmetricMatrix<double> mat = matrix;
    const Vector<double> vec = b;
    SparseSymmetricMatrix<double>::Solve(mat, vec, iters, x);

    CPPUNIT_ASSERT(x[0] < 10.3 && x[1]< 0.6 && x[2] < 0.9 );
}

//-------------------------------------------------------------------------
void SparseSymmetricMatrixTest::TestSolve2()
//-------------------------------------------------------------------------
{
    SparseSymmetricMatrix<double> matrix;

    matrix.Resize(3);

    matrix.SetRowSize(0,3);
    matrix.SetRowSize(1,3);
    matrix.SetRowSize(2,3);

    matrix.m_ppElements[0][0].Value = 1.;
    matrix.m_ppElements[0][0].N = 0;
    matrix.m_ppElements[1][0].Value = 2.;
    matrix.m_ppElements[1][0].N = 1;
    matrix.m_ppElements[2][0].Value = 0.;
    matrix.m_ppElements[2][0].N = 2;

    matrix.m_ppElements[0][1].Value = 2.;
    matrix.m_ppElements[0][1].N = 0;
    matrix.m_ppElements[1][1].Value = 1.;
    matrix.m_ppElements[1][1].N = 1;
    matrix.m_ppElements[2][1].Value = 0.;
    matrix.m_ppElements[2][1].N = 2;

    matrix.m_ppElements[0][2].Value = 0.;
    matrix.m_ppElements[0][2].N = 0;
    matrix.m_ppElements[1][2].Value = 0.;
    matrix.m_ppElements[1][2].N = 1;
    matrix.m_ppElements[2][2].Value = 1.;
    matrix.m_ppElements[2][2].N = 2;

    Vector<double> b(3);
    b[0] = 1;
    b[1] = 2;
    b[2] = 3;

    Vector<double> d(3);
    d[0] = 1.;
    d[1] = 1.;
    d[2] = 1.;

    Vector<double> x(3);

    const int iters = 3;

    const SparseSymmetricMatrix<double> mat = matrix;
    const Vector<double> vec = b;
    const Vector<double> diag = d;
    SparseSymmetricMatrix<double>::Solve(mat, diag, vec, iters, x);

    CPPUNIT_ASSERT(x[0] == 21  && x[1] == 42 && x[2] == 3);
}