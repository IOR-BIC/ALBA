/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorConstraintTest
 Authors: Stefano Perticoni
 
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

#include "albaInteractorConstraintTest.h"
#include "vtkDoubleArray.h"
#include <cppunit/config/SourcePrefix.h>
#include <iostream>


void albaInteractorConstraintTest::TestFixture()
{

}

void albaInteractorConstraintTest::TestConstructor()
{
  albaInteractorConstraint *constraint = new albaInteractorConstraint;
  cppDEL(constraint);
}

void albaInteractorConstraintTest::TestSetGetConstraintModality()
{
  albaInteractorConstraint *constraint = new albaInteractorConstraint;
  constraint->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK);   
  
  CPPUNIT_ASSERT(constraint->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE);
  CPPUNIT_ASSERT(constraint->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::LOCK);
  CPPUNIT_ASSERT(constraint->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::LOCK);

  cppDEL(constraint);
}

void albaInteractorConstraintTest::TestGetNumberOfDOF()
{
  albaInteractorConstraint *constraint = new albaInteractorConstraint;
  constraint->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK);   
  CPPUNIT_ASSERT(constraint->GetNumberOfDOF() == 1);
  cppDEL(constraint);
}

void albaInteractorConstraintTest::TestGetConstraintAxis()
{
  albaInteractorConstraint *constraint = new albaInteractorConstraint;
  constraint->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK);   
  CPPUNIT_ASSERT(constraint->GetConstraintAxis() == albaInteractorConstraint::X);
  cppDEL(constraint);
}

void albaInteractorConstraintTest::TestGetConstraintPlane()
{
  albaInteractorConstraint *constraint = new albaInteractorConstraint;
  constraint->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);   
  CPPUNIT_ASSERT(constraint->GetConstraintPlane() == albaInteractorConstraint::XY);
  cppDEL(constraint);
}

void albaInteractorConstraintTest::TestGetConstraintPlaneAxes()
{
  albaInteractorConstraint *constraint = new albaInteractorConstraint;
  constraint->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);   
  int axis1;
  int axis2;
  constraint->GetConstraintPlaneAxes(axis1,axis2);
  CPPUNIT_ASSERT(axis1 == albaInteractorConstraint::X && axis2 == albaInteractorConstraint::Y);
  cppDEL(constraint);
}

void albaInteractorConstraintTest::TestReset()
{
  albaInteractorConstraint *constraint = new albaInteractorConstraint;
  constraint->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK);   
  constraint->Reset();
  CPPUNIT_ASSERT(constraint->GetConstraintModality(albaInteractorConstraint::X) == albaInteractorConstraint::FREE);
  CPPUNIT_ASSERT(constraint->GetConstraintModality(albaInteractorConstraint::Y) == albaInteractorConstraint::FREE);
  CPPUNIT_ASSERT(constraint->GetConstraintModality(albaInteractorConstraint::Z) == albaInteractorConstraint::FREE);

  cppDEL(constraint);
}

void albaInteractorConstraintTest::TestSetGetLowerBound()
{
  albaInteractorConstraint *constraint = new albaInteractorConstraint;
  constraint->SetLowerBound(albaInteractorConstraint::X, 3);
  CPPUNIT_ASSERT(constraint->GetLowerBound(albaInteractorConstraint::X) == 3);
  cppDEL(constraint);
}

void albaInteractorConstraintTest::TestSetGetUpperBound()
{
  albaInteractorConstraint *constraint = new albaInteractorConstraint;
  constraint->SetUpperBound(albaInteractorConstraint::X, 3);
  CPPUNIT_ASSERT(constraint->GetUpperBound(albaInteractorConstraint::X) == 3);

  cppDEL(constraint);
}

void albaInteractorConstraintTest::TestSetGetBounds()
{
  double inBounds[2];
  inBounds[0] = 0;
  inBounds[1] = 10;

  albaInteractorConstraint *constraint = new albaInteractorConstraint;
  constraint->SetBounds(albaInteractorConstraint::X, inBounds);
  
  double outBounds[2];
  outBounds[0] = -1;
  outBounds[1] = -1;

  constraint->GetBounds(albaInteractorConstraint::X, outBounds);
  CPPUNIT_ASSERT(outBounds[0] == inBounds[0] && outBounds[1] == outBounds[1]);

  cppDEL(constraint);
}

void albaInteractorConstraintTest::TestSetGetMin()
{
  albaInteractorConstraint *constraint = new albaInteractorConstraint;
  constraint->SetMin(albaInteractorConstraint::X, 3);
  CPPUNIT_ASSERT(constraint->GetMin(albaInteractorConstraint::X) == 3);
  cppDEL(constraint);
}

void albaInteractorConstraintTest::TestSetGetMax()
{
  albaInteractorConstraint *constraint = new albaInteractorConstraint;
  constraint->SetMax(albaInteractorConstraint::X, 3);
  CPPUNIT_ASSERT(constraint->GetMax(albaInteractorConstraint::X) == 3);
  cppDEL(constraint);
}

void albaInteractorConstraintTest::TestSetGetStep()
{
  albaInteractorConstraint *constraint = new albaInteractorConstraint;
  constraint->SetStep(albaInteractorConstraint::X, 3);
  CPPUNIT_ASSERT(constraint->GetStep(albaInteractorConstraint::X) == 3);
  cppDEL(constraint);
}

void albaInteractorConstraintTest::TestSetSnapStep()
{
  albaInteractorConstraint *constraint = new albaInteractorConstraint;
  constraint->SetSnapStep(albaInteractorConstraint::X, 2, 10, 2);
  CPPUNIT_ASSERT(constraint->GetMin(albaInteractorConstraint::X) == 2);
  CPPUNIT_ASSERT(constraint->GetMax(albaInteractorConstraint::X) == 10);
  CPPUNIT_ASSERT(constraint->GetStep(albaInteractorConstraint::X) == 2);
  cppDEL(constraint);
}

void albaInteractorConstraintTest::TestSetGetSnapArray()
{
  vtkDoubleArray *array = vtkDoubleArray::New();

  albaInteractorConstraint *constraint = new albaInteractorConstraint;
  constraint->SetSnapArray(albaInteractorConstraint::X, array);
  
  CPPUNIT_ASSERT(constraint->GetSnapArray(albaInteractorConstraint::X) == array);
  cppDEL(constraint);

  vtkDEL(array);
}
