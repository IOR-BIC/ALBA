/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTransformConstraintTest
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

#include "albaTransformConstraintTest.h"
#include "vtkDoubleArray.h"
#include "vtkRenderer.h"
#include "albaRefSys.h"
#include <cppunit/config/SourcePrefix.h>
#include <iostream>


void albaTransformConstraintTest::TestFixture()
{

}

void albaTransformConstraintTest::TestConstructor()
{
  albaTransformConstraint *constraint = new albaTransformConstraint;
  cppDEL(constraint);
}

void albaTransformConstraintTest::TestSetGetConstraintModality()
{
  albaTransformConstraint *constraint = new albaTransformConstraint;
  constraint->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::LOCK, albaTransformConstraint::LOCK);   
  
  CPPUNIT_ASSERT(constraint->GetConstraintModality(albaTransformConstraint::X) == albaTransformConstraint::FREE);
  CPPUNIT_ASSERT(constraint->GetConstraintModality(albaTransformConstraint::Y) == albaTransformConstraint::LOCK);
  CPPUNIT_ASSERT(constraint->GetConstraintModality(albaTransformConstraint::Z) == albaTransformConstraint::LOCK);

  cppDEL(constraint);
}

void albaTransformConstraintTest::TestGetNumberOfDOF()
{
  albaTransformConstraint *constraint = new albaTransformConstraint;
  constraint->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::LOCK, albaTransformConstraint::LOCK);   
  CPPUNIT_ASSERT(constraint->GetNumberOfDOF() == 1);
  cppDEL(constraint);
}

void albaTransformConstraintTest::TestGetConstraintAxis()
{
  albaTransformConstraint *constraint = new albaTransformConstraint;
  constraint->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::LOCK, albaTransformConstraint::LOCK);   
  CPPUNIT_ASSERT(constraint->GetConstraintAxis() == albaTransformConstraint::X);
  cppDEL(constraint);
}

void albaTransformConstraintTest::TestGetConstraintPlane()
{
  albaTransformConstraint *constraint = new albaTransformConstraint;
  constraint->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::FREE, albaTransformConstraint::LOCK);   
  CPPUNIT_ASSERT(constraint->GetConstraintPlane() == albaTransformConstraint::XY);
  cppDEL(constraint);
}

void albaTransformConstraintTest::TestGetConstraintPlaneAxes()
{
  albaTransformConstraint *constraint = new albaTransformConstraint;
  constraint->SetConstraintModality(albaTransformConstraint::FREE, albaTransformConstraint::FREE, albaTransformConstraint::LOCK);   
  int axis1;
  int axis2;
  constraint->GetConstraintPlaneAxes(axis1,axis2);
  CPPUNIT_ASSERT(axis1 == albaTransformConstraint::X && axis2 == albaTransformConstraint::Y);
  cppDEL(constraint);
}

void albaTransformConstraintTest::TestReset()
{
  albaTransformConstraint *constraint = new albaTransformConstraint;
  constraint->SetConstraintModality(albaTransformConstraint::LOCK, albaTransformConstraint::LOCK, albaTransformConstraint::LOCK);   
  constraint->Reset();
  CPPUNIT_ASSERT(constraint->GetConstraintModality(albaTransformConstraint::X) == albaTransformConstraint::FREE);
  CPPUNIT_ASSERT(constraint->GetConstraintModality(albaTransformConstraint::Y) == albaTransformConstraint::FREE);
  CPPUNIT_ASSERT(constraint->GetConstraintModality(albaTransformConstraint::Z) == albaTransformConstraint::FREE);

  cppDEL(constraint);
}

void albaTransformConstraintTest::TestSetGetLowerBound()
{
  albaTransformConstraint *constraint = new albaTransformConstraint;
  constraint->SetLowerBound(albaTransformConstraint::X, 3);
  CPPUNIT_ASSERT(constraint->GetLowerBound(albaTransformConstraint::X) == 3);
  cppDEL(constraint);
}

void albaTransformConstraintTest::TestSetGetUpperBound()
{
  albaTransformConstraint *constraint = new albaTransformConstraint;
  constraint->SetUpperBound(albaTransformConstraint::X, 3);
  CPPUNIT_ASSERT(constraint->GetUpperBound(albaTransformConstraint::X) == 3);

  cppDEL(constraint);
}

