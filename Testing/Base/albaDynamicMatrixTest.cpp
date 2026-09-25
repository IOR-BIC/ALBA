/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDynamicMatrixTest
 Authors: Gianluigi Crimi
 
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

#include "albaDynamicMatrixTest.h"
#include "albaDynamicMatrix.h"
#include "albaString.h"


#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//matrix test 
//0.0 0.1 0.2 0.3
//1.0 1.1 1.2 1.3
//2.0 2.1 2.2 2.3
//3.0 3.1 3.2 3.3

//----------------------------------------------------------------------------
void albaDynamicMatrixTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//-----------------------------------------------------------
void albaDynamicMatrixTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaDynamicMatrix *rMIU=new albaDynamicMatrix();
  delete rMIU;
}
//-----------------------------------------------------------
void albaDynamicMatrixTest::TestReadMatrix() 
//-----------------------------------------------------------
{
 
  albaString matrixFile = ALBA_DATA_ROOT;
  matrixFile << "/Matrix/TestMatrix001.txt";
  albaDynamicMatrix mat;
  int res = mat.ReadFromFile(matrixFile);

  CPPUNIT_ASSERT( res == ALBA_OK );
  CPPUNIT_ASSERT( mat.GetColsNum() == 4 && mat.GetColsNum() == 4 );
  
  CPPUNIT_ASSERT(mat(0, 0) == 0.0 && mat(0, 1) == 0.1 && mat(0, 2) == 0.2 && mat(0, 3) == 0.3);
  CPPUNIT_ASSERT(mat(1, 0) == 1.0 && mat(1, 1) == 1.1 && mat(1, 2) == 1.2 && mat(1, 3) == 1.3);
  CPPUNIT_ASSERT(mat(2, 0) == 2.0 && mat(2, 1) == 2.1 && mat(2, 2) == 2.2 && mat(2, 3) == 2.3);
  CPPUNIT_ASSERT(mat(3, 0) == 3.0 && mat(3, 1) == 3.1 && mat(3, 2) == 3.2 && mat(3, 3) == 3.3);
  
}

//----------------------------------------------------------------------------
void albaDynamicMatrixTest::TestBigFileRead()
{
  albaDynamicMatrix matrix;

	albaString matrixFile = ALBA_DATA_ROOT;
  matrixFile << "/RAW_MAL/Fprg3bsi.man";

  int result=matrix.ReadFromFile(matrixFile);

  CPPUNIT_ASSERT( result == ALBA_OK );

}
