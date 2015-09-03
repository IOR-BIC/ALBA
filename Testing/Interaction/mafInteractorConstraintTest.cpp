/*=========================================================================

 Program: MAF2
 Module: mafInteractorConstraintTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafInteractorConstraintTest.h"
#include "vtkDoubleArray.h"
#include <cppunit/config/SourcePrefix.h>
#include <iostream>


void mafInteractorConstraintTest::TestFixture()
{

}

void mafInteractorConstraintTest::TestConstructor()
{
  mafInteractorConstraint *constraint = new mafInteractorConstraint;
  cppDEL(constraint);
}

void mafInteractorConstraintTest::TestSetGetConstraintModality()
{
  mafInteractorConstraint *constraint = new mafInteractorConstraint;
  constraint->SetConstraintModality(mafInteractorConstraint::FREE, mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK);   
  
  CPPUNIT_ASSERT(constraint->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::FREE);
  CPPUNIT_ASSERT(constraint->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::LOCK);
  CPPUNIT_ASSERT(constraint->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::LOCK);

  cppDEL(constraint);
}

void mafInteractorConstraintTest::TestGetNumberOfDOF()
{
  mafInteractorConstraint *constraint = new mafInteractorConstraint;
  constraint->SetConstraintModality(mafInteractorConstraint::FREE, mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK);   
  CPPUNIT_ASSERT(constraint->GetNumberOfDOF() == 1);
  cppDEL(constraint);
}

void mafInteractorConstraintTest::TestGetConstraintAxis()
{
  mafInteractorConstraint *constraint = new mafInteractorConstraint;
  constraint->SetConstraintModality(mafInteractorConstraint::FREE, mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK);   
  CPPUNIT_ASSERT(constraint->GetConstraintAxis() == mafInteractorConstraint::X);
  cppDEL(constraint);
}

void mafInteractorConstraintTest::TestGetConstraintPlane()
{
  mafInteractorConstraint *constraint = new mafInteractorConstraint;
  constraint->SetConstraintModality(mafInteractorConstraint::FREE, mafInteractorConstraint::FREE, mafInteractorConstraint::LOCK);   
  CPPUNIT_ASSERT(constraint->GetConstraintPlane() == mafInteractorConstraint::XY);
  cppDEL(constraint);
}

void mafInteractorConstraintTest::TestGetConstraintPlaneAxes()
{
  mafInteractorConstraint *constraint = new mafInteractorConstraint;
  constraint->SetConstraintModality(mafInteractorConstraint::FREE, mafInteractorConstraint::FREE, mafInteractorConstraint::LOCK);   
  int axis1;
  int axis2;
  constraint->GetConstraintPlaneAxes(axis1,axis2);
  CPPUNIT_ASSERT(axis1 == mafInteractorConstraint::X && axis2 == mafInteractorConstraint::Y);
  cppDEL(constraint);
}

void mafInteractorConstraintTest::TestReset()
{
  mafInteractorConstraint *constraint = new mafInteractorConstraint;
  constraint->SetConstraintModality(mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK, mafInteractorConstraint::LOCK);   
  constraint->Reset();
  CPPUNIT_ASSERT(constraint->GetConstraintModality(mafInteractorConstraint::X) == mafInteractorConstraint::FREE);
  CPPUNIT_ASSERT(constraint->GetConstraintModality(mafInteractorConstraint::Y) == mafInteractorConstraint::FREE);
  CPPUNIT_ASSERT(constraint->GetConstraintModality(mafInteractorConstraint::Z) == mafInteractorConstraint::FREE);

  cppDEL(constraint);
}

void mafInteractorConstraintTest::TestSetGetLowerBound()
{
  mafInteractorConstraint *constraint = new mafInteractorConstraint;
  constraint->SetLowerBound(mafInteractorConstraint::X, 3);
  CPPUNIT_ASSERT(constraint->GetLowerBound(mafInteractorConstraint::X) == 3);
  cppDEL(constraint);
}

void mafInteractorConstraintTest::TestSetGetUpperBound()
{
  mafInteractorConstraint *constraint = new mafInteractorConstraint;
  constraint->SetUpperBound(mafInteractorConstraint::X, 3);
  CPPUNIT_ASSERT(constraint->GetUpperBound(mafInteractorConstraint::X) == 3);

  cppDEL(constraint);
}

void mafInteractorConstraintTest::TestSetGetBounds()
{
  double inBounds[2];
  inBounds[0] = 0;
  inBounds[1] = 10;

  mafInteractorConstraint *constraint = new mafInteractorConstraint;
  constraint->SetBounds(mafInteractorConstraint::X, inBounds);
  
  double outBounds[2];
  outBounds[0] = -1;
  outBounds[1] = -1;

  constraint->GetBounds(mafInteractorConstraint::X, outBounds);
  CPPUNIT_ASSERT(outBounds[0] == inBounds[0] && outBounds[1] == outBounds[1]);

  cppDEL(constraint);
}

void mafInteractorConstraintTest::TestSetGetMin()
{
  mafInteractorConstraint *constraint = new mafInteractorConstraint;
  constraint->SetMin(mafInteractorConstraint::X, 3);
  CPPUNIT_ASSERT(constraint->GetMin(mafInteractorConstraint::X) == 3);
  cppDEL(constraint);
}

void mafInteractorConstraintTest::TestSetGetMax()
{
  mafInteractorConstraint *constraint = new mafInteractorConstraint;
  constraint->SetMax(mafInteractorConstraint::X, 3);
  CPPUNIT_ASSERT(constraint->GetMax(mafInteractorConstraint::X) == 3);
  cppDEL(constraint);
}

void mafInteractorConstraintTest::TestSetGetStep()
{
  mafInteractorConstraint *constraint = new mafInteractorConstraint;
  constraint->SetStep(mafInteractorConstraint::X, 3);
  CPPUNIT_ASSERT(constraint->GetStep(mafInteractorConstraint::X) == 3);
  cppDEL(constraint);
}

void mafInteractorConstraintTest::TestSetSnapStep()
{
  mafInteractorConstraint *constraint = new mafInteractorConstraint;
  constraint->SetSnapStep(mafInteractorConstraint::X, 2, 10, 2);
  CPPUNIT_ASSERT(constraint->GetMin(mafInteractorConstraint::X) == 2);
  CPPUNIT_ASSERT(constraint->GetMax(mafInteractorConstraint::X) == 10);
  CPPUNIT_ASSERT(constraint->GetStep(mafInteractorConstraint::X) == 2);
  cppDEL(constraint);
}

void mafInteractorConstraintTest::TestSetGetSnapArray()
{
  vtkDoubleArray *array = vtkDoubleArray::New();

  mafInteractorConstraint *constraint = new mafInteractorConstraint;
  constraint->SetSnapArray(mafInteractorConstraint::X, array);
  
  CPPUNIT_ASSERT(constraint->GetSnapArray(mafInteractorConstraint::X) == array);
  cppDEL(constraint);

  vtkDEL(array);
}
