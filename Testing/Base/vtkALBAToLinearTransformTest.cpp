/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAToLinearTransformTest
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

#include <cppunit/config/SourcePrefix.h>
#include "vtkALBAToLinearTransformTest.h"

#include "albaSmartPointer.h"
#include "albaMatrix.h"
#include "albaTransform.h"

#include "vtkALBAToLinearTransform.h"
#include "vtkALBASmartPointer.h"

#include "vtkLinearTransform.h"
#include <iostream>


//----------------------------------------------------------------------------
void vtkALBAToLinearTransformTest::TestSmartAllocation()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkALBAToLinearTransform> tl;
}
//----------------------------------------------------------------------------
void vtkALBAToLinearTransformTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  vtkALBAToLinearTransform *lt = vtkALBAToLinearTransform::New();
  lt->Delete();
}
//-------------------------------------------------------------------------
void vtkALBAToLinearTransformTest::TestSetGetInputMatrix()
//-------------------------------------------------------------------------
{
  albaSmartPointer<albaMatrix> matrix1;
  matrix1->Identity();
  matrix1->SetElement(2,2,2); //set i=2, j=2 element to 2 instead of 1
  
  vtkALBASmartPointer<vtkALBAToLinearTransform> tl;
  tl->SetInputMatrix(matrix1);
  tl->Update();

  albaMatrix *control = tl->GetInputMatrix();
  
  CPPUNIT_ASSERT(matrix1->Equals(control));

}
//-------------------------------------------------------------------------
void vtkALBAToLinearTransformTest::TestSetGetInputTransform()
//-------------------------------------------------------------------------
{
  albaSmartPointer<albaMatrix> matrix1;
  matrix1->Identity();
  int i,j;
  for(i=0;i<4;i++)
    for(j=0;j<4;j++)
      matrix1->SetElement(i,j,i+j);

  albaSmartPointer<albaTransform> transform;
  transform->SetMatrix(*matrix1);
  transform->Update();

  vtkALBASmartPointer<vtkALBAToLinearTransform> tl;
  tl->SetInputTransform(transform);
  tl->Update();

  albaTransform *control = albaTransform::SafeDownCast(tl->GetInputTransform());

  double pos1[3], rot1[3];
  double pos2[3], rot2[3];

  transform->GetPosition(pos1);
  transform->GetOrientation(rot1);

  control->GetPosition(pos2);
  control->GetOrientation(rot2);
  
  CPPUNIT_ASSERT(pos1[0] == pos2[0] && pos1[1] == pos2[1] && pos1[2] == pos2[2]);
  CPPUNIT_ASSERT(rot1[0] == rot2[0] && rot1[1] == rot2[1] && rot1[2] == rot2[2]);

}
//-------------------------------------------------------------------------
void vtkALBAToLinearTransformTest::TestInverse()
//-------------------------------------------------------------------------
{
  albaMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);

  albaMatrix m2;
  m2 = m;

  m.Invert();

  albaSmartPointer<albaTransform> transform1; //inverted
  transform1->SetMatrix(m);
  transform1->Update();

  vtkALBASmartPointer<vtkALBAToLinearTransform> tl;
  tl->SetInputTransform(transform1);
  tl->Inverse();
  tl->Update();
  
  albaMatrix control;
  control.SetVTKMatrix(tl->GetMatrix());

  CPPUNIT_ASSERT(m2.Equals(&control));
}
//-------------------------------------------------------------------------
void vtkALBAToLinearTransformTest::TestMTime()
//-------------------------------------------------------------------------
{
  albaSmartPointer<albaMatrix> m;
  m->Modified();
  unsigned long timeMatrix = m->GetMTime();

  vtkALBASmartPointer<vtkALBAToLinearTransform> tl0;
  tl0->SetInputMatrix(m);
  tl0->Update();

  CPPUNIT_ASSERT(tl0->GetMTime() >= timeMatrix);

  albaSmartPointer<albaTransform> transform;
  transform->Modified();
  unsigned long timeTransform = transform->GetMTime();

  vtkALBASmartPointer<vtkALBAToLinearTransform> tl1;
  tl1->SetInputTransform(transform);
  tl1->Update();

  CPPUNIT_ASSERT(tl1->GetMTime() >= timeTransform);

}
//-------------------------------------------------------------------------
void vtkALBAToLinearTransformTest::TestMakeTransform()
//-------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkALBAToLinearTransform> tl1;
  vtkLinearTransform *copy = NULL;
  copy = vtkLinearTransform::SafeDownCast(tl1->MakeTransform());

  CPPUNIT_ASSERT(copy != NULL);

  copy->Delete();
}
