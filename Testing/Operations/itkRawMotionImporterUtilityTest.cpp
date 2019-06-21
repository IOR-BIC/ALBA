/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaItkRawMotionImporterUtilityTest
 Authors: Daniele Giunchi
 
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

#include "itkRawMotionImporterUtilityTest.h"
#include "albaItkRawMotionImporterUtility.h"
#include "albaString.h"

#include "vnl/vnl_matrix.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//matrix test 
//0.0 0.1 0.2 0.3
//1.0 1.1 1.2 1.3
//2.0 2.1 2.2 2.3
//3.0 3.1 3.2 3.3

//----------------------------------------------------------------------------
void albaItkRawMotionImporterUtilityTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//-----------------------------------------------------------
void albaItkRawMotionImporterUtilityTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaItkRawMotionImporterUtility *rMIU=new albaItkRawMotionImporterUtility();
  delete rMIU;
}
//-----------------------------------------------------------
void albaItkRawMotionImporterUtilityTest::TestReadMatrix() 
//-----------------------------------------------------------
{
  enum ID_RESULT
  {
    RES_OK = 0,
    RES_ERROR = 1,
  };

  albaString matrixFile = ALBA_DATA_ROOT;
  matrixFile << "/Matrix/TestMatrix001.txt";
  vnl_matrix<double> mat;
  albaItkRawMotionImporterUtility *rMIU=new albaItkRawMotionImporterUtility();
  int res = rMIU->ReadMatrix(mat,matrixFile);

  m_Result = res == RES_OK;
  TEST_RESULT;

  m_Result = mat.columns() == 4 && mat.rows() == 4;
  TEST_RESULT;

  m_Result = mat.get(0,0) == 0.0 && 
             mat.get(0,1) == 0.1 &&
             mat.get(0,2) == 0.2 &&
             mat.get(0,3) == 0.3 &&

             mat.get(1,0) == 1.0 &&
             mat.get(1,1) == 1.1 &&
             mat.get(1,2) == 1.2 &&
             mat.get(1,3) == 1.3 &&

             mat.get(2,0) == 2.0 &&
             mat.get(2,1) == 2.1 &&
             mat.get(2,2) == 2.2 &&
             mat.get(2,3) == 2.3 &&

             mat.get(3,0) == 3.0 &&
             mat.get(3,1) == 3.1 &&
             mat.get(3,2) == 3.2 &&
             mat.get(3,3) == 3.3;
  TEST_RESULT;

  delete rMIU;
}
