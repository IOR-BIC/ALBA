/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMatrixVectorTest
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
#include "albaMatrixVectorTest.h"

#include "albaMatrixVector.h"

//----------------------------------------------------------------------------
void albaMatrixVectorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaMatrixVectorTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaMatrixVector mv;
}
//----------------------------------------------------------------------------
void albaMatrixVectorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaMatrixVector *mv = new albaMatrixVector();
  cppDEL(mv);
}
//----------------------------------------------------------------------------
void albaMatrixVectorTest::TestAppendKeyMatrix()
//----------------------------------------------------------------------------
{
  albaMatrixVector *mv = new albaMatrixVector();
  albaMatrix *matrix1,*matrix2;
  albaMatrix matrix3;
  albaNEW(matrix1);
  albaNEW(matrix2);
  
  mv->AppendKeyMatrix(matrix1);
  mv->AppendKeyMatrix(matrix2);

  // To check the override AppendKeyMatrix(const albaMatrix &m)
  mv->AppendKeyMatrix(matrix3);
  matrix3.Zero();

  CPPUNIT_ASSERT(matrix1==mv->GetKeyMatrix(0) && matrix2==mv->GetKeyMatrix(1));

  albaMatrix *matrix4=mv->GetKeyMatrix(2);

  // The element i,i in matrix3 should be 1 (matrix 3 is an identity) and in matrix4 should be 0
  for (int i = 0; i < 4; i++)
  {
  	CPPUNIT_ASSERT(matrix4->GetElement(i,i)==1 && matrix3.GetElement(i,i)==0);
  }

  albaDEL(matrix1);
  albaDEL(matrix2);
  cppDEL(mv);
}
//----------------------------------------------------------------------------
void albaMatrixVectorTest::TestGetNumberOfMatrices()
//----------------------------------------------------------------------------
{
  albaMatrixVector *mv = new albaMatrixVector();
  albaMatrix *matrix1,*matrix2;
  albaMatrix matrix3;
  albaNEW(matrix1);
  albaNEW(matrix2);
  
  matrix2->SetTimeStamp(1.0);

  mv->AppendKeyMatrix(matrix1);
  mv->AppendKeyMatrix(matrix2);

  // To check the override AppendKeyMatrix(const albaMatrix &m)
  mv->AppendKeyMatrix(matrix3);

  // Only 2 Matrices because matrix1 and matrix3 have the same timeStamp
  CPPUNIT_ASSERT(mv->GetNumberOfMatrices() == 2);

  albaDEL(matrix1);
  albaDEL(matrix2);
  cppDEL(mv);
}
//----------------------------------------------------------------------------
void albaMatrixVectorTest::TestGetNearestMatrix()
//----------------------------------------------------------------------------
{
  albaMatrixVector *mv = new albaMatrixVector();
  albaMatrix *matrix1,*matrix2;
  albaMatrix matrix3;
  albaNEW(matrix1);
  albaNEW(matrix2);

  matrix2->SetTimeStamp(1.0);
  matrix3.SetTimeStamp(2.0);

  mv->AppendKeyMatrix(matrix1);
  mv->AppendKeyMatrix(matrix2);

  // To check the override AppendKeyMatrix(const albaMatrix &m)
  mv->AppendKeyMatrix(matrix3);

  CPPUNIT_ASSERT(mv->GetNearestMatrix(0.8) == matrix2);

  albaDEL(matrix1);
  albaDEL(matrix2);
  cppDEL(mv);
}
//----------------------------------------------------------------------------
void albaMatrixVectorTest::TestGetMatrixBefore()
//----------------------------------------------------------------------------
{
  albaMatrixVector *mv = new albaMatrixVector();
  albaMatrix *matrix1,*matrix2;
  albaMatrix matrix3;
  albaNEW(matrix1);
  albaNEW(matrix2);

  matrix2->SetTimeStamp(1.0);
  matrix3.SetTimeStamp(2.0);

  mv->AppendKeyMatrix(matrix1);
  mv->AppendKeyMatrix(matrix2);

  // To check the override AppendKeyMatrix(const albaMatrix &m)
  mv->AppendKeyMatrix(matrix3);

  CPPUNIT_ASSERT(mv->GetMatrixBefore(0.8) == matrix1);

  albaDEL(matrix1);
  albaDEL(matrix2);
  cppDEL(mv);
}
//----------------------------------------------------------------------------
void albaMatrixVectorTest::TestGetMatrix()
//----------------------------------------------------------------------------
{
  albaMatrixVector *mv = new albaMatrixVector();
  albaMatrix *matrix1,*matrix2;
  albaMatrix matrix3;
  albaNEW(matrix1);
  albaNEW(matrix2);

  matrix2->SetTimeStamp(1.0);
  matrix3.SetTimeStamp(2.0);

  mv->AppendKeyMatrix(matrix1);
  mv->AppendKeyMatrix(matrix2);

  // To check the override AppendKeyMatrix(const albaMatrix &m)
  mv->AppendKeyMatrix(matrix3);

  CPPUNIT_ASSERT(mv->GetMatrix(0.8) == NULL);
  CPPUNIT_ASSERT(mv->GetMatrix(0.0) == matrix1);
  CPPUNIT_ASSERT(mv->GetMatrix(1.0) == matrix2);

  albaDEL(matrix1);
  albaDEL(matrix2);
  cppDEL(mv);
}
//----------------------------------------------------------------------------
void albaMatrixVectorTest::TestSetMatrix()
//----------------------------------------------------------------------------
{
  albaMatrixVector *mv = new albaMatrixVector();
  albaMatrix *matrix1,*matrix2;
  albaMatrix matrixOfZero;

  albaNEW(matrix2);
  matrix2->SetTimeStamp(1.0);
  mv->AppendKeyMatrix(matrix2);

  matrixOfZero.Zero();
  matrixOfZero.SetTimeStamp(2.0);
  // To check the override SetMatrix(const albaMatrix &m)
  mv->SetMatrix(matrixOfZero);

  albaNEW(matrix1);
  // Set matrix1 with timestamp 0.0 , so a new key matrix is inserted
  mv->SetMatrix(matrix1);

  CPPUNIT_ASSERT(mv->GetMatrix(0.0) == matrix1);
  CPPUNIT_ASSERT(mv->GetMatrix(1.0) == matrix2);

  for (int i = 0; i < 4; i++)
  {
    // Check if matrixOfZero has been inserted
    CPPUNIT_ASSERT(mv->GetMatrix(2.0)->GetElement(i,i)==0);
  }

  albaMatrix *matrixOfTwo;
  albaNEW(matrixOfTwo);

  for (int i = 0; i < 4; i++)
  {
    // Create a matrix with "2" in diagonal elements
    matrixOfTwo->SetElement(i,i,2.0);
  }
  matrixOfTwo->SetTimeStamp(1.0);

  mv->SetMatrix(matrixOfTwo);

  // matrixOfTwo has the same timestamp of matrix2 so it should replace matrix2
  CPPUNIT_ASSERT(mv->GetNumberOfMatrices()==3);

  // In timestamp 1.0 should be matrixOfTwo
  CPPUNIT_ASSERT(mv->GetMatrix(1.0)==matrixOfTwo);

  albaMatrix matrixOfThree;

  for (int i = 0; i < 4; i++)
  {
    // Create a matrix with "3" in diagonal elements
    matrixOfThree.SetElement(i,i,3.0);
  }
  matrixOfThree.SetTimeStamp(0.0);

  // To check the override SetMatrix(const albaMatrix &m)
  mv->SetMatrix(matrixOfThree);

  for (int i = 0; i < 4; i++)
  {
    // In timestamp 0.0 should be matrixOfThree
    CPPUNIT_ASSERT(mv->GetMatrix(0.0)->GetElement(i,i)==3.0);
  }


  albaDEL(matrixOfTwo);
  albaDEL(matrix1);
  albaDEL(matrix2);
  cppDEL(mv);
}
