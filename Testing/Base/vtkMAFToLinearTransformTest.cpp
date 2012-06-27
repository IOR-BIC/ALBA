/*=========================================================================

 Program: MAF2
 Module: vtkMAFToLinearTransformTest
 Authors: Daniele Giunchi
 
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

#include <cppunit/config/SourcePrefix.h>
#include "vtkMAFToLinearTransformTest.h"

#include "mafSmartPointer.h"
#include "mafMatrix.h"
#include "mafTransform.h"

#include "vtkMAFToLinearTransform.h"
#include "vtkMAFSmartPointer.h"

#include "vtkLinearTransform.h"
#include <iostream>

//----------------------------------------------------------------------------
void vtkMAFToLinearTransformTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkMAFToLinearTransformTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void vtkMAFToLinearTransformTest::TestSmartAllocation()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFToLinearTransform> tl;
}
//----------------------------------------------------------------------------
void vtkMAFToLinearTransformTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  vtkMAFToLinearTransform *lt = vtkMAFToLinearTransform::New();
  lt->Delete();
}
//-------------------------------------------------------------------------
void vtkMAFToLinearTransformTest::TestSetGetInputMatrix()
//-------------------------------------------------------------------------
{
  mafSmartPointer<mafMatrix> matrix1;
  matrix1->Identity();
  matrix1->SetElement(2,2,2); //set i=2, j=2 element to 2 instead of 1
  
  vtkMAFSmartPointer<vtkMAFToLinearTransform> tl;
  tl->SetInputMatrix(matrix1);
  tl->Update();

  mafMatrix *control = tl->GetInputMatrix();
  
  CPPUNIT_ASSERT(matrix1->Equals(control));

}
//-------------------------------------------------------------------------
void vtkMAFToLinearTransformTest::TestSetGetInputTransform()
//-------------------------------------------------------------------------
{
  mafSmartPointer<mafMatrix> matrix1;
  matrix1->Identity();
  int i,j;
  for(i=0;i<4;i++)
    for(j=0;j<4;j++)
      matrix1->SetElement(i,j,i+j);

  mafSmartPointer<mafTransform> transform;
  transform->SetMatrix(*matrix1);
  transform->Update();

  vtkMAFSmartPointer<vtkMAFToLinearTransform> tl;
  tl->SetInputTransform(transform);
  tl->Update();

  mafTransform *control = mafTransform::SafeDownCast(tl->GetInputTransform());

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
void vtkMAFToLinearTransformTest::TestInverse()
//-------------------------------------------------------------------------
{
  mafMatrix m;
  m.SetElement(0,0,3.0);
  m.SetElement(1,0,2.0);
  m.SetElement(2,2,5.0);

  mafMatrix m2;
  m2 = m;

  m.Invert();

  mafSmartPointer<mafTransform> transform1; //inverted
  transform1->SetMatrix(m);
  transform1->Update();

  vtkMAFSmartPointer<vtkMAFToLinearTransform> tl;
  tl->SetInputTransform(transform1);
  tl->Inverse();
  tl->Update();
  
  mafMatrix control;
  control.SetVTKMatrix(tl->GetMatrix());

  CPPUNIT_ASSERT(m2.Equals(&control));
}
//-------------------------------------------------------------------------
void vtkMAFToLinearTransformTest::TestMTime()
//-------------------------------------------------------------------------
{
  mafSmartPointer<mafMatrix> m;
  m->Modified();
  unsigned long timeMatrix = m->GetMTime();

  vtkMAFSmartPointer<vtkMAFToLinearTransform> tl0;
  tl0->SetInputMatrix(m);
  tl0->Update();

  CPPUNIT_ASSERT(tl0->GetMTime() >= timeMatrix);

  mafSmartPointer<mafTransform> transform;
  transform->Modified();
  unsigned long timeTransform = transform->GetMTime();

  vtkMAFSmartPointer<vtkMAFToLinearTransform> tl1;
  tl1->SetInputTransform(transform);
  tl1->Update();

  CPPUNIT_ASSERT(tl1->GetMTime() >= timeTransform);

}
//-------------------------------------------------------------------------
void vtkMAFToLinearTransformTest::TestMakeTransform()
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFToLinearTransform> tl1;
  vtkLinearTransform *copy = NULL;
  copy = vtkLinearTransform::SafeDownCast(tl1->MakeTransform());

  CPPUNIT_ASSERT(copy != NULL);

  copy->Delete();
}
