/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpTransformInterfaceTest
 Authors: Roberto Mucci
 
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
#include "albaOpTransformInterfaceTest.h"

#include "albaOpTransformInterface.h"
#include "albaVMEVolumeGray.h"
#include "albaVMESurface.h"
#include "albaVMEExternalData.h"
#include "albaEvent.h"
#include "albaMatrix.h"
#include "albaInteractorGenericMouse.h"
#include "albaTransform.h"

#include "vtkALBASmartPointer.h"
#include "vtkCubeSource.h"
#include "vtkPolyData.h"

#define DELTA 0.00001

//----------------------------------------------------------------------------
void albaOpTransformInterfaceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpTransformInterfaceTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaOpTransformInterface op;
}
//----------------------------------------------------------------------------
void albaOpTransformInterfaceTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaOpTransformInterface *op = new albaOpTransformInterface();
  albaDEL(op);
}

//----------------------------------------------------------------------------
void albaOpTransformInterfaceTest::TestPostMultiplyEventMatrix()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMESurface> input;
  vtkALBASmartPointer<vtkCubeSource> cube;
  cube->Update();

  input->SetData(cube->GetOutput(),0.0);
  input->GetOutput()->Update();
  input->Update();

  albaSmartPointer<albaMatrix> translateMatrix;
  translateMatrix->SetElement(0,3,10);
  translateMatrix->SetElement(1,3,5);
  translateMatrix->SetElement(2,3,20);
  translateMatrix->SetElement(3,3,1);

  albaEvent *e = new albaEvent(this,ID_TRANSFORM);
  e->SetArg(albaInteractorGenericMouse::MOUSE_MOVE);
  e->SetMatrix(translateMatrix);

  albaOpTransformInterface *op = new albaOpTransformInterface();
  op->TestModeOn();
  op->SetInput(input);
  op->PostMultiplyEventMatrix(e);

  albaVMESurface *output = albaVMESurface::SafeDownCast(op->GetInput());
  output->GetOutput()->Update();
  output->Update();

  albaMatrix *matrixOutput = output->GetOutput()->GetAbsMatrix();
  double a = matrixOutput->GetElement(0,3);
  CPPUNIT_ASSERT( matrixOutput->GetElement(0,3) == translateMatrix->GetElement(0,3) );
  CPPUNIT_ASSERT( matrixOutput->GetElement(1,3) == translateMatrix->GetElement(1,3) );
  CPPUNIT_ASSERT( matrixOutput->GetElement(2,3) == translateMatrix->GetElement(2,3) );
  CPPUNIT_ASSERT( matrixOutput->GetElement(3,3) == translateMatrix->GetElement(3,3) );

  albaDEL(op);

  delete e;
}
//----------------------------------------------------------------------------
void albaOpTransformInterfaceTest::TestOpDo()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMESurface> input;
  vtkALBASmartPointer<vtkCubeSource> cube;
  cube->Update();

  input->SetData(cube->GetOutput(),0.0);
  input->GetOutput()->Update();
  input->Update();

  albaMatrix rotateMatrix;
  albaTransform::SetOrientation(rotateMatrix,10,20,30);

  albaEvent *e = new albaEvent(this,ID_TRANSFORM);
  e->SetArg(albaInteractorGenericMouse::MOUSE_MOVE);
  e->SetMatrix(&rotateMatrix);

  albaOpTransformInterface *op = new albaOpTransformInterface();
  op->TestModeOn();
  op->SetInput(input);
  op->PostMultiplyEventMatrix(e);
  op->OpDo();

  albaVMESurface *output = albaVMESurface::SafeDownCast(op->GetInput());
  output->GetOutput()->Update();
  output->Update();

  albaMatrix *matrixOutput = output->GetOutput()->GetAbsMatrix();

  for(int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    {
      CPPUNIT_ASSERT( matrixOutput->GetElement(i,j) + DELTA > rotateMatrix.GetElement(i,j) && matrixOutput->GetElement(i,j) - DELTA < rotateMatrix.GetElement(i,j) );
    }
  }

  albaDEL(op);

  delete e;
}


