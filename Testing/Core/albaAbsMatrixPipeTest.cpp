/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAbsMatrixPipeTest
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
#include "albaAbsMatrixPipeTest.h"
#include "albaAbsMatrixPipe.h"
#include "albaVMESurface.h"
#include "albaMatrix.h"
#include "albaTransform.h"

#include "vtkALBASmartPointer.h"
#include "vtkDataSet.h"
#include "vtkCubeSource.h"
#include "vtkSphereSource.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaAbsMatrixPipeTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaAbsMatrixPipeTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaAbsMatrixPipe matrix;
}

//----------------------------------------------------------------------------
void albaAbsMatrixPipeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaAbsMatrixPipe *matrix1 = new albaAbsMatrixPipe();
  delete matrix1;

  albaSmartPointer<albaAbsMatrixPipe> matrix2;
  
  albaAbsMatrixPipe *matrix3;
  albaNEW(matrix3);
  albaDEL(matrix3);
}
//----------------------------------------------------------------------------
void albaAbsMatrixPipeTest::TestSetVME()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaAbsMatrixPipe> matrixPipe;
  vtkALBASmartPointer<vtkCubeSource> cubeVTK;
  albaSmartPointer<albaVMESurface> cubeVME;

  //Test Set/Get
  cubeVME->SetData(cubeVTK->GetOutput(),0.0);
  cubeVME->GetOutput()->Update();
  cubeVME->Update();

  matrixPipe->SetVME(cubeVME);

  result = (albaVME *)cubeVME == matrixPipe->GetVME();

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test without a parent
  albaMatrix matrixCube;
  albaTransform::Translate(matrixCube,10,20,30,POST_MULTIPLY);
  albaTransform::RotateX(matrixCube,10,POST_MULTIPLY);
  albaTransform::RotateY(matrixCube,10,POST_MULTIPLY);
  albaTransform::RotateZ(matrixCube,10,POST_MULTIPLY);

  cubeVME->SetMatrix(matrixCube);
  matrixPipe->Update();
  const albaMatrix matrixABSPipe = matrixPipe->GetMatrix();

  result = matrixCube.Equals(&matrixABSPipe);

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test with a parent
  vtkALBASmartPointer<vtkSphereSource> sphereVTK;
  albaSmartPointer<albaVMESurface> sphereVME;

  sphereVME->SetData(sphereVTK->GetOutput(),0.0);
  sphereVME->GetOutput()->Update();
  sphereVME->Update();
  
  albaMatrix matrixSphere;
  albaTransform::Translate(matrixSphere,5,25,-15,POST_MULTIPLY);
  albaTransform::RotateX(matrixSphere,5,POST_MULTIPLY);
  albaTransform::RotateY(matrixSphere,5,POST_MULTIPLY);
  albaTransform::RotateZ(matrixSphere,5,POST_MULTIPLY);

  sphereVME->SetMatrix(matrixSphere);
  sphereVME->GetOutput()->Update();
  sphereVME->Update();

  cubeVME->ReparentTo(sphereVME);
  sphereVME->GetOutput()->Update();
  sphereVME->Update();

  matrixPipe->SetVME(cubeVME);
  matrixPipe->Update();

  const albaMatrix matrixABSPipe2 = matrixPipe->GetMatrix();

  result = !matrixCube.Equals(&matrixABSPipe2);
  
  TEST_RESULT;
  
  albaMatrix matrixResult;
  albaMatrix::Multiply4x4(matrixCube,matrixSphere,matrixResult);

  result = !matrixResult.Equals(&matrixABSPipe2);

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test changed matrix of the parent
  albaTransform::Translate(matrixSphere,25,-25,10,POST_MULTIPLY);
  albaTransform::RotateX(matrixSphere,15,POST_MULTIPLY);
  albaTransform::RotateY(matrixSphere,25,POST_MULTIPLY);
  albaTransform::RotateZ(matrixSphere,35,POST_MULTIPLY);

  const albaMatrix matrixABSPipe3 = matrixPipe->GetMatrix();

  result = !matrixResult.Equals(&matrixABSPipe3);

  TEST_RESULT;

  albaMatrix::Multiply4x4(matrixCube,matrixSphere,matrixResult);

  result = !matrixResult.Equals(&matrixABSPipe3);

  TEST_RESULT;
}
