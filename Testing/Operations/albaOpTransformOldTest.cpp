/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpTransformOldTest
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
#include "albaOpTransformOldTest.h"

#include "albaOpTransformOld.h"
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
void albaOpTransformOldTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaOpTransformOldTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaOpTransformOld op;
}
//----------------------------------------------------------------------------
void albaOpTransformOldTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaOpTransformOld *op = new albaOpTransformOld();
  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpTransformOldTest::TestAccept()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEVolumeGray> volume;
  albaSmartPointer<albaVMESurface> surface;
  albaSmartPointer<albaVMEExternalData> external;

  albaOpTransformOld *op = new albaOpTransformOld();
  CPPUNIT_ASSERT( op->Accept(volume) == true );
  CPPUNIT_ASSERT( op->Accept(surface) == true );
  CPPUNIT_ASSERT( op->Accept(external) == false );

  albaDEL(op);

}
//----------------------------------------------------------------------------
void albaOpTransformOldTest::TestOnEventGizmoTranslate()
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

  albaOpTransformOld *op = new albaOpTransformOld();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();
  op->OnEventGizmoTranslate(e);
  op->OpDo();

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
void albaOpTransformOldTest::TestOnEventGizmoRotate()
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

  albaOpTransformOld *op = new albaOpTransformOld();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();
  op->OnEventGizmoRotate(e);
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
//----------------------------------------------------------------------------
void albaOpTransformOldTest::TestOnEventGizmoScale()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMESurface> input;
  vtkALBASmartPointer<vtkCubeSource> cube;
  cube->Update();

  input->SetData(cube->GetOutput(),0.0);
  input->GetOutput()->Update();
  input->Update();

  albaMatrix scaleMatrix;
  albaTransform::Scale(scaleMatrix,10,20,30,0);

  albaOpTransformOld *op = new albaOpTransformOld();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();
  op->SetNewAbsMatrix(scaleMatrix);
  op->OpDo();

  albaVMESurface *output = albaVMESurface::SafeDownCast(op->GetInput());
  output->GetOutput()->Update();
  output->Update();

  vtkPolyData *polydataOutput = vtkPolyData::SafeDownCast(output->GetOutput()->GetVTKData());
  polydataOutput->Update();
  for (int i=0;i<cube->GetOutput()->GetNumberOfPoints();i++)
  {
    double b = cube->GetOutput()->GetPoint(i)[0];
    double a = polydataOutput->GetPoint(i)[0];
    CPPUNIT_ASSERT( cube->GetOutput()->GetPoint(i)[0] == (polydataOutput->GetPoint(i)[0]/10) );
    CPPUNIT_ASSERT( cube->GetOutput()->GetPoint(i)[1] == (polydataOutput->GetPoint(i)[1]/20) );
    CPPUNIT_ASSERT( cube->GetOutput()->GetPoint(i)[2] == (polydataOutput->GetPoint(i)[2]/30) );
  }

  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpTransformOldTest::TestOnEventGuiTransform()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMESurface> input;
  vtkALBASmartPointer<vtkCubeSource> cube;
  cube->Update();

  input->SetData(cube->GetOutput(),0.0);
  input->GetOutput()->Update();
  input->Update();

  albaMatrix inputMmatrix;
  albaTransform::SetOrientation(inputMmatrix,10,20,30);
  albaTransform::SetPosition(inputMmatrix,100,-20,60);

  albaEvent *e = new albaEvent(this,ID_TRANSFORM);
  e->SetArg(albaInteractorGenericMouse::MOUSE_MOVE);
  e->SetMatrix(&inputMmatrix);

  albaOpTransformOld *op = new albaOpTransformOld();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();
  op->OnEventGuiTransform(e);
  op->OpDo();

  albaVMESurface *output = albaVMESurface::SafeDownCast(op->GetInput());
  output->GetOutput()->Update();
  output->Update();

  albaMatrix *matrixOutput = output->GetOutput()->GetAbsMatrix();

  for(int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    {
      CPPUNIT_ASSERT( matrixOutput->GetElement(i,j) + DELTA > inputMmatrix.GetElement(i,j) && matrixOutput->GetElement(i,j) - DELTA < inputMmatrix.GetElement(i,j) );
    }
  }

  delete e;

  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpTransformOldTest::TestOnEventGuiSaveRestorePose()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMESurface> input;
  vtkALBASmartPointer<vtkCubeSource> cube;
  cube->Update();

  input->SetData(cube->GetOutput(),0.0);
  input->GetOutput()->Update();
  input->Update();

  albaMatrix inputMmatrix;
  albaTransform::SetOrientation(inputMmatrix,10,20,30);
  albaTransform::SetPosition(inputMmatrix,100,-20,60);

  albaEvent *e = new albaEvent(this,ID_TRANSFORM);
  e->SetArg(albaInteractorGenericMouse::MOUSE_MOVE);
  e->SetMatrix(&inputMmatrix);

  albaOpTransformOld *op = new albaOpTransformOld();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();
  op->OnEventGuiTransform(e);
  op->OpDo();

  albaVMESurface *output = albaVMESurface::SafeDownCast(op->GetInput());
  output->GetOutput()->Update();
  output->Update();

  albaMatrix *matrixOutput = output->GetOutput()->GetAbsMatrix();

  for(int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    {
      CPPUNIT_ASSERT( matrixOutput->GetElement(i,j) + DELTA > inputMmatrix.GetElement(i,j) && matrixOutput->GetElement(i,j) - DELTA < inputMmatrix.GetElement(i,j) );
    }
  }

  delete e;

  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpTransformOldTest::TestReset()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMESurface> input;
  vtkALBASmartPointer<vtkCubeSource> cube;
  cube->Update();

  input->SetData(cube->GetOutput(),0.0);
  input->GetOutput()->Update();
  input->Update();

  albaMatrix inputMmatrix;
  albaTransform::SetOrientation(inputMmatrix,10,20,30);
  albaTransform::SetPosition(inputMmatrix,100,-20,60);

  albaEvent *e = new albaEvent(this,ID_TRANSFORM);
  e->SetArg(albaInteractorGenericMouse::MOUSE_MOVE);
  e->SetMatrix(&inputMmatrix);

  albaOpTransformOld *op = new albaOpTransformOld();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();
  op->OnEventGuiTransform(e);
  op->Reset();
  op->OpDo();

  albaVMESurface *output = albaVMESurface::SafeDownCast(op->GetInput());
  output->GetOutput()->Update();
  output->Update();

  albaMatrix *matrixOutput = output->GetOutput()->GetAbsMatrix();

  albaSmartPointer<albaMatrix> identity;
  identity->Identity();

  for(int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    {
      CPPUNIT_ASSERT( matrixOutput->GetElement(i,j) == identity->GetElement(i,j) );
    }
  }

  delete e;

  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpTransformOldTest::TestOpUndo()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMESurface> input;
  vtkALBASmartPointer<vtkCubeSource> cube;
  cube->Update();

  input->SetData(cube->GetOutput(),0.0);
  input->GetOutput()->Update();
  input->Update();

  albaMatrix inputMmatrix;
  albaTransform::SetOrientation(inputMmatrix,10,20,30);
  albaTransform::SetPosition(inputMmatrix,100,-20,60);

  albaEvent *e = new albaEvent(this,ID_TRANSFORM);
  e->SetArg(albaInteractorGenericMouse::MOUSE_MOVE);
  e->SetMatrix(&inputMmatrix);

  albaOpTransformOld *op = new albaOpTransformOld();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();
  op->OnEventGuiTransform(e);
  op->OpDo();
  op->OpUndo();

  albaVMESurface *output = albaVMESurface::SafeDownCast(op->GetInput());
  output->GetOutput()->Update();
  output->Update();

  albaMatrix *matrixOutput = output->GetOutput()->GetAbsMatrix();

  albaSmartPointer<albaMatrix> identity;
  identity->Identity();

  for(int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    {
      CPPUNIT_ASSERT( matrixOutput->GetElement(i,j) == identity->GetElement(i,j) );
    }
  }

  delete e;

  albaDEL(op);
}
