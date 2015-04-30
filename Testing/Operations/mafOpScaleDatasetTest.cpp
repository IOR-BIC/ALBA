/*=========================================================================

 Program: MAF2
 Module: mafOpScaleDatasetTest
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
#include "mafOpScaleDatasetTest.h"
#include "mafOpScaleDataset.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"
#include "mafVMERoot.h"
#include "mafMatrix.h"
#include "mafTransform.h"

#include "vtkMAFSmartPointer.h"
#include "vtkCubeSource.h"
#include "vtkPolyData.h"
#include "vtkStructuredPoints.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"

//-----------------------------------------------------------
void mafOpScaleDatasetTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpScaleDataset *op = new mafOpScaleDataset();
  mafDEL(op);
}
//-----------------------------------------------------------
void mafOpScaleDatasetTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  mafOpScaleDataset extrusion; 
}
//-----------------------------------------------------------
void mafOpScaleDatasetTest::TestOpDoVolume() 
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkStructuredPoints> sp;
  sp->SetOrigin(0.0,0.0,0.0);
  sp->SetSpacing(1.0,1.0,1.0);
  sp->SetDimensions(5,10,6);
  sp->Update();

  vtkMAFSmartPointer<vtkDoubleArray> scalars;
  scalars->SetNumberOfTuples(300);
  for (int i=0;i<300;i++)
  {
    scalars->SetTuple1(i,0.0);
  }
  sp->GetPointData()->SetScalars(scalars);
  sp->Update();

  double startPoint[3];
  sp->GetPoint(0,startPoint);

  mafSmartPointer<mafVMEVolumeGray> volumeInput;

  volumeInput->SetData(sp,0.0);
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  mafOpScaleDataset *op = new mafOpScaleDataset();
  op->TestModeOn();
  op->SetInput(volumeInput);

  mafMatrix m;
  double scaleFactorX,scaleFactorY,scaleFactorZ;
  scaleFactorX = 2.0;
  scaleFactorY = 3.0;
  scaleFactorZ = 1.0;
  mafTransform::Scale(m,scaleFactorX,scaleFactorY,scaleFactorZ,0);
  op->OpRun();
  op->SetNewAbsMatrix(m);
  op->OpDo();

  mafVMEVolumeGray *volumeOutput = mafVMEVolumeGray::SafeDownCast(op->GetInput());
  volumeOutput->GetOutput()->Update();
  volumeOutput->Update();

  vtkStructuredPoints *spOutput = vtkStructuredPoints::SafeDownCast(volumeOutput->GetOutput()->GetVTKData());
  spOutput->Update();

  double endPoint[3];
  spOutput->GetPoint(0,endPoint);

  CPPUNIT_ASSERT( endPoint[0] == startPoint[0]*scaleFactorX );
  CPPUNIT_ASSERT( endPoint[1] == startPoint[1]*scaleFactorY );
  CPPUNIT_ASSERT( endPoint[2] == startPoint[2]*scaleFactorZ );  

  mafDEL(op);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void mafOpScaleDatasetTest::TestOpDoSurface() 
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkCubeSource> cube;
  cube->Update();

  double startPoint[3];
  cube->GetOutput()->GetPoint(0,startPoint);

  mafSmartPointer<mafVMESurface> surfaceInput;

  surfaceInput->SetData(cube->GetOutput(),0.0);
  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  mafOpScaleDataset *op = new mafOpScaleDataset();
  op->TestModeOn();
  op->SetInput(surfaceInput);

  mafMatrix m;
  double scaleFactorX,scaleFactorY,scaleFactorZ;
  scaleFactorX = 2.0;
  scaleFactorY = 3.0;
  scaleFactorZ = 1.0;
  mafTransform::Scale(m,scaleFactorX,scaleFactorY,scaleFactorZ,0);
  op->OpRun();
  op->SetNewAbsMatrix(m);
  op->OpDo();

  mafVMESurface *surfaceOutput = mafVMESurface::SafeDownCast(op->GetInput());
  surfaceOutput->GetOutput()->Update();
  surfaceOutput->Update();

  vtkPolyData *cubeOutput = vtkPolyData::SafeDownCast(surfaceOutput->GetOutput()->GetVTKData());
  cubeOutput->Update();
  
  double endPoint[3];
  cubeOutput->GetPoint(0,endPoint);

  CPPUNIT_ASSERT( endPoint[0] == startPoint[0]*scaleFactorX );
  CPPUNIT_ASSERT( endPoint[1] == startPoint[1]*scaleFactorY );
  CPPUNIT_ASSERT( endPoint[2] == startPoint[2]*scaleFactorZ );  

  mafDEL(op);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void mafOpScaleDatasetTest::TestReset() 
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkCubeSource> cube;
  cube->Update();

  double startPoint[3];
  cube->GetOutput()->GetPoint(0,startPoint);

  mafSmartPointer<mafVMESurface> surfaceInput;

  surfaceInput->SetData(cube->GetOutput(),0.0);
  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  mafOpScaleDataset *op = new mafOpScaleDataset();
  op->TestModeOn();
  op->SetInput(surfaceInput);

  mafMatrix m;
  double scaleFactorX,scaleFactorY,scaleFactorZ;
  scaleFactorX = 2.0;
  scaleFactorY = 3.0;
  scaleFactorZ = 1.0;
  mafTransform::Scale(m,scaleFactorX,scaleFactorY,scaleFactorZ,0);
  op->OpRun();
  op->SetNewAbsMatrix(m);
  op->Reset();

  mafVMESurface *surfaceOutput = mafVMESurface::SafeDownCast(op->GetInput());
  surfaceOutput->GetOutput()->Update();
  surfaceOutput->Update();

  vtkPolyData *cubeOutput = vtkPolyData::SafeDownCast(surfaceOutput->GetOutput()->GetVTKData());
  cubeOutput->Update();

  double endPoint[3];
  cubeOutput->GetPoint(0,endPoint);

  CPPUNIT_ASSERT( endPoint[0] == startPoint[0] );
  CPPUNIT_ASSERT( endPoint[1] == startPoint[1] );
  CPPUNIT_ASSERT( endPoint[2] == startPoint[2] );  

  mafDEL(op);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void mafOpScaleDatasetTest::TestAccept() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMESurface> surface;
  mafSmartPointer<mafVMEVolumeGray> volume;
  mafSmartPointer<mafVMERoot> root;
  mafOpScaleDataset *op = new mafOpScaleDataset();
  op->TestModeOn();

  CPPUNIT_ASSERT( op->Accept(surface) );
  CPPUNIT_ASSERT( op->Accept(volume) );
  CPPUNIT_ASSERT( !op->Accept(root) );

  mafDEL(op);

  delete wxLog::SetActiveTarget(NULL);
}
