/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBADOFMatrixTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include <cppunit/config/SourcePrefix.h>

#include "vtkALBADOFMatrixTest.h"

#include "vtkALBADOFMatrix.h"
#include "vtkDoubleArray.h"

void vtkALBADOFMatrixTest::TestFixture()
{

}

void vtkALBADOFMatrixTest::TestConstructorDestructor()
{
  vtkALBADOFMatrix *source = vtkALBADOFMatrix::New();
  source->Delete();
}

void vtkALBADOFMatrixTest::TestDeepCopy()
{
  vtkALBADOFMatrix *source = vtkALBADOFMatrix::New();

  source->SetState(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X, vtkALBADOFMatrix::SNAP_STEP);
  source->SetState(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::Y, vtkALBADOFMatrix::SNAP_ARRAY);
  
  vtkALBADOFMatrix *target = vtkALBADOFMatrix::New();
 
  target->DeepCopy(source);

  int i, j;

  for (i = 0; i < 3; i++)
  {
    for (j = 0; j < 3; j++)
    {
      CPPUNIT_ASSERT(target->GetState(i,j) == source->GetState(i, j));
    }
  }

  source->Delete();
  target->Delete();

}

void vtkALBADOFMatrixTest::TestReset()
{
  vtkALBADOFMatrix *matrix = vtkALBADOFMatrix::New();
  
  matrix->SetState(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X, vtkALBADOFMatrix::SNAP_STEP);

  matrix->Reset();

  int state = matrix->GetState(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X);
  CPPUNIT_ASSERT(state == vtkALBADOFMatrix::LOCK);

  matrix->Delete();

}

void vtkALBADOFMatrixTest::TestSetGetState()
{
  vtkALBADOFMatrix *matrix = vtkALBADOFMatrix::New();

  matrix->SetState(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X, vtkALBADOFMatrix::SNAP_STEP);
  int state = matrix->GetState(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X);
  CPPUNIT_ASSERT(state == vtkALBADOFMatrix::SNAP_STEP);

  matrix->Delete();

}

void vtkALBADOFMatrixTest::TestSetGetLowerBound()
{
  vtkALBADOFMatrix *matrix = vtkALBADOFMatrix::New();

  matrix->SetLowerBound(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X, 5);
  int lowerBound = matrix->GetLowerBound(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X);
  CPPUNIT_ASSERT(lowerBound == 5);

  matrix->Delete();

}

void vtkALBADOFMatrixTest::TestSetGetUpperBound()
{
  vtkALBADOFMatrix *matrix = vtkALBADOFMatrix::New();

  matrix->SetLowerBound(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X, 5);
  int lowerBound = matrix->GetLowerBound(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X);
  CPPUNIT_ASSERT(lowerBound == 5);

  matrix->Delete();

}

void vtkALBADOFMatrixTest::TestSetGetMin()
{
  vtkALBADOFMatrix *matrix = vtkALBADOFMatrix::New();

  matrix->SetMin(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X, 5);
  int minimum = matrix->GetMin(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X);
  CPPUNIT_ASSERT(minimum == 5);

  matrix->Delete();
}

void vtkALBADOFMatrixTest::TestSetGetMax()
{
  vtkALBADOFMatrix *matrix = vtkALBADOFMatrix::New();

  matrix->SetMax(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X, 5);
  int minimum = matrix->GetMax(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X);
  CPPUNIT_ASSERT(minimum == 5);

  matrix->Delete();
}

void vtkALBADOFMatrixTest::TestSetGetStep()
{  
  vtkALBADOFMatrix *matrix = vtkALBADOFMatrix::New();

  matrix->SetStep(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X, 5);
  int step = matrix->GetStep(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X);
  CPPUNIT_ASSERT(step == 5);

  matrix->Delete();
}

void vtkALBADOFMatrixTest::TestSetGetArray()
{
  vtkALBADOFMatrix *matrix = vtkALBADOFMatrix::New();
  
  vtkDoubleArray *inputArray = vtkDoubleArray::New();

  matrix->SetArray(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X, inputArray);
  vtkDoubleArray *outputArray = matrix->GetArray(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X);
  CPPUNIT_ASSERT(outputArray != NULL);
  CPPUNIT_ASSERT(outputArray == inputArray);
  inputArray->Delete();
  matrix->Delete(); 
}

void vtkALBADOFMatrixTest::TestGetDOFNumber()
{
  vtkALBADOFMatrix *matrix = vtkALBADOFMatrix::New();
  
  matrix->SetState(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X, vtkALBADOFMatrix::FREE);
  matrix->SetState(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::Y, vtkALBADOFMatrix::FREE);
  int dofNumberX = matrix->GetDOFNumber(vtkALBADOFMatrix::TRANSLATE);

  CPPUNIT_ASSERT(dofNumberX == 2);

  matrix->Delete(); 
}

void vtkALBADOFMatrixTest::TestGetConstrainAxis()
{
  vtkALBADOFMatrix *matrix = vtkALBADOFMatrix::New();

  matrix->SetState(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X, vtkALBADOFMatrix::FREE);
  matrix->SetState(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::Y, vtkALBADOFMatrix::FREE);
  int constrainAxis = matrix->GetConstrainAxis(vtkALBADOFMatrix::TRANSLATE);

  CPPUNIT_ASSERT(constrainAxis == -1);
  
  matrix->SetState(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X, vtkALBADOFMatrix::LOCK);
  constrainAxis = matrix->GetConstrainAxis(vtkALBADOFMatrix::TRANSLATE);
  
  CPPUNIT_ASSERT(constrainAxis == vtkALBADOFMatrix::Y);

  matrix->Delete(); 
}

void vtkALBADOFMatrixTest::TestGetConstrainPlane()
{
  vtkALBADOFMatrix *matrix = vtkALBADOFMatrix::New();

  matrix->SetState(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X, vtkALBADOFMatrix::FREE);
  matrix->SetState(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::Y, vtkALBADOFMatrix::FREE);
  int constrainPlane = matrix->GetConstrainPlane(vtkALBADOFMatrix::TRANSLATE);

  CPPUNIT_ASSERT(constrainPlane == vtkALBADOFMatrix::XY);

  matrix->SetState(vtkALBADOFMatrix::TRANSLATE, vtkALBADOFMatrix::X, vtkALBADOFMatrix::LOCK);
  constrainPlane = matrix->GetConstrainPlane(vtkALBADOFMatrix::TRANSLATE);

  CPPUNIT_ASSERT(constrainPlane == -1);

  matrix->Delete(); 
  
}