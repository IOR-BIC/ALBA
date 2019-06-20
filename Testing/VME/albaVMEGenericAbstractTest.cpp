/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEGenericAbstractTest
 Authors: Alberto Losi
 
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
#include "albaVMEGenericAbstractTest.h"
#include "albaVMEGenericAbstract.h"
#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaSmartPointer.h"
#include "albaVMESurfaceParametric.h"
#include "albaVMEItem.h"
#include "albaVMEItemVTK.h"
#include "albaMatrixVector.h"
#include "albaDataVector.h"
#include "albaVMEOutputNULL.h"
#include "stdio.h"

#define TEST_RESULT CPPUNIT_ASSERT(result)

enum PARAMETRIC_SURFACE_TYPE_ID
{
  PARAMETRIC_SPHERE = 0,
  PARAMETRIC_CONE,
  PARAMETRIC_CYLINDER,
  PARAMETRIC_CUBE,
  PARAMETRIC_PLANE,
  PARAMETRIC_ELLIPSOID,
};

//----------------------------------------------------------------------------
/** Concrete class for testing. */
class albaVMEGenericAbstractConcrete: public albaVMEGenericAbstract
//----------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaVMEGenericAbstractConcrete,albaVMEGenericAbstract);
  void SetData(albaVMEItem *item); // Add an item to the DataVector. Implemented for test GetDataVector() method (and related)
protected:
  albaVMEGenericAbstractConcrete();
  virtual ~albaVMEGenericAbstractConcrete();
  albaTransform *m_Transform;
};

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEGenericAbstractConcrete);
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
albaVMEGenericAbstractConcrete::albaVMEGenericAbstractConcrete()
:albaVMEGenericAbstract()
//----------------------------------------------------------------------------
{
  m_DataVector = new albaDataVector();
  m_DataVector->SetListener(this);

  albaNEW(m_Transform);
  albaVMEOutputNULL *output = albaVMEOutputNULL::New();
  output->SetTransform(m_Transform);
  SetOutput(output);
}
//----------------------------------------------------------------------------
albaVMEGenericAbstractConcrete::~albaVMEGenericAbstractConcrete()
//----------------------------------------------------------------------------
{
  albaDEL(m_Transform);
  cppDEL(m_DataVector);
}
//----------------------------------------------------------------------------
void albaVMEGenericAbstractConcrete::SetData(albaVMEItem *item)
//----------------------------------------------------------------------------
{
  m_DataVector->AppendItem(item);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void albaVMEGenericAbstractTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaVMEGenericAbstractTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGenericAbstractConcrete> vme1;
  albaVMEGenericAbstractConcrete *vme2;
  albaNEW(vme2);
  albaDEL(vme2);
}

