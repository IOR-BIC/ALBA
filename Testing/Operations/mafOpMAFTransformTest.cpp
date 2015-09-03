/*=========================================================================

 Program: MAF2
 Module: mafOpMAFTransformTest
 Authors: Matteo Giacomoni
 
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
#include "mafOpMAFTransformTest.h"

#include "mafOpMAFTransform.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafVMEExternalData.h"
#include "mafEvent.h"
#include "mafMatrix.h"
#include "mafInteractorGenericMouse.h"
#include "mafTransform.h"

#include "vtkMAFSmartPointer.h"
#include "vtkCubeSource.h"
#include "vtkPolyData.h"

#define DELTA 0.00001

//----------------------------------------------------------------------------
void mafOpMAFTransformTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafOpMAFTransformTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafOpMAFTransform op;
}
//----------------------------------------------------------------------------
void mafOpMAFTransformTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafOpMAFTransform *op = new mafOpMAFTransform();
  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpMAFTransformTest::TestAccept()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volume;
  mafSmartPointer<mafVMESurface> surface;
  mafSmartPointer<mafVMEExternalData> external;

  mafOpMAFTransform *op = new mafOpMAFTransform();
  CPPUNIT_ASSERT( op->Accept(volume) == true );
  CPPUNIT_ASSERT( op->Accept(surface) == true );
  CPPUNIT_ASSERT( op->Accept(external) == false );

  mafDEL(op);

}
//----------------------------------------------------------------------------
void mafOpMAFTransformTest::TestOnEventGizmoTranslate()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMESurface> input;
  vtkMAFSmartPointer<vtkCubeSource> cube;
  cube->Update();

  input->SetData(cube->GetOutput(),0.0);
  input->GetOutput()->Update();
  input->Update();

  mafSmartPointer<mafMatrix> translateMatrix;
  translateMatrix->SetElement(0,3,10);
  translateMatrix->SetElement(1,3,5);
  translateMatrix->SetElement(2,3,20);
  translateMatrix->SetElement(3,3,1);

  mafEvent *e = new mafEvent(this,ID_TRANSFORM);
  e->SetArg(mafInteractorGenericMouse::MOUSE_MOVE);
  e->SetMatrix(translateMatrix);

  mafOpMAFTransform *op = new mafOpMAFTransform();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();
  op->OnEventGizmoTranslate(e);
  op->OpDo();

  mafVMESurface *output = mafVMESurface::SafeDownCast(op->GetInput());
  output->GetOutput()->Update();
  output->Update();

  mafMatrix *matrixOutput = output->GetOutput()->GetAbsMatrix();
  double a = matrixOutput->GetElement(0,3);
  CPPUNIT_ASSERT( matrixOutput->GetElement(0,3) == translateMatrix->GetElement(0,3) );
  CPPUNIT_ASSERT( matrixOutput->GetElement(1,3) == translateMatrix->GetElement(1,3) );
  CPPUNIT_ASSERT( matrixOutput->GetElement(2,3) == translateMatrix->GetElement(2,3) );
  CPPUNIT_ASSERT( matrixOutput->GetElement(3,3) == translateMatrix->GetElement(3,3) );

  mafDEL(op);

  delete e;
}
//----------------------------------------------------------------------------
void mafOpMAFTransformTest::TestOnEventGizmoRotate()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMESurface> input;
  vtkMAFSmartPointer<vtkCubeSource> cube;
  cube->Update();

  input->SetData(cube->GetOutput(),0.0);
  input->GetOutput()->Update();
  input->Update();

  mafMatrix rotateMatrix;
  mafTransform::SetOrientation(rotateMatrix,10,20,30);

  mafEvent *e = new mafEvent(this,ID_TRANSFORM);
  e->SetArg(mafInteractorGenericMouse::MOUSE_MOVE);
  e->SetMatrix(&rotateMatrix);

  mafOpMAFTransform *op = new mafOpMAFTransform();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();
  op->OnEventGizmoRotate(e);
  op->OpDo();

  mafVMESurface *output = mafVMESurface::SafeDownCast(op->GetInput());
  output->GetOutput()->Update();
  output->Update();

  mafMatrix *matrixOutput = output->GetOutput()->GetAbsMatrix();

  for(int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    {
      CPPUNIT_ASSERT( matrixOutput->GetElement(i,j) + DELTA > rotateMatrix.GetElement(i,j) && matrixOutput->GetElement(i,j) - DELTA < rotateMatrix.GetElement(i,j) );
    }
  }

  mafDEL(op);

  delete e;
}
//----------------------------------------------------------------------------
void mafOpMAFTransformTest::TestOnEventGizmoScale()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMESurface> input;
  vtkMAFSmartPointer<vtkCubeSource> cube;
  cube->Update();

  input->SetData(cube->GetOutput(),0.0);
  input->GetOutput()->Update();
  input->Update();

  mafMatrix scaleMatrix;
  mafTransform::Scale(scaleMatrix,10,20,30,0);

  mafOpMAFTransform *op = new mafOpMAFTransform();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();
  op->SetNewAbsMatrix(scaleMatrix);
  op->OpDo();

  mafVMESurface *output = mafVMESurface::SafeDownCast(op->GetInput());
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

  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpMAFTransformTest::TestOnEventGuiTransform()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMESurface> input;
  vtkMAFSmartPointer<vtkCubeSource> cube;
  cube->Update();

  input->SetData(cube->GetOutput(),0.0);
  input->GetOutput()->Update();
  input->Update();

  mafMatrix inputMmatrix;
  mafTransform::SetOrientation(inputMmatrix,10,20,30);
  mafTransform::SetPosition(inputMmatrix,100,-20,60);

  mafEvent *e = new mafEvent(this,ID_TRANSFORM);
  e->SetArg(mafInteractorGenericMouse::MOUSE_MOVE);
  e->SetMatrix(&inputMmatrix);

  mafOpMAFTransform *op = new mafOpMAFTransform();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();
  op->OnEventGuiTransform(e);
  op->OpDo();

  mafVMESurface *output = mafVMESurface::SafeDownCast(op->GetInput());
  output->GetOutput()->Update();
  output->Update();

  mafMatrix *matrixOutput = output->GetOutput()->GetAbsMatrix();

  for(int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    {
      CPPUNIT_ASSERT( matrixOutput->GetElement(i,j) + DELTA > inputMmatrix.GetElement(i,j) && matrixOutput->GetElement(i,j) - DELTA < inputMmatrix.GetElement(i,j) );
    }
  }

  delete e;

  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpMAFTransformTest::TestOnEventGuiSaveRestorePose()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMESurface> input;
  vtkMAFSmartPointer<vtkCubeSource> cube;
  cube->Update();

  input->SetData(cube->GetOutput(),0.0);
  input->GetOutput()->Update();
  input->Update();

  mafMatrix inputMmatrix;
  mafTransform::SetOrientation(inputMmatrix,10,20,30);
  mafTransform::SetPosition(inputMmatrix,100,-20,60);

  mafEvent *e = new mafEvent(this,ID_TRANSFORM);
  e->SetArg(mafInteractorGenericMouse::MOUSE_MOVE);
  e->SetMatrix(&inputMmatrix);

  mafOpMAFTransform *op = new mafOpMAFTransform();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();
  op->OnEventGuiTransform(e);
  op->OpDo();

  mafVMESurface *output = mafVMESurface::SafeDownCast(op->GetInput());
  output->GetOutput()->Update();
  output->Update();

  mafMatrix *matrixOutput = output->GetOutput()->GetAbsMatrix();

  for(int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    {
      CPPUNIT_ASSERT( matrixOutput->GetElement(i,j) + DELTA > inputMmatrix.GetElement(i,j) && matrixOutput->GetElement(i,j) - DELTA < inputMmatrix.GetElement(i,j) );
    }
  }

  delete e;

  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpMAFTransformTest::TestReset()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMESurface> input;
  vtkMAFSmartPointer<vtkCubeSource> cube;
  cube->Update();

  input->SetData(cube->GetOutput(),0.0);
  input->GetOutput()->Update();
  input->Update();

  mafMatrix inputMmatrix;
  mafTransform::SetOrientation(inputMmatrix,10,20,30);
  mafTransform::SetPosition(inputMmatrix,100,-20,60);

  mafEvent *e = new mafEvent(this,ID_TRANSFORM);
  e->SetArg(mafInteractorGenericMouse::MOUSE_MOVE);
  e->SetMatrix(&inputMmatrix);

  mafOpMAFTransform *op = new mafOpMAFTransform();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();
  op->OnEventGuiTransform(e);
  op->Reset();
  op->OpDo();

  mafVMESurface *output = mafVMESurface::SafeDownCast(op->GetInput());
  output->GetOutput()->Update();
  output->Update();

  mafMatrix *matrixOutput = output->GetOutput()->GetAbsMatrix();

  mafSmartPointer<mafMatrix> identity;
  identity->Identity();

  for(int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    {
      CPPUNIT_ASSERT( matrixOutput->GetElement(i,j) == identity->GetElement(i,j) );
    }
  }

  delete e;

  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpMAFTransformTest::TestOpUndo()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMESurface> input;
  vtkMAFSmartPointer<vtkCubeSource> cube;
  cube->Update();

  input->SetData(cube->GetOutput(),0.0);
  input->GetOutput()->Update();
  input->Update();

  mafMatrix inputMmatrix;
  mafTransform::SetOrientation(inputMmatrix,10,20,30);
  mafTransform::SetPosition(inputMmatrix,100,-20,60);

  mafEvent *e = new mafEvent(this,ID_TRANSFORM);
  e->SetArg(mafInteractorGenericMouse::MOUSE_MOVE);
  e->SetMatrix(&inputMmatrix);

  mafOpMAFTransform *op = new mafOpMAFTransform();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();
  op->OnEventGuiTransform(e);
  op->OpDo();
  op->OpUndo();

  mafVMESurface *output = mafVMESurface::SafeDownCast(op->GetInput());
  output->GetOutput()->Update();
  output->Update();

  mafMatrix *matrixOutput = output->GetOutput()->GetAbsMatrix();

  mafSmartPointer<mafMatrix> identity;
  identity->Identity();

  for(int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    {
      CPPUNIT_ASSERT( matrixOutput->GetElement(i,j) == identity->GetElement(i,j) );
    }
  }

  delete e;

  mafDEL(op);
}
