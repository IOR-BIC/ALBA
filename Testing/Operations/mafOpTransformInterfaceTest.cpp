/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpTransformInterfaceTest.cpp,v $
Language:  C++
Date:      $Date: 2011-01-24 13:28:10 $
Version:   $Revision: 1.1.2.1 $
Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004 
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
#include "mafOpTransformInterfaceTest.h"

#include "mafOpTransformInterface.h"
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
void mafOpTransformInterfaceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpTransformInterfaceTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpTransformInterfaceTest::tearDown()
//----------------------------------------------------------------------------
{
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafOpTransformInterfaceTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafOpTransformInterface op;
}
//----------------------------------------------------------------------------
void mafOpTransformInterfaceTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafOpTransformInterface *op = new mafOpTransformInterface();
  mafDEL(op);
}

//----------------------------------------------------------------------------
void mafOpTransformInterfaceTest::TestPostMultiplyEventMatrix()
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

  mafOpTransformInterface *op = new mafOpTransformInterface();
  op->TestModeOn();
  op->SetInput(input);
  op->PostMultiplyEventMatrix(e);

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
void mafOpTransformInterfaceTest::TestOpDo()
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

  mafOpTransformInterface *op = new mafOpTransformInterface();
  op->TestModeOn();
  op->SetInput(input);
  op->PostMultiplyEventMatrix(e);
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


