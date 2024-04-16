/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)Medical
 Module: SparseNMatrixTest
 Authors: Stefano Perticoni
 
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
#include "SparseNMatrixTest.h"

#include "vtkMEDPoissonSurfaceReconstruction.h"
#include "vtkMEDPoissonSurfaceReconstruction.cxx"

//-------------------------------------------------------------------------
void SparseNMatrixTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  SparseNMatrix<double, 3> *matrix1 = new SparseNMatrix<double, 3>();

  CPPUNIT_ASSERT( matrix1->UseAllocator() == false );
  CPPUNIT_ASSERT( matrix1->rows == 0 );

  delete matrix1;

  SparseNMatrix<double , 3> *matrix2 = new SparseNMatrix<double,3>(5);

  CPPUNIT_ASSERT( matrix2->UseAllocator() == false );
  CPPUNIT_ASSERT( matrix2->rows == 5 );

  delete matrix2;
}

//-------------------------------------------------------------------------
void SparseNMatrixTest::TestStaticAllocation()
//-------------------------------------------------------------------------
{
  SparseNMatrix<double, 10> matrix1;

  CPPUNIT_ASSERT( matrix1.UseAllocator() == false );
  CPPUNIT_ASSERT( matrix1.rows == 0 );

  SparseNMatrix<double, 10> matrix2 = SparseNMatrix<double, 10>(5);

  CPPUNIT_ASSERT( matrix2.UseAllocator() == false );
  CPPUNIT_ASSERT( matrix2.rows == 5 );
 
}

//-------------------------------------------------------------------------
void SparseNMatrixTest::TestSetAllocator()
//-------------------------------------------------------------------------
{
	SparseNMatrix<double,10> matrix;

	matrix.SetAllocator(1000);

	CPPUNIT_ASSERT( matrix.UseAllocator() == TRUE );
}

//-------------------------------------------------------------------------
void SparseNMatrixTest::TestResize()
//-------------------------------------------------------------------------
{
	SparseNMatrix<double,10> matrix;

	matrix.Resize(100);

	CPPUNIT_ASSERT( matrix.rows == 100 );
}

//-------------------------------------------------------------------------
void SparseNMatrixTest::TestSetRowSize()
//-------------------------------------------------------------------------
{
	SparseNMatrix<double,10> matrix;

	matrix.Resize(100);
	matrix.SetRowSize(5,20);

	CPPUNIT_ASSERT( matrix.rowSizes[5] == 20 );

	matrix.SetRowSize(10,15);
	
	CPPUNIT_ASSERT( matrix.rowSizes[10] == 15 );
}


