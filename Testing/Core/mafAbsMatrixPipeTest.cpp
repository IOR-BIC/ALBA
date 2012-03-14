/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafAbsMatrixPipeTest.cpp,v $
Language:  C++
Date:      $Date: 2010-05-18 08:27:35 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2008
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafAbsMatrixPipeTest.h"
#include "mafAbsMatrixPipe.h"
#include "mafVMESurface.h"
#include "mafMatrix.h"
#include "mafTransform.h"

#include "vtkMAFSmartPointer.h"
#include "vtkDataSet.h"
#include "vtkCubeSource.h"
#include "vtkSphereSource.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafAbsMatrixPipeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafAbsMatrixPipeTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void mafAbsMatrixPipeTest::tearDown()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafAbsMatrixPipeTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafAbsMatrixPipe matrix;
}

//----------------------------------------------------------------------------
void mafAbsMatrixPipeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafAbsMatrixPipe *matrix1 = new mafAbsMatrixPipe();
  delete matrix1;

  mafSmartPointer<mafAbsMatrixPipe> matrix2;
  
  mafAbsMatrixPipe *matrix3;
  mafNEW(matrix3);
  mafDEL(matrix3);
}
//----------------------------------------------------------------------------
void mafAbsMatrixPipeTest::TestSetVME()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafAbsMatrixPipe> matrixPipe;
  vtkMAFSmartPointer<vtkCubeSource> cubeVTK;
  mafSmartPointer<mafVMESurface> cubeVME;

  //Test Set/Get
  cubeVME->SetData(cubeVTK->GetOutput(),0.0);
  cubeVME->GetOutput()->GetVTKData()->Update();
  cubeVME->GetOutput()->Update();
  cubeVME->Update();

  matrixPipe->SetVME(cubeVME);

  result = mafVME::SafeDownCast(cubeVME) == matrixPipe->GetVME();

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test without a parent
  mafMatrix matrixCube;
  mafTransform::Translate(matrixCube,10,20,30,POST_MULTIPLY);
  mafTransform::RotateX(matrixCube,10,POST_MULTIPLY);
  mafTransform::RotateY(matrixCube,10,POST_MULTIPLY);
  mafTransform::RotateZ(matrixCube,10,POST_MULTIPLY);

  cubeVME->SetMatrix(matrixCube);
  matrixPipe->Update();
  const mafMatrix matrixABSPipe = matrixPipe->GetMatrix();

  result = matrixCube.Equals(&matrixABSPipe);

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test with a parent
  vtkMAFSmartPointer<vtkSphereSource> sphereVTK;
  mafSmartPointer<mafVMESurface> sphereVME;

  sphereVME->SetData(sphereVTK->GetOutput(),0.0);
  sphereVME->GetOutput()->GetVTKData()->Update();
  sphereVME->GetOutput()->Update();
  sphereVME->Update();
  
  mafMatrix matrixSphere;
  mafTransform::Translate(matrixSphere,5,25,-15,POST_MULTIPLY);
  mafTransform::RotateX(matrixSphere,5,POST_MULTIPLY);
  mafTransform::RotateY(matrixSphere,5,POST_MULTIPLY);
  mafTransform::RotateZ(matrixSphere,5,POST_MULTIPLY);

  sphereVME->SetMatrix(matrixSphere);
  sphereVME->GetOutput()->Update();
  sphereVME->Update();

  cubeVME->ReparentTo(sphereVME);
  sphereVME->GetOutput()->Update();
  sphereVME->Update();

  matrixPipe->SetVME(cubeVME);
  matrixPipe->Update();

  const mafMatrix matrixABSPipe2 = matrixPipe->GetMatrix();

  result = !matrixCube.Equals(&matrixABSPipe2);
  
  TEST_RESULT;
  
  mafMatrix matrixResult;
  mafMatrix::Multiply4x4(matrixCube,matrixSphere,matrixResult);

  result = !matrixResult.Equals(&matrixABSPipe2);

  TEST_RESULT;
  //////////////////////////////////////////////////////////////////////////
  //Test changed matrix of the parent
  mafTransform::Translate(matrixSphere,25,-25,10,POST_MULTIPLY);
  mafTransform::RotateX(matrixSphere,15,POST_MULTIPLY);
  mafTransform::RotateY(matrixSphere,25,POST_MULTIPLY);
  mafTransform::RotateZ(matrixSphere,35,POST_MULTIPLY);

  const mafMatrix matrixABSPipe3 = matrixPipe->GetMatrix();

  result = !matrixResult.Equals(&matrixABSPipe3);

  TEST_RESULT;

  mafMatrix::Multiply4x4(matrixCube,matrixSphere,matrixResult);

  result = !matrixResult.Equals(&matrixABSPipe3);

  TEST_RESULT;
}
