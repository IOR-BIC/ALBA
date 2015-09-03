/*=========================================================================

 Program: MAF2
 Module: mafVMEGenericAbstractTest
 Authors: Alberto Losi
 
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
#include "mafVMEGenericAbstractTest.h"
#include "mafVMEGenericAbstract.h"
#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafSmartPointer.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMEItem.h"
#include "mafVMEItemVTK.h"
#include "mafMatrixVector.h"
#include "mafDataVector.h"
#include "mafNodeGeneric.h"
#include "mafVMEOutputNULL.h"
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
class mafVMEGenericAbstractConcrete: public mafVMEGenericAbstract
//----------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafVMEGenericAbstractConcrete,mafVMEGenericAbstract);
  void SetData(mafVMEItem *item); // Add an item to the DataVector. Implemented for test GetDataVector() method (and related)
protected:
  mafVMEGenericAbstractConcrete();
  virtual ~mafVMEGenericAbstractConcrete();
  mafTransform *m_Transform;
};

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEGenericAbstractConcrete);
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
mafVMEGenericAbstractConcrete::mafVMEGenericAbstractConcrete()
:mafVMEGenericAbstract()
//----------------------------------------------------------------------------
{
  m_DataVector = new mafDataVector();
  m_DataVector->SetListener(this);

  mafNEW(m_Transform);
  mafVMEOutputNULL *output = mafVMEOutputNULL::New();
  output->SetTransform(m_Transform);
  SetOutput(output);
}
//----------------------------------------------------------------------------
mafVMEGenericAbstractConcrete::~mafVMEGenericAbstractConcrete()
//----------------------------------------------------------------------------
{
  mafDEL(m_Transform);
  cppDEL(m_DataVector);
}
//----------------------------------------------------------------------------
void mafVMEGenericAbstractConcrete::SetData(mafVMEItem *item)
//----------------------------------------------------------------------------
{
  m_DataVector->AppendItem(item);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void mafVMEGenericAbstractTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafVMEGenericAbstractTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEGenericAbstractConcrete> vme1;
  mafVMEGenericAbstractConcrete *vme2;
  mafNEW(vme2);
  mafDEL(vme2);
}

//----------------------------------------------------------------------------
void mafVMEGenericAbstractTest::TestAllConstructor()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEGenericAbstractConcrete> vme1;
  result = vme1->GetReferenceCount() == 1; // mafSmartPointer increase the reference count
  TEST_RESULT;

  mafVMEGenericAbstractConcrete *vme2 = NULL;
  vme2 = mafVMEGenericAbstractConcrete::New();
  result = vme2->GetReferenceCount() == 0; // default reference count on New
  TEST_RESULT;
  mafDEL(vme2);

  mafVMEGenericAbstractConcrete *vme3 = NULL;
  mafNEW(vme3);
  result = vme3->GetReferenceCount() == 1; // mafNEW macro increase the reference count
  TEST_RESULT;
  mafDEL(vme3);
}
//----------------------------------------------------------------------------
void mafVMEGenericAbstractTest::TestDeepCopy()
//----------------------------------------------------------------------------
{
  // Prepare matrices to add to the MatrixVector
  mafMatrix matrix1, matrix2;
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
  mafVMESurfaceParametric *sphere, *cube;
  mafNEW(sphere);
  mafNEW(cube);
  sphere->SetGeometryType(PARAMETRIC_SPHERE);
  cube->SetGeometryType(PARAMETRIC_CUBE);

  mafVMEItemVTK *item1, *item2;
  mafNEW(item1);
  mafNEW(item2);
  item1->SetData(sphere->GetOutput()->GetVTKData());
  item2->SetData(cube->GetOutput()->GetVTKData());
  item1->SetTimeStamp(1.);
  item2->SetTimeStamp(2.);

  mafSmartPointer<mafVMEGenericAbstractConcrete> vme1;

  vme1->SetName("mafVMEGenericAbstract_Test");
  vme1->SetTimeStamp(0.);

  vme1->SetMatrix(matrix1);
  vme1->SetMatrix(matrix2);

  vme1->SetData(item1); // This is not a method of mafVMEGenericAbstract!
  vme1->SetData(item2); // This is not a method of mafVMEGenericAbstract!

  mafSmartPointer<mafVMEGenericAbstractConcrete> vme2;
 
  vme2->DeepCopy(vme1);

  result = vme1->Equals(vme2);
  TEST_RESULT;

  mafDEL(item1);
  mafDEL(item2);
  mafDEL(sphere);
  mafDEL(cube);
}
//----------------------------------------------------------------------------
void mafVMEGenericAbstractTest::TestShallowCopy()
//----------------------------------------------------------------------------
{
  // Prepare matrices to add to the MatrixVector
  mafMatrix matrix1, matrix2;
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
  mafVMESurfaceParametric *sphere, *cube;
  mafNEW(sphere);
  mafNEW(cube);
  sphere->SetGeometryType(PARAMETRIC_SPHERE);
  cube->SetGeometryType(PARAMETRIC_CUBE);

  mafVMEItemVTK *item1, *item2;
  mafNEW(item1);
  mafNEW(item2);
  item1->SetData(sphere->GetOutput()->GetVTKData());
  item2->SetData(cube->GetOutput()->GetVTKData());
  item1->SetTimeStamp(1.);
  item2->SetTimeStamp(2.);

  mafSmartPointer<mafVMEGenericAbstractConcrete> vme1;

  vme1->SetName("mafVMEGenericAbstract_Test");
  vme1->SetTimeStamp(0.);

  vme1->SetMatrix(matrix1);
  vme1->SetMatrix(matrix2);

  vme1->SetData(item1); // This is not a method of mafVMEGenericAbstract!
  vme1->SetData(item2); // This is not a method of mafVMEGenericAbstract!

  mafSmartPointer<mafVMEGenericAbstractConcrete> vme2;

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

  mafDEL(item1);
  mafDEL(item2);
  mafDEL(sphere);
  mafDEL(cube);
}
//----------------------------------------------------------------------------
void mafVMEGenericAbstractTest::TestEquals()
//----------------------------------------------------------------------------
{
  // Prepare matrices to add to the MatrixVector
  mafMatrix matrix1A;
  mafMatrix matrix2A;
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

  mafMatrix matrix1B;
  mafMatrix matrix2B;

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
  mafVMESurfaceParametric *sphere, *cube, *cone, *cylinder;
  mafNEW(sphere);
  mafNEW(cube);
  mafNEW(cone);
  mafNEW(cylinder);
  sphere->SetGeometryType(PARAMETRIC_SPHERE);
  cube->SetGeometryType(PARAMETRIC_CUBE);
  cone->SetGeometryType(PARAMETRIC_CONE);
  cylinder->SetGeometryType(PARAMETRIC_CYLINDER);

  mafVMEItemVTK *item1A, *item2A, *item1B, *item2B;
  mafNEW(item1A);
  mafNEW(item2A);
  mafNEW(item1B);
  mafNEW(item2B);
  item1A->SetData(sphere->GetOutput()->GetVTKData());
  item2A->SetData(cube->GetOutput()->GetVTKData());
  item1B->SetData(cone->GetOutput()->GetVTKData());
  item2B->SetData(cylinder->GetOutput()->GetVTKData());
  item1A->SetTimeStamp(1.);
  item2A->SetTimeStamp(2.);
  item1B->SetTimeStamp(3.);
  item2B->SetTimeStamp(4.);

  mafSmartPointer<mafVMEGenericAbstractConcrete> vme1;
  vme1->SetName("mafVMEGenericAbstract_Test");
  vme1->SetTimeStamp(0.);

  vme1->SetMatrix(matrix1A);
  vme1->SetMatrix(matrix2A);

  vme1->SetData(item1A); // This is not a method of mafVMEGenericAbstract!
  vme1->SetData(item2A); // This is not a method of mafVMEGenericAbstract!

  mafVMEGenericAbstractConcrete *vme2;
  mafNEW(vme2);
  // Different MatrixVector and DataVector
  vme2->SetName("mafVMEGenericAbstract_Test");
  vme2->SetTimeStamp(0.);

  vme2->SetMatrix(matrix1B);
  vme2->SetMatrix(matrix2B);

  vme2->SetData(item1B); // This is not a method of mafVMEGenericAbstract!
  vme2->SetData(item2B); // This is not a method of mafVMEGenericAbstract!

  result = !vme1->Equals(vme2);
  mafDEL(vme2);
  TEST_RESULT;

  // Same MatrixVector and different DataVector
  mafNEW(vme2);
  vme2->SetMatrix(matrix1A);
  vme2->SetMatrix(matrix2A);

  vme2->SetData(item1B); // This is not a method of mafVMEGenericAbstract!
  vme2->SetData(item2B); // This is not a method of mafVMEGenericAbstract!

  result = !vme1->Equals(vme2);
  mafDEL(vme2);
  TEST_RESULT;

  // Different MatrixVector and same DataVector
  mafNEW(vme2);
  vme2->SetMatrix(matrix1B);
  vme2->SetMatrix(matrix2B);

  vme2->SetData(item1A); // This is not a method of mafVMEGenericAbstract!
  vme2->SetData(item2A); // This is not a method of mafVMEGenericAbstract!

  result = !vme1->Equals(vme2);
  mafDEL(vme2);
  TEST_RESULT;

  // Same MatrixVector and same DataVector
  mafNEW(vme2);
  vme2->SetMatrix(matrix1A);
  vme2->SetMatrix(matrix2A);

  vme2->SetData(item1A); // This is not a method of mafVMEGenericAbstract!
  vme2->SetData(item2A); // This is not a method of mafVMEGenericAbstract!

  result = !vme1->Equals(vme2);
  TEST_RESULT;

  mafDEL(vme2);

  mafDEL(sphere);
  mafDEL(cube);
  mafDEL(cone);
  mafDEL(cylinder);

  mafDEL(item1A);
  mafDEL(item2A);
  mafDEL(item1B);
  mafDEL(item2B);
}
//----------------------------------------------------------------------------
void mafVMEGenericAbstractTest::TestReparentTo()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEGenericAbstractConcrete> vme;
  mafSmartPointer<mafVMEGenericAbstractConcrete> parent;

  vme->ReparentTo(parent);

  result = ((mafVMEGenericAbstractConcrete*)vme->GetParent() == (mafVMEGenericAbstractConcrete*)parent); // parent must be parent of vme
  TEST_RESULT;

  result = ((mafVMEGenericAbstractConcrete*)parent->GetFirstChild() == (mafVMEGenericAbstractConcrete*)vme); // vme must be first child of parent
  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafVMEGenericAbstractTest::TestGetMatrixVector()
//----------------------------------------------------------------------------
{
  // Prepare matrices to add to the MatrixVector
  mafMatrix matrix1, matrix2;
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

  mafSmartPointer<mafVMEGenericAbstractConcrete> vme;

  vme->SetMatrix(matrix1);
  vme->SetMatrix(matrix2);

  // Create a matrix vector with the same matrices
  mafMatrixVector *matrixVector = new mafMatrixVector();
  matrixVector->SetMatrix(matrix1);
  matrixVector->SetMatrix(matrix2);

  result = matrixVector->Equals(vme->GetMatrixVector()); // The two MatrixVector must be equals
  TEST_RESULT;

  cppDEL(matrixVector);
}
//----------------------------------------------------------------------------
void mafVMEGenericAbstractTest::TestSetMatrix()
//----------------------------------------------------------------------------
{
// Prepare matrices to add to the MatrixVector
  mafMatrix matrix;
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

  mafSmartPointer<mafVMEGenericAbstractConcrete> vme;

  vme->SetMatrix(matrix);

  // Create a matrix vector with the same matrices
  mafMatrixVector *matrixVector = new mafMatrixVector();
  matrixVector->SetMatrix(matrix);

  // GetMatrixVector is the only way to get inserted matrices
  result = matrixVector->Equals(vme->GetMatrixVector()); // The two MatrixVector must be equals
  TEST_RESULT;

  cppDEL(matrixVector);
}
//----------------------------------------------------------------------------
void mafVMEGenericAbstractTest::TestGetDataVector()
//----------------------------------------------------------------------------
{
  // Prepare data to add to the DataVector
  // Create dummy VME to get DataSets
  mafVMESurfaceParametric *sphere, *cube;
  mafNEW(sphere);
  mafNEW(cube);
  sphere->SetGeometryType(PARAMETRIC_SPHERE);
  cube->SetGeometryType(PARAMETRIC_CUBE);

  mafVMEItemVTK *item1, *item2;
  mafNEW(item1);
  mafNEW(item2);
  item1->SetData(sphere->GetOutput()->GetVTKData());
  item2->SetData(cube->GetOutput()->GetVTKData());
  item1->SetTimeStamp(1.);
  item2->SetTimeStamp(2.);

  mafSmartPointer<mafVMEGenericAbstractConcrete> vme;

  vme->SetData(item1); // This is not a method of mafVMEGenericAbstract!
  vme->SetData(item2); // This is not a method of mafVMEGenericAbstract!

  mafDataVector *dataVector = new mafDataVector();
  //mafNEW(dataVector);
  dataVector->InsertItem(item1);
  dataVector->InsertItem(item2);

  result = dataVector->Equals(vme->GetDataVector()); // The two DataVector must be equals
  TEST_RESULT;

  cppDEL(dataVector);
  mafDEL(item1);
  mafDEL(item2);
  mafDEL(sphere);
  mafDEL(cube);
}
//----------------------------------------------------------------------------
void mafVMEGenericAbstractTest::TestGetDataTimeStamps()
//----------------------------------------------------------------------------
{
  // Prepare data to add to the DataVector
  mafVMEItemVTK *item1, *item2;
  mafNEW(item1);
  mafNEW(item2);
  item1->SetTimeStamp(1.);
  item2->SetTimeStamp(2.);

  mafSmartPointer<mafVMEGenericAbstractConcrete> vme;

  vme->SetData(item1); // This is not a method of mafVMEGenericAbstract!
  vme->SetData(item2); // This is not a method of mafVMEGenericAbstract!

  std::vector<mafTimeStamp> kframes;
  vme->GetDataTimeStamps(kframes);
  result = (kframes[0] == 1. && kframes[1] == 2.);

  mafDEL(item1);
  mafDEL(item2);
  TEST_RESULT;

  kframes.clear();
}
//----------------------------------------------------------------------------
void mafVMEGenericAbstractTest::TestGetMatrixTimeStamps()
//----------------------------------------------------------------------------
{
  // Prepare matrices to add to the MatrixVector
  mafMatrix matrix1, matrix2;
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

  mafSmartPointer<mafVMEGenericAbstractConcrete> vme;

  vme->SetMatrix(matrix1);
  vme->SetMatrix(matrix2);

  std::vector<mafTimeStamp> kframes;
  vme->GetMatrixTimeStamps(kframes);
  result = (kframes[0] == 1. && kframes[1] == 2.);
  TEST_RESULT;

  kframes.clear();
}
//----------------------------------------------------------------------------
void mafVMEGenericAbstractTest::TestGetLocalTimeStamps()
//----------------------------------------------------------------------------
{
  // Prepare matrices to add to the MatrixVector
mafMatrix matrix1, matrix2;
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
  mafVMEItemVTK *item1, *item2;
  mafNEW(item1);
  mafNEW(item2);
  item1->SetTimeStamp(0.);
  item2->SetTimeStamp(2.);

  mafSmartPointer<mafVMEGenericAbstractConcrete> vme;

  vme->SetMatrix(matrix1);
  vme->SetMatrix(matrix2);

  vme->SetData(item1); // This is not a method of mafVMEGenericAbstract!
  vme->SetData(item2); // This is not a method of mafVMEGenericAbstract!

  std::vector<mafTimeStamp> kframes;
  vme->GetLocalTimeStamps(kframes);
  result = (kframes[0] == 0. && kframes[1] == 1. && kframes[2] == 2.); // Timestamps list is obtained merging timestamps for matrices and items
  TEST_RESULT;

  kframes.clear();
  mafDEL(item1);
  mafDEL(item2);
}
//----------------------------------------------------------------------------
void mafVMEGenericAbstractTest::TestGetLocalTimeBounds()
//----------------------------------------------------------------------------
{
  // Prepare matrices to add to the MatrixVector
mafMatrix matrix1, matrix2;
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
  mafVMEItemVTK *item1, *item2;
  mafNEW(item1);
  mafNEW(item2);
  item1->SetTimeStamp(0.);
  item2->SetTimeStamp(2.);

  mafSmartPointer<mafVMEGenericAbstractConcrete> vme;

  vme->SetMatrix(matrix1);
  vme->SetMatrix(matrix2);

  vme->SetData(item1); // This is not a method of mafVMEGenericAbstract!
  vme->SetData(item2); // This is not a method of mafVMEGenericAbstract!

  mafTimeStamp bounds[2];
  vme->GetLocalTimeBounds(bounds);
  result = (bounds[0] == 0. && bounds[1] == 2.);
  TEST_RESULT;

  mafDEL(item1);
  mafDEL(item2);
}
//----------------------------------------------------------------------------
void mafVMEGenericAbstractTest::TestIsAnimated()
//----------------------------------------------------------------------------
{
  // Prepare matrices to add to the MatrixVector
mafMatrix matrix1, matrix2;
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

  mafVMEGenericAbstractConcrete *vme;

  //Testing with matrices
  mafNEW(vme);

  vme->SetMatrix(matrix1);
  result = !vme->IsAnimated();
  TEST_RESULT;

  vme->SetMatrix(matrix2);
  result = vme->IsAnimated();
  TEST_RESULT;

  mafDEL(vme);

  // Prepare data to add to the DataVector
  mafVMEItemVTK *item1, *item2;
  mafNEW(item1);
  mafNEW(item2);
  item1->SetTimeStamp(0.);
  item2->SetTimeStamp(2.);

  //Testing with items
  mafNEW(vme);

  vme->SetData(item1); // This is not a method of mafVMEGenericAbstract!
  result = !vme->IsAnimated();
  TEST_RESULT;

  vme->SetData(item2); // This is not a method of mafVMEGenericAbstract!
  result = vme->IsAnimated();
  TEST_RESULT;

  mafDEL(item1);
  mafDEL(item2);

  mafDEL(vme);
}
//----------------------------------------------------------------------------
void mafVMEGenericAbstractTest::TestIsDataAvailable()
//----------------------------------------------------------------------------
{
  // Prepare data to add to the DataVector
  // Create dummy VME to get DataSets
  mafVMESurfaceParametric *sphere, *cube;
  mafNEW(sphere);
  mafNEW(cube);
  sphere->SetGeometryType(PARAMETRIC_SPHERE);
  cube->SetGeometryType(PARAMETRIC_CUBE);

  mafVMEItemVTK *item1, *item2;
  mafNEW(item1);
  mafNEW(item2);
  item1->SetData(sphere->GetOutput()->GetVTKData());
  item2->SetData(cube->GetOutput()->GetVTKData());
  item1->SetTimeStamp(1.);
  item2->SetTimeStamp(2.);

  mafSmartPointer<mafVMEGenericAbstractConcrete> vme;

  vme->SetTimeStamp((mafTimeStamp)1.);
  //result = !vme->IsDataAvailable();
  //TEST_RESULT;

  vme->SetData(item1); // This is not a method of mafVMEGenericAbstract!
  vme->SetData(item2); // This is not a method of mafVMEGenericAbstract!

  result = vme->IsDataAvailable();
  TEST_RESULT;

  mafDEL(item1);
  mafDEL(item2);
  mafDEL(sphere);
  mafDEL(cube);
}