void albaTransformConstraintTest::TestSetGetBounds()
{
  double inBounds[2];
  inBounds[0] = 0;
  inBounds[1] = 10;

  albaTransformConstraint *constraint = new albaTransformConstraint;
  constraint->SetBounds(albaTransformConstraint::X, inBounds);
  
  double outBounds[2];
  outBounds[0] = -1;
  outBounds[1] = -1;

  constraint->GetBounds(albaTransformConstraint::X, outBounds);
  CPPUNIT_ASSERT(outBounds[0] == inBounds[0] && outBounds[1] == outBounds[1]);

  cppDEL(constraint);
}

void albaTransformConstraintTest::TestSetGetMin()
{
  albaTransformConstraint *constraint = new albaTransformConstraint;
  constraint->SetMin(albaTransformConstraint::X, 3);
  CPPUNIT_ASSERT(constraint->GetMin(albaTransformConstraint::X) == 3);
  cppDEL(constraint);
}

void albaTransformConstraintTest::TestSetGetMax()
{
  albaTransformConstraint *constraint = new albaTransformConstraint;
  constraint->SetMax(albaTransformConstraint::X, 3);
  CPPUNIT_ASSERT(constraint->GetMax(albaTransformConstraint::X) == 3);
  cppDEL(constraint);
}

void albaTransformConstraintTest::TestSetGetStep()
{
  albaTransformConstraint *constraint = new albaTransformConstraint;
  constraint->SetStep(albaTransformConstraint::X, 3);
  CPPUNIT_ASSERT(constraint->GetStep(albaTransformConstraint::X) == 3);
  cppDEL(constraint);
}

void albaTransformConstraintTest::TestSetSnapStep()
{
  albaTransformConstraint *constraint = new albaTransformConstraint;
  constraint->SetSnapStep(albaTransformConstraint::X, 2, 10, 2);
  CPPUNIT_ASSERT(constraint->GetMin(albaTransformConstraint::X) == 2);
  CPPUNIT_ASSERT(constraint->GetMax(albaTransformConstraint::X) == 10);
  CPPUNIT_ASSERT(constraint->GetStep(albaTransformConstraint::X) == 2);
  cppDEL(constraint);
}

void albaTransformConstraintTest::TestSetGetSnapArray()
{
  vtkDoubleArray *array = vtkDoubleArray::New();

  albaTransformConstraint *constraint = new albaTransformConstraint;
  constraint->SetSnapArray(albaTransformConstraint::X, array);
  
  CPPUNIT_ASSERT(constraint->GetSnapArray(albaTransformConstraint::X) == array);
  cppDEL(constraint);

  vtkDEL(array);
}

//----------------------------------------------------------------------------
void albaTransformConstraintTest::TestCopy()
{
  vtkRenderer *renderer = vtkRenderer::New();
  albaTransformConstraint *source = new albaTransformConstraint();
  source->SetConstraintModality(albaTransformConstraint::X, albaTransformConstraint::BOUNDS);
  source->SetUpperBound(albaTransformConstraint::X, 5);
  source->SetStep(albaTransformConstraint::Y, 2);
  source->GetRefSys()->SetTypeToView(renderer);

  // the assignment copies the settings and the contents of the ref sys, not its pointer
  albaTransformConstraint *target = new albaTransformConstraint();
  *target = *source;
  CPPUNIT_ASSERT(target->GetRefSys() != source->GetRefSys());
  CPPUNIT_ASSERT(target->GetConstraintModality(albaTransformConstraint::X) == albaTransformConstraint::BOUNDS);
  CPPUNIT_ASSERT(target->GetUpperBound(albaTransformConstraint::X) == 5);
  CPPUNIT_ASSERT(target->GetStep(albaTransformConstraint::Y) == 2);
  CPPUNIT_ASSERT(target->GetRefSys()->GetRenderer() == renderer);

  // the copy constructor does the same
  albaTransformConstraint *copy = new albaTransformConstraint(*source);
  CPPUNIT_ASSERT(copy->GetRefSys() != source->GetRefSys());
  CPPUNIT_ASSERT(copy->GetUpperBound(albaTransformConstraint::X) == 5);

  // each constraint deletes its own ref sys and releases the renderer
  delete source;
  delete target;
  delete copy;
  CPPUNIT_ASSERT(renderer->GetReferenceCount() == 1);
  renderer->Delete();
}