/*=========================================================================

 Program: MAF2
 Module: vtkMAFDOFMatrixTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>

#include "vtkMAFDOFMatrixTest.h"

#include "vtkMAFDOFMatrix.h"
#include "vtkDoubleArray.h"

void vtkMAFDOFMatrixTest::TestFixture()
{

}

void vtkMAFDOFMatrixTest::TestConstructorDestructor()
{
  vtkMAFDOFMatrix *source = vtkMAFDOFMatrix::New();
  source->Delete();
}

void vtkMAFDOFMatrixTest::TestDeepCopy()
{
  vtkMAFDOFMatrix *source = vtkMAFDOFMatrix::New();

  source->SetState(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X, vtkMAFDOFMatrix::SNAP_STEP);
  source->SetState(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::Y, vtkMAFDOFMatrix::SNAP_ARRAY);
  
  vtkMAFDOFMatrix *target = vtkMAFDOFMatrix::New();
 
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

void vtkMAFDOFMatrixTest::TestReset()
{
  vtkMAFDOFMatrix *matrix = vtkMAFDOFMatrix::New();
  
  matrix->SetState(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X, vtkMAFDOFMatrix::SNAP_STEP);

  matrix->Reset();

  int state = matrix->GetState(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X);
  CPPUNIT_ASSERT(state == vtkMAFDOFMatrix::LOCK);

  matrix->Delete();

}

void vtkMAFDOFMatrixTest::TestSetGetState()
{
  vtkMAFDOFMatrix *matrix = vtkMAFDOFMatrix::New();

  matrix->SetState(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X, vtkMAFDOFMatrix::SNAP_STEP);
  int state = matrix->GetState(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X);
  CPPUNIT_ASSERT(state == vtkMAFDOFMatrix::SNAP_STEP);

  matrix->Delete();

}

void vtkMAFDOFMatrixTest::TestSetGetLowerBound()
{
  vtkMAFDOFMatrix *matrix = vtkMAFDOFMatrix::New();

  matrix->SetLowerBound(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X, 5);
  int lowerBound = matrix->GetLowerBound(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X);
  CPPUNIT_ASSERT(lowerBound == 5);

  matrix->Delete();

}

void vtkMAFDOFMatrixTest::TestSetGetUpperBound()
{
  vtkMAFDOFMatrix *matrix = vtkMAFDOFMatrix::New();

  matrix->SetLowerBound(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X, 5);
  int lowerBound = matrix->GetLowerBound(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X);
  CPPUNIT_ASSERT(lowerBound == 5);

  matrix->Delete();

}

void vtkMAFDOFMatrixTest::TestSetGetMin()
{
  vtkMAFDOFMatrix *matrix = vtkMAFDOFMatrix::New();

  matrix->SetMin(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X, 5);
  int minimum = matrix->GetMin(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X);
  CPPUNIT_ASSERT(minimum == 5);

  matrix->Delete();
}

void vtkMAFDOFMatrixTest::TestSetGetMax()
{
  vtkMAFDOFMatrix *matrix = vtkMAFDOFMatrix::New();

  matrix->SetMax(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X, 5);
  int minimum = matrix->GetMax(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X);
  CPPUNIT_ASSERT(minimum == 5);

  matrix->Delete();
}

void vtkMAFDOFMatrixTest::TestSetGetStep()
{  
  vtkMAFDOFMatrix *matrix = vtkMAFDOFMatrix::New();

  matrix->SetStep(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X, 5);
  int step = matrix->GetStep(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X);
  CPPUNIT_ASSERT(step == 5);

  matrix->Delete();
}

void vtkMAFDOFMatrixTest::TestSetGetArray()
{
  vtkMAFDOFMatrix *matrix = vtkMAFDOFMatrix::New();
  
  vtkDoubleArray *inputArray = vtkDoubleArray::New();

  matrix->SetArray(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X, inputArray);
  vtkDoubleArray *outputArray = matrix->GetArray(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X);
  CPPUNIT_ASSERT(outputArray != NULL);
  CPPUNIT_ASSERT(outputArray == inputArray);
  inputArray->Delete();
  matrix->Delete(); 
}

void vtkMAFDOFMatrixTest::TestGetDOFNumber()
{
  vtkMAFDOFMatrix *matrix = vtkMAFDOFMatrix::New();
  
  matrix->SetState(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X, vtkMAFDOFMatrix::FREE);
  matrix->SetState(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::Y, vtkMAFDOFMatrix::FREE);
  int dofNumberX = matrix->GetDOFNumber(vtkMAFDOFMatrix::TRANSLATE);

  CPPUNIT_ASSERT(dofNumberX == 2);

  matrix->Delete(); 
}

void vtkMAFDOFMatrixTest::TestGetConstrainAxis()
{
  vtkMAFDOFMatrix *matrix = vtkMAFDOFMatrix::New();

  matrix->SetState(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X, vtkMAFDOFMatrix::FREE);
  matrix->SetState(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::Y, vtkMAFDOFMatrix::FREE);
  int constrainAxis = matrix->GetConstrainAxis(vtkMAFDOFMatrix::TRANSLATE);

  CPPUNIT_ASSERT(constrainAxis == -1);
  
  matrix->SetState(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X, vtkMAFDOFMatrix::LOCK);
  constrainAxis = matrix->GetConstrainAxis(vtkMAFDOFMatrix::TRANSLATE);
  
  CPPUNIT_ASSERT(constrainAxis == vtkMAFDOFMatrix::Y);

  matrix->Delete(); 
}

void vtkMAFDOFMatrixTest::TestGetConstrainPlane()
{
  vtkMAFDOFMatrix *matrix = vtkMAFDOFMatrix::New();

  matrix->SetState(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X, vtkMAFDOFMatrix::FREE);
  matrix->SetState(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::Y, vtkMAFDOFMatrix::FREE);
  int constrainPlane = matrix->GetConstrainPlane(vtkMAFDOFMatrix::TRANSLATE);

  CPPUNIT_ASSERT(constrainPlane == vtkMAFDOFMatrix::XY);

  matrix->SetState(vtkMAFDOFMatrix::TRANSLATE, vtkMAFDOFMatrix::X, vtkMAFDOFMatrix::LOCK);
  constrainPlane = matrix->GetConstrainPlane(vtkMAFDOFMatrix::TRANSLATE);

  CPPUNIT_ASSERT(constrainPlane == -1);

  matrix->Delete(); 
  
}