//----------------------------------------------------------------------------
void albaVMEGenericAbstractTest::TestAllConstructor()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGenericAbstractConcrete> vme1;
  result = vme1->GetReferenceCount() == 1; // albaSmartPointer increase the reference count
  TEST_RESULT;

  albaVMEGenericAbstractConcrete *vme2 = NULL;
  vme2 = albaVMEGenericAbstractConcrete::New();
  result = vme2->GetReferenceCount() == 0; // default reference count on New
  TEST_RESULT;
  albaDEL(vme2);

  albaVMEGenericAbstractConcrete *vme3 = NULL;
  albaNEW(vme3);
  result = vme3->GetReferenceCount() == 1; // albaNEW macro increase the reference count
  TEST_RESULT;
  albaDEL(vme3);
}
//----------------------------------------------------------------------------
void albaVMEGenericAbstractTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  // Prepare matrices to add to the MatrixVector
  albaMatrix matrix1, matrix2;
  matrix1.SetElement(0,0,.1);
  matrix1.SetElement(0,1,.2);
  matrix1.SetElement(0,2,.3);
  matrix1.SetElement(0,3,.4);

  matrix1.SetElement(1,0,.5);
  matrix1.SetElement(1,1,.6);
  matrix1.SetElement(1,2,.7);
  matrix1.SetElement(1,3,.8);

  matrix1.SetElement(2,0,.9);
  matrix1.SetElement(2,1,1.0);
  matrix1.SetElement(2,2,1.1);
  matrix1.SetElement(2,3,1.2);

  matrix1.SetElement(3,0,1.3);
  matrix1.SetElement(3,1,1.4);
  matrix1.SetElement(3,2,1.5);
  matrix1.SetElement(3,3,1.6);

  matrix2.SetElement(0,0,1.8);
  matrix2.SetElement(0,1,1.9);
  matrix2.SetElement(0,2,2.0);
  matrix2.SetElement(0,3,2.1);

  matrix2.SetElement(1,0,2.2);
  matrix2.SetElement(1,1,2.3);
  matrix2.SetElement(1,2,2.4);
  matrix2.SetElement(1,3,2.5);

  matrix2.SetElement(2,0,2.6);
  matrix2.SetElement(2,1,2.7);
  matrix2.SetElement(2,2,2.8);
  matrix2.SetElement(2,3,2.9);

  matrix2.SetElement(3,0,3.0);
  matrix2.SetElement(3,1,3.1);
  matrix2.SetElement(3,2,3.2);
  matrix2.SetElement(3,3,3.3);

  matrix1.SetTimeStamp(1.);
  matrix2.SetTimeStamp(2.);

  // Prepare data to add to the DataVector
  // Create dummy VME to get DataSets
  albaVMESurfaceParametric *sphere, *cube;
  albaNEW(sphere);
  albaNEW(cube);
  sphere->SetGeometryType(PARAMETRIC_SPHERE);
  cube->SetGeometryType(PARAMETRIC_CUBE);

  albaVMEItemVTK *item1, *item2;
  albaNEW(item1);
  albaNEW(item2);
  item1->SetData(sphere->GetOutput()->GetVTKData());
  item2->SetData(cube->GetOutput()->GetVTKData());
  item1->SetTimeStamp(1.);
  item2->SetTimeStamp(2.);

  albaSmartPointer<albaVMEGenericAbstractConcrete> vme1;

  vme1->SetName("albaVMEGenericAbstract_Test");
  vme1->SetTimeStamp(0.);

  vme1->SetMatrix(matrix1);
  vme1->SetMatrix(matrix2);

  vme1->SetData(item1); // This is not a method of albaVMEGenericAbstract!
  vme1->SetData(item2); // This is not a method of albaVMEGenericAbstract!

  albaSmartPointer<albaVMEGenericAbstractConcrete> vme2;
 
  vme2->DeepCopy(vme1);

  result = vme1->Equals(vme2);
  TEST_RESULT;

  albaDEL(item1);
  albaDEL(item2);
  albaDEL(sphere);
  albaDEL(cube);
}
//----------------------------------------------------------------------------
void albaVMEGenericAbstractTest::TestShallowCopy()
//----------------------------------------------------------------------------
{
  // Prepare matrices to add to the MatrixVector
  albaMatrix matrix1, matrix2;
  matrix1.SetElement(0,0,.1);
  matrix1.SetElement(0,1,.2);
  matrix1.SetElement(0,2,.3);
  matrix1.SetElement(0,3,.4);

  matrix1.SetElement(1,0,.5);
  matrix1.SetElement(1,1,.6);
  matrix1.SetElement(1,2,.7);
  matrix1.SetElement(1,3,.8);

  matrix1.SetElement(2,0,.9);
  matrix1.SetElement(2,1,1.0);
  matrix1.SetElement(2,2,1.1);
  matrix1.SetElement(2,3,1.2);

  matrix1.SetElement(3,0,1.3);
  matrix1.SetElement(3,1,1.4);
  matrix1.SetElement(3,2,1.5);
  matrix1.SetElement(3,3,1.6);

  matrix2.SetElement(0,0,1.8);
  matrix2.SetElement(0,1,1.9);
  matrix2.SetElement(0,2,2.0);
  matrix2.SetElement(0,3,2.1);

  matrix2.SetElement(1,0,2.2);
  matrix2.SetElement(1,1,2.3);
  matrix2.SetElement(1,2,2.4);
  matrix2.SetElement(1,3,2.5);

  matrix2.SetElement(2,0,2.6);
  matrix2.SetElement(2,1,2.7);
  matrix2.SetElement(2,2,2.8);
  matrix2.SetElement(2,3,2.9);

  matrix2.SetElement(3,0,3.0);
  matrix2.SetElement(3,1,3.1);
  matrix2.SetElement(3,2,3.2);
  matrix2.SetElement(3,3,3.3);

  matrix1.SetTimeStamp(1.);
  matrix2.SetTimeStamp(2.);

  // Prepare data to add to the DataVector
  // Create dummy VME to get DataSets
  albaVMESurfaceParametric *sphere, *cube;
  albaNEW(sphere);
  albaNEW(cube);
  sphere->SetGeometryType(PARAMETRIC_SPHERE);
  cube->SetGeometryType(PARAMETRIC_CUBE);

  albaVMEItemVTK *item1, *item2;
  albaNEW(item1);
  albaNEW(item2);
  item1->SetData(sphere->GetOutput()->GetVTKData());
  item2->SetData(cube->GetOutput()->GetVTKData());
  item1->SetTimeStamp(1.);
  item2->SetTimeStamp(2.);

  albaSmartPointer<albaVMEGenericAbstractConcrete> vme1;

  vme1->SetName("albaVMEGenericAbstract_Test");
  vme1->SetTimeStamp(0.);

  vme1->SetMatrix(matrix1);
  vme1->SetMatrix(matrix2);

  vme1->SetData(item1); // This is not a method of albaVMEGenericAbstract!
  vme1->SetData(item2); // This is not a method of albaVMEGenericAbstract!

  albaSmartPointer<albaVMEGenericAbstractConcrete> vme2;

  vme2->ShallowCopy(vme1);

  result = vme1->Equals(vme2);
  TEST_RESULT;

  result = (vme1->GetDataVector()->GetNumberOfItems() == vme2->GetDataVector()->GetNumberOfItems());
  TEST_RESULT;

  for(int i = 0; i < vme1->GetDataVector()->GetNumberOfItems(); i++)
  {
    result = (vme1->GetDataVector()->GetItemByIndex(i)->Equals(vme2->GetDataVector()->GetItemByIndex(i))); // Shallow copy (m_DataVector attribute is referenced)
    TEST_RESULT;
  }

  albaDEL(item1);
  albaDEL(item2);
  albaDEL(sphere);
  albaDEL(cube);
}
//----------------------------------------------------------------------------
void albaVMEGenericAbstractTest::TestEquals()
//----------------------------------------------------------------------------
{
  // Prepare matrices to add to the MatrixVector
  albaMatrix matrix1A;
  albaMatrix matrix2A;
  matrix1A.SetElement(0,0,.1);
  matrix1A.SetElement(0,1,.2);
  matrix1A.SetElement(0,2,.3);
  matrix1A.SetElement(0,3,.4);

  matrix1A.SetElement(1,0,.5);
  matrix1A.SetElement(1,1,.6);
  matrix1A.SetElement(1,2,.7);
  matrix1A.SetElement(1,3,.8);

  matrix1A.SetElement(2,0,.9);
  matrix1A.SetElement(2,1,1.0);
  matrix1A.SetElement(2,2,1.1);
  matrix1A.SetElement(2,3,1.2);

  matrix1A.SetElement(3,0,1.3);
  matrix1A.SetElement(3,1,1.4);
  matrix1A.SetElement(3,2,1.5);
  matrix1A.SetElement(3,3,1.6);

  matrix2A.SetElement(0,0,1.8);
  matrix2A.SetElement(0,1,1.9);
  matrix2A.SetElement(0,2,2.0);
  matrix2A.SetElement(0,3,2.1);

  matrix2A.SetElement(1,0,2.2);
  matrix2A.SetElement(1,1,2.3);
  matrix2A.SetElement(1,2,2.4);
  matrix2A.SetElement(1,3,2.5);

  matrix2A.SetElement(2,0,2.6);
  matrix2A.SetElement(2,1,2.7);
  matrix2A.SetElement(2,2,2.8);
  matrix2A.SetElement(2,3,2.9);

  matrix2A.SetElement(3,0,3.0);
  matrix2A.SetElement(3,1,3.1);
  matrix2A.SetElement(3,2,3.2);
  matrix2A.SetElement(3,3,3.3);

  matrix1A.SetTimeStamp(1.);
  matrix2A.SetTimeStamp(2.);

  albaMatrix matrix1B;
  albaMatrix matrix2B;

  matrix1B.SetElement(0,0,1);
  matrix1B.SetElement(0,1,2);
  matrix1B.SetElement(0,2,3);
  matrix1B.SetElement(0,3,4);

  matrix1B.SetElement(1,0,5);
  matrix1B.SetElement(1,1,6);
  matrix1B.SetElement(1,2,7);
  matrix1B.SetElement(1,3,8);

  matrix1B.SetElement(2,0,9);
  matrix1B.SetElement(2,1,10);
  matrix1B.SetElement(2,2,11);
  matrix1B.SetElement(2,3,12);

  matrix1B.SetElement(3,0,13);
  matrix1B.SetElement(3,1,14);
  matrix1B.SetElement(3,2,15);
  matrix1B.SetElement(3,3,16);

  matrix2B.SetElement(0,0,18);
  matrix2B.SetElement(0,1,19);
  matrix2B.SetElement(0,2,20);
  matrix2B.SetElement(0,3,21);

  matrix2B.SetElement(1,0,22);
  matrix2B.SetElement(1,1,23);
  matrix2B.SetElement(1,2,24);
  matrix2B.SetElement(1,3,25);

   matrix2B.SetElement(2,0,26);
  matrix2B.SetElement(2,1,27);
  matrix2B.SetElement(2,2,28);
  matrix2B.SetElement(2,3,29);

  matrix2B.SetElement(3,0,30);
  matrix2B.SetElement(3,1,31);
  matrix2B.SetElement(3,2,32);
  matrix2B.SetElement(3,3,33);

  matrix1B.SetTimeStamp(1.);
  matrix2B.SetTimeStamp(2.);

  // Prepare data to add to the DataVector
  // Create dummy VME to get DataSets
  albaVMESurfaceParametric *sphere, *cube, *cone, *cylinder;
  albaNEW(sphere);
  albaNEW(cube);
  albaNEW(cone);
  albaNEW(cylinder);
  sphere->SetGeometryType(PARAMETRIC_SPHERE);
  cube->SetGeometryType(PARAMETRIC_CUBE);
  cone->SetGeometryType(PARAMETRIC_CONE);
  cylinder->SetGeometryType(PARAMETRIC_CYLINDER);

  albaVMEItemVTK *item1A, *item2A, *item1B, *item2B;
  albaNEW(item1A);
  albaNEW(item2A);
  albaNEW(item1B);
  albaNEW(item2B);
  item1A->SetData(sphere->GetOutput()->GetVTKData());
  item2A->SetData(cube->GetOutput()->GetVTKData());
  item1B->SetData(cone->GetOutput()->GetVTKData());
  item2B->SetData(cylinder->GetOutput()->GetVTKData());
  item1A->SetTimeStamp(1.);
  item2A->SetTimeStamp(2.);
  item1B->SetTimeStamp(3.);
  item2B->SetTimeStamp(4.);

  albaSmartPointer<albaVMEGenericAbstractConcrete> vme1;
  vme1->SetName("albaVMEGenericAbstract_Test");
  vme1->SetTimeStamp(0.);

  vme1->SetMatrix(matrix1A);
  vme1->SetMatrix(matrix2A);

  vme1->SetData(item1A); // This is not a method of albaVMEGenericAbstract!
  vme1->SetData(item2A); // This is not a method of albaVMEGenericAbstract!

  albaVMEGenericAbstractConcrete *vme2;
  albaNEW(vme2);
  // Different MatrixVector and DataVector
  vme2->SetName("albaVMEGenericAbstract_Test");
  vme2->SetTimeStamp(0.);

  vme2->SetMatrix(matrix1B);
  vme2->SetMatrix(matrix2B);

  vme2->SetData(item1B); // This is not a method of albaVMEGenericAbstract!
  vme2->SetData(item2B); // This is not a method of albaVMEGenericAbstract!

  result = !vme1->Equals(vme2);
  albaDEL(vme2);
  TEST_RESULT;

  // Same MatrixVector and different DataVector
  albaNEW(vme2);
  vme2->SetMatrix(matrix1A);
  vme2->SetMatrix(matrix2A);

  vme2->SetData(item1B); // This is not a method of albaVMEGenericAbstract!
  vme2->SetData(item2B); // This is not a method of albaVMEGenericAbstract!

  result = !vme1->Equals(vme2);
  albaDEL(vme2);
  TEST_RESULT;

  // Different MatrixVector and same DataVector
  albaNEW(vme2);
  vme2->SetMatrix(matrix1B);
  vme2->SetMatrix(matrix2B);

  vme2->SetData(item1A); // This is not a method of albaVMEGenericAbstract!
  vme2->SetData(item2A); // This is not a method of albaVMEGenericAbstract!

  result = !vme1->Equals(vme2);
  albaDEL(vme2);
  TEST_RESULT;

  // Same MatrixVector and same DataVector
  albaNEW(vme2);
  vme2->SetMatrix(matrix1A);
  vme2->SetMatrix(matrix2A);

  vme2->SetData(item1A); // This is not a method of albaVMEGenericAbstract!
  vme2->SetData(item2A); // This is not a method of albaVMEGenericAbstract!

  result = !vme1->Equals(vme2);
  TEST_RESULT;

  albaDEL(vme2);

  albaDEL(sphere);
  albaDEL(cube);
  albaDEL(cone);
  albaDEL(cylinder);

  albaDEL(item1A);
  albaDEL(item2A);
  albaDEL(item1B);
  albaDEL(item2B);
}
//----------------------------------------------------------------------------
void albaVMEGenericAbstractTest::TestReparentTo()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEGenericAbstractConcrete> vme;
  albaSmartPointer<albaVMEGenericAbstractConcrete> parent;

  vme->ReparentTo(parent);

  result = ((albaVMEGenericAbstractConcrete*)vme->GetParent() == (albaVMEGenericAbstractConcrete*)parent); // parent must be parent of vme
  TEST_RESULT;

  result = ((albaVMEGenericAbstractConcrete*)parent->GetFirstChild() == (albaVMEGenericAbstractConcrete*)vme); // vme must be first child of parent
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaVMEGenericAbstractTest::TestGetMatrixVector()
//----------------------------------------------------------------------------
{
  // Prepare matrices to add to the MatrixVector
  albaMatrix matrix1, matrix2;
  matrix1.SetElement(0,0,.1);
  matrix1.SetElement(0,1,.2);
  matrix1.SetElement(0,2,.3);
  matrix1.SetElement(0,3,.4);

  matrix1.SetElement(1,0,.5);
  matrix1.SetElement(1,1,.6);
  matrix1.SetElement(1,2,.7);
  matrix1.SetElement(1,3,.8);

  matrix1.SetElement(2,0,.9);
  matrix1.SetElement(2,1,1.0);
  matrix1.SetElement(2,2,1.1);
  matrix1.SetElement(2,3,1.2);

  matrix1.SetElement(3,0,1.3);
  matrix1.SetElement(3,1,1.4);
  matrix1.SetElement(3,2,1.5);
  matrix1.SetElement(3,3,1.6);

  matrix2.SetElement(0,0,1.8);
  matrix2.SetElement(0,1,1.9);
  matrix2.SetElement(0,2,2.0);
  matrix2.SetElement(0,3,2.1);

  matrix2.SetElement(1,0,2.2);
  matrix2.SetElement(1,1,2.3);
  matrix2.SetElement(1,2,2.4);
  matrix2.SetElement(1,3,2.5);

  matrix2.SetElement(2,0,2.6);
  matrix2.SetElement(2,1,2.7);
  matrix2.SetElement(2,2,2.8);
  matrix2.SetElement(2,3,2.9);

  matrix2.SetElement(3,0,3.0);
  matrix2.SetElement(3,1,3.1);
  matrix2.SetElement(3,2,3.2);
  matrix2.SetElement(3,3,3.3);

  matrix1.SetTimeStamp(1.);
  matrix2.SetTimeStamp(2.);

  albaSmartPointer<albaVMEGenericAbstractConcrete> vme;

  vme->SetMatrix(matrix1);
  vme->SetMatrix(matrix2);

  // Create a matrix vector with the same matrices
  albaMatrixVector *matrixVector = new albaMatrixVector();
  matrixVector->SetMatrix(matrix1);
  matrixVector->SetMatrix(matrix2);

  result = matrixVector->Equals(vme->GetMatrixVector()); // The two MatrixVector must be equals
  TEST_RESULT;

  cppDEL(matrixVector);
}
//----------------------------------------------------------------------------
void albaVMEGenericAbstractTest::TestSetMatrix()
//----------------------------------------------------------------------------
{
// Prepare matrices to add to the MatrixVector
  albaMatrix matrix;
  matrix.SetElement(0,0,.1);
  matrix.SetElement(0,1,.2);
  matrix.SetElement(0,2,.3);
  matrix.SetElement(0,3,.4);

  matrix.SetElement(1,0,.5);
  matrix.SetElement(1,1,.6);
  matrix.SetElement(1,2,.7);
  matrix.SetElement(1,3,.8);

  matrix.SetElement(2,0,.9);
  matrix.SetElement(2,1,1.0);
  matrix.SetElement(2,2,1.1);
  matrix.SetElement(2,3,1.2);

  matrix.SetElement(3,0,1.3);
  matrix.SetElement(3,1,1.4);
  matrix.SetElement(3,2,1.5);
  matrix.SetElement(3,3,1.6);

  matrix.SetTimeStamp(1.);

  albaSmartPointer<albaVMEGenericAbstractConcrete> vme;

  vme->SetMatrix(matrix);

  // Create a matrix vector with the same matrices
  albaMatrixVector *matrixVector = new albaMatrixVector();
  matrixVector->SetMatrix(matrix);

  // GetMatrixVector is the only way to get inserted matrices
  result = matrixVector->Equals(vme->GetMatrixVector()); // The two MatrixVector must be equals
  TEST_RESULT;

  cppDEL(matrixVector);
}
//----------------------------------------------------------------------------
void albaVMEGenericAbstractTest::TestGetDataVector()
//----------------------------------------------------------------------------
{
  // Prepare data to add to the DataVector
  // Create dummy VME to get DataSets
  albaVMESurfaceParametric *sphere, *cube;
  albaNEW(sphere);
  albaNEW(cube);
  sphere->SetGeometryType(PARAMETRIC_SPHERE);
  cube->SetGeometryType(PARAMETRIC_CUBE);

  albaVMEItemVTK *item1, *item2;
  albaNEW(item1);
  albaNEW(item2);
  item1->SetData(sphere->GetOutput()->GetVTKData());
  item2->SetData(cube->GetOutput()->GetVTKData());
  item1->SetTimeStamp(1.);
  item2->SetTimeStamp(2.);

  albaSmartPointer<albaVMEGenericAbstractConcrete> vme;

  vme->SetData(item1); // This is not a method of albaVMEGenericAbstract!
  vme->SetData(item2); // This is not a method of albaVMEGenericAbstract!

  albaDataVector *dataVector = new albaDataVector();
  //albaNEW(dataVector);
  dataVector->InsertItem(item1);
  dataVector->InsertItem(item2);

  result = dataVector->Equals(vme->GetDataVector()); // The two DataVector must be equals
  TEST_RESULT;

  cppDEL(dataVector);
  albaDEL(item1);
  albaDEL(item2);
  albaDEL(sphere);
  albaDEL(cube);
}
//----------------------------------------------------------------------------
void albaVMEGenericAbstractTest::TestGetDataTimeStamps()
//----------------------------------------------------------------------------
{
  // Prepare data to add to the DataVector
  albaVMEItemVTK *item1, *item2;
  albaNEW(item1);
  albaNEW(item2);
  item1->SetTimeStamp(1.);
  item2->SetTimeStamp(2.);

  albaSmartPointer<albaVMEGenericAbstractConcrete> vme;

  vme->SetData(item1); // This is not a method of albaVMEGenericAbstract!
  vme->SetData(item2); // This is not a method of albaVMEGenericAbstract!

  std::vector<albaTimeStamp> kframes;
  vme->GetDataTimeStamps(kframes);
  result = (kframes[0] == 1. && kframes[1] == 2.);

  albaDEL(item1);
  albaDEL(item2);
  TEST_RESULT;

  kframes.clear();
}
//----------------------------------------------------------------------------
void albaVMEGenericAbstractTest::TestGetMatrixTimeStamps()
//----------------------------------------------------------------------------
{
  // Prepare matrices to add to the MatrixVector
  albaMatrix matrix1, matrix2;
  matrix1.SetElement(0,0,.1);
  matrix1.SetElement(0,1,.2);
  matrix1.SetElement(0,2,.3);
  matrix1.SetElement(0,3,.4);

  matrix1.SetElement(1,0,.5);
  matrix1.SetElement(1,1,.6);
  matrix1.SetElement(1,2,.7);
  matrix1.SetElement(1,3,.8);

  matrix1.SetElement(2,0,.9);
  matrix1.SetElement(2,1,1.0);
  matrix1.SetElement(2,2,1.1);
  matrix1.SetElement(2,3,1.2);

  matrix1.SetElement(3,0,1.3);
  matrix1.SetElement(3,1,1.4);
  matrix1.SetElement(3,2,1.5);
  matrix1.SetElement(3,3,1.6);

  matrix2.SetElement(0,0,1.8);
  matrix2.SetElement(0,1,1.9);
  matrix2.SetElement(0,2,2.0);
  matrix2.SetElement(0,3,2.1);

  matrix2.SetElement(1,0,2.2);
  matrix2.SetElement(1,1,2.3);
  matrix2.SetElement(1,2,2.4);
  matrix2.SetElement(1,3,2.5);

  matrix2.SetElement(2,0,2.6);
  matrix2.SetElement(2,1,2.7);
  matrix2.SetElement(2,2,2.8);
  matrix2.SetElement(2,3,2.9);

  matrix2.SetElement(3,0,3.0);
  matrix2.SetElement(3,1,3.1);
  matrix2.SetElement(3,2,3.2);
  matrix2.SetElement(3,3,3.3);

  matrix1.SetTimeStamp(1.);
  matrix2.SetTimeStamp(2.);

  albaSmartPointer<albaVMEGenericAbstractConcrete> vme;

  vme->SetMatrix(matrix1);
  vme->SetMatrix(matrix2);

  std::vector<albaTimeStamp> kframes;
  vme->GetMatrixTimeStamps(kframes);
  result = (kframes[0] == 1. && kframes[1] == 2.);
  TEST_RESULT;

  kframes.clear();
}
//----------------------------------------------------------------------------
void albaVMEGenericAbstractTest::TestGetLocalTimeStamps()
//----------------------------------------------------------------------------
{
  // Prepare matrices to add to the MatrixVector
albaMatrix matrix1, matrix2;
  matrix1.SetElement(0,0,.1);
  matrix1.SetElement(0,1,.2);
  matrix1.SetElement(0,2,.3);
  matrix1.SetElement(0,3,.4);

  matrix1.SetElement(1,0,.5);
  matrix1.SetElement(1,1,.6);
  matrix1.SetElement(1,2,.7);
  matrix1.SetElement(1,3,.8);

  matrix1.SetElement(2,0,.9);
  matrix1.SetElement(2,1,1.0);
  matrix1.SetElement(2,2,1.1);
  matrix1.SetElement(2,3,1.2);

  matrix1.SetElement(3,0,1.3);
  matrix1.SetElement(3,1,1.4);
  matrix1.SetElement(3,2,1.5);
  matrix1.SetElement(3,3,1.6);

  matrix2.SetElement(0,0,1.8);
  matrix2.SetElement(0,1,1.9);
  matrix2.SetElement(0,2,2.0);
  matrix2.SetElement(0,3,2.1);

  matrix2.SetElement(1,0,2.2);
  matrix2.SetElement(1,1,2.3);
  matrix2.SetElement(1,2,2.4);
  matrix2.SetElement(1,3,2.5);

  matrix2.SetElement(2,0,2.6);
  matrix2.SetElement(2,1,2.7);
  matrix2.SetElement(2,2,2.8);
  matrix2.SetElement(2,3,2.9);

  matrix2.SetElement(3,0,3.0);
  matrix2.SetElement(3,1,3.1);
  matrix2.SetElement(3,2,3.2);
  matrix2.SetElement(3,3,3.3);

  matrix1.SetTimeStamp(1.);
  matrix2.SetTimeStamp(2.);

  // Prepare data to add to the DataVector
  albaVMEItemVTK *item1, *item2;
  albaNEW(item1);
  albaNEW(item2);
  item1->SetTimeStamp(0.);
  item2->SetTimeStamp(2.);

  albaSmartPointer<albaVMEGenericAbstractConcrete> vme;

  vme->SetMatrix(matrix1);
  vme->SetMatrix(matrix2);

  vme->SetData(item1); // This is not a method of albaVMEGenericAbstract!
  vme->SetData(item2); // This is not a method of albaVMEGenericAbstract!

  std::vector<albaTimeStamp> kframes;
  vme->GetLocalTimeStamps(kframes);
  result = (kframes[0] == 0. && kframes[1] == 1. && kframes[2] == 2.); // Timestamps list is obtained merging timestamps for matrices and items
  TEST_RESULT;

  kframes.clear();
  albaDEL(item1);
  albaDEL(item2);
}
//----------------------------------------------------------------------------
void albaVMEGenericAbstractTest::TestGetLocalTimeBounds()
//----------------------------------------------------------------------------
{
  // Prepare matrices to add to the MatrixVector
albaMatrix matrix1, matrix2;
  matrix1.SetElement(0,0,.1);
  matrix1.SetElement(0,1,.2);
  matrix1.SetElement(0,2,.3);
  matrix1.SetElement(0,3,.4);

  matrix1.SetElement(1,0,.5);
  matrix1.SetElement(1,1,.6);
  matrix1.SetElement(1,2,.7);
  matrix1.SetElement(1,3,.8);

  matrix1.SetElement(2,0,.9);
  matrix1.SetElement(2,1,1.0);
  matrix1.SetElement(2,2,1.1);
  matrix1.SetElement(2,3,1.2);

  matrix1.SetElement(3,0,1.3);
  matrix1.SetElement(3,1,1.4);
  matrix1.SetElement(3,2,1.5);
  matrix1.SetElement(3,3,1.6);

  matrix2.SetElement(0,0,1.8);
  matrix2.SetElement(0,1,1.9);
  matrix2.SetElement(0,2,2.0);
  matrix2.SetElement(0,3,2.1);

  matrix2.SetElement(1,0,2.2);
  matrix2.SetElement(1,1,2.3);
  matrix2.SetElement(1,2,2.4);
  matrix2.SetElement(1,3,2.5);

  matrix2.SetElement(2,0,2.6);
  matrix2.SetElement(2,1,2.7);
  matrix2.SetElement(2,2,2.8);
  matrix2.SetElement(2,3,2.9);

  matrix2.SetElement(3,0,3.0);
  matrix2.SetElement(3,1,3.1);
  matrix2.SetElement(3,2,3.2);
  matrix2.SetElement(3,3,3.3);

  matrix1.SetTimeStamp(1.);
  matrix2.SetTimeStamp(2.);
  albaVMEItemVTK *item1, *item2;
  albaNEW(item1);
  albaNEW(item2);
  item1->SetTimeStamp(0.);
  item2->SetTimeStamp(2.);

  albaSmartPointer<albaVMEGenericAbstractConcrete> vme;

  vme->SetMatrix(matrix1);
  vme->SetMatrix(matrix2);

  vme->SetData(item1); // This is not a method of albaVMEGenericAbstract!
  vme->SetData(item2); // This is not a method of albaVMEGenericAbstract!

  albaTimeStamp bounds[2];
  vme->GetLocalTimeBounds(bounds);
  result = (bounds[0] == 0. && bounds[1] == 2.);
  TEST_RESULT;

  albaDEL(item1);
  albaDEL(item2);
}
//----------------------------------------------------------------------------
void albaVMEGenericAbstractTest::TestIsAnimated()
//----------------------------------------------------------------------------
{
  // Prepare matrices to add to the MatrixVector
albaMatrix matrix1, matrix2;
  matrix1.SetElement(0,0,.1);
  matrix1.SetElement(0,1,.2);
  matrix1.SetElement(0,2,.3);
  matrix1.SetElement(0,3,.4);

  matrix1.SetElement(1,0,.5);
  matrix1.SetElement(1,1,.6);
  matrix1.SetElement(1,2,.7);
  matrix1.SetElement(1,3,.8);

  matrix1.SetElement(2,0,.9);
  matrix1.SetElement(2,1,1.0);
  matrix1.SetElement(2,2,1.1);
  matrix1.SetElement(2,3,1.2);

  matrix1.SetElement(3,0,1.3);
  matrix1.SetElement(3,1,1.4);
  matrix1.SetElement(3,2,1.5);
  matrix1.SetElement(3,3,1.6);

  matrix2.SetElement(0,0,1.8);
  matrix2.SetElement(0,1,1.9);
  matrix2.SetElement(0,2,2.0);
  matrix2.SetElement(0,3,2.1);

  matrix2.SetElement(1,0,2.2);
  matrix2.SetElement(1,1,2.3);
  matrix2.SetElement(1,2,2.4);
  matrix2.SetElement(1,3,2.5);

  matrix2.SetElement(2,0,2.6);
  matrix2.SetElement(2,1,2.7);
  matrix2.SetElement(2,2,2.8);
  matrix2.SetElement(2,3,2.9);

  matrix2.SetElement(3,0,3.0);
  matrix2.SetElement(3,1,3.1);
  matrix2.SetElement(3,2,3.2);
  matrix2.SetElement(3,3,3.3);

  matrix1.SetTimeStamp(1.);
  matrix2.SetTimeStamp(2.);

  albaVMEGenericAbstractConcrete *vme;

  //Testing with matrices
  albaNEW(vme);

  vme->SetMatrix(matrix1);
  result = !vme->IsAnimated();
  TEST_RESULT;

  vme->SetMatrix(matrix2);
  result = vme->IsAnimated();
  TEST_RESULT;

  albaDEL(vme);

  // Prepare data to add to the DataVector
  albaVMEItemVTK *item1, *item2;
  albaNEW(item1);
  albaNEW(item2);
  item1->SetTimeStamp(0.);
  item2->SetTimeStamp(2.);

  //Testing with items
  albaNEW(vme);

  vme->SetData(item1); // This is not a method of albaVMEGenericAbstract!
  result = !vme->IsAnimated();
  TEST_RESULT;

  vme->SetData(item2); // This is not a method of albaVMEGenericAbstract!
  result = vme->IsAnimated();
  TEST_RESULT;

  albaDEL(item1);
  albaDEL(item2);

  albaDEL(vme);
}
//----------------------------------------------------------------------------
void albaVMEGenericAbstractTest::TestIsDataAvailable()
//----------------------------------------------------------------------------
{
  // Prepare data to add to the DataVector
  // Create dummy VME to get DataSets
  albaVMESurfaceParametric *sphere, *cube;
  albaNEW(sphere);
  albaNEW(cube);
  sphere->SetGeometryType(PARAMETRIC_SPHERE);
  cube->SetGeometryType(PARAMETRIC_CUBE);

  albaVMEItemVTK *item1, *item2;
  albaNEW(item1);
  albaNEW(item2);
  item1->SetData(sphere->GetOutput()->GetVTKData());
  item2->SetData(cube->GetOutput()->GetVTKData());
  item1->SetTimeStamp(1.);
  item2->SetTimeStamp(2.);

  albaSmartPointer<albaVMEGenericAbstractConcrete> vme;

  vme->SetTimeStamp((albaTimeStamp)1.);
  //result = !vme->IsDataAvailable();
  //TEST_RESULT;

  vme->SetData(item1); // This is not a method of albaVMEGenericAbstract!
  vme->SetData(item2); // This is not a method of albaVMEGenericAbstract!

  result = vme->IsDataAvailable();
  TEST_RESULT;

  albaDEL(item1);
  albaDEL(item2);
  albaDEL(sphere);
  albaDEL(cube);
}