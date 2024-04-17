/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpScaleDatasetTest
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
#include "albaOpScaleDatasetTest.h"
#include "albaOpScaleDataset.h"
#include "albaVMESurface.h"
#include "albaVMEVolumeGray.h"
#include "albaVMERoot.h"
#include "albaMatrix.h"
#include "albaTransform.h"

#include "vtkALBASmartPointer.h"
#include "vtkCubeSource.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"

//-----------------------------------------------------------
void albaOpScaleDatasetTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpScaleDataset *op = new albaOpScaleDataset();
  albaDEL(op);
}
//-----------------------------------------------------------
void albaOpScaleDatasetTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  albaOpScaleDataset extrusion; 
}
//-----------------------------------------------------------
void albaOpScaleDatasetTest::TestOpDoVolume() 
//-----------------------------------------------------------
{
  vtkALBASmartPointer<vtkImageData> sp;
  sp->SetOrigin(0.0,0.0,0.0);
  sp->SetSpacing(1.0,1.0,1.0);
  sp->SetDimensions(5,10,6);

  vtkALBASmartPointer<vtkDoubleArray> scalars;
  scalars->SetNumberOfTuples(300);
  for (int i=0;i<300;i++)
  {
    scalars->SetTuple1(i,0.0);
  }
  sp->GetPointData()->SetScalars(scalars);

  double startPoint[3];
  sp->GetPoint(0,startPoint);

  albaSmartPointer<albaVMEVolumeGray> volumeInput;

  volumeInput->SetData(sp,0.0);
  volumeInput->GetOutput()->Update();
  volumeInput->Update();

  albaOpScaleDataset *op = new albaOpScaleDataset();
  op->TestModeOn();
  op->SetInput(volumeInput);

  albaMatrix m;
  double scaleFactorX,scaleFactorY,scaleFactorZ;
  scaleFactorX = 2.0;
  scaleFactorY = 3.0;
  scaleFactorZ = 1.0;
  albaTransform::Scale(m,scaleFactorX,scaleFactorY,scaleFactorZ,0);
  op->OpRun();
  op->SetNewAbsMatrix(m);
  op->OpDo();

  albaVMEVolumeGray *volumeOutput = albaVMEVolumeGray::SafeDownCast(op->GetInput());
  volumeOutput->GetOutput()->Update();
  volumeOutput->Update();

  vtkImageData *spOutput = vtkImageData::SafeDownCast(volumeOutput->GetOutput()->GetVTKData());

  double endPoint[3];
  spOutput->GetPoint(0,endPoint);

  CPPUNIT_ASSERT( endPoint[0] == startPoint[0]*scaleFactorX );
  CPPUNIT_ASSERT( endPoint[1] == startPoint[1]*scaleFactorY );
  CPPUNIT_ASSERT( endPoint[2] == startPoint[2]*scaleFactorZ );  

  albaDEL(op);
}
//-----------------------------------------------------------
void albaOpScaleDatasetTest::TestOpDoSurface() 
//-----------------------------------------------------------
{
  vtkALBASmartPointer<vtkCubeSource> cube;
  cube->Update();

  double startPoint[3];
  cube->GetOutput()->GetPoint(0,startPoint);

  albaSmartPointer<albaVMESurface> surfaceInput;

  surfaceInput->SetData(cube->GetOutput(),0.0);
  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  albaOpScaleDataset *op = new albaOpScaleDataset();
  op->TestModeOn();
  op->SetInput(surfaceInput);

  albaMatrix m;
  double scaleFactorX,scaleFactorY,scaleFactorZ;
  scaleFactorX = 2.0;
  scaleFactorY = 3.0;
  scaleFactorZ = 1.0;
  albaTransform::Scale(m,scaleFactorX,scaleFactorY,scaleFactorZ,0);
  op->OpRun();
  op->SetNewAbsMatrix(m);
  op->OpDo();

  albaVMESurface *surfaceOutput = albaVMESurface::SafeDownCast(op->GetInput());
  surfaceOutput->GetOutput()->Update();
  surfaceOutput->Update();

  vtkPolyData *cubeOutput = vtkPolyData::SafeDownCast(surfaceOutput->GetOutput()->GetVTKData());
  
  double endPoint[3];
  cubeOutput->GetPoint(0,endPoint);

  CPPUNIT_ASSERT( endPoint[0] == startPoint[0]*scaleFactorX );
  CPPUNIT_ASSERT( endPoint[1] == startPoint[1]*scaleFactorY );
  CPPUNIT_ASSERT( endPoint[2] == startPoint[2]*scaleFactorZ );  

  albaDEL(op);
}
//-----------------------------------------------------------
void albaOpScaleDatasetTest::TestReset() 
//-----------------------------------------------------------
{
  vtkALBASmartPointer<vtkCubeSource> cube;
  cube->Update();

  double startPoint[3];
  cube->GetOutput()->GetPoint(0,startPoint);

  albaSmartPointer<albaVMESurface> surfaceInput;

  surfaceInput->SetData(cube->GetOutput(),0.0);
  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  albaOpScaleDataset *op = new albaOpScaleDataset();
  op->TestModeOn();
  op->SetInput(surfaceInput);

  albaMatrix m;
  double scaleFactorX,scaleFactorY,scaleFactorZ;
  scaleFactorX = 2.0;
  scaleFactorY = 3.0;
  scaleFactorZ = 1.0;
  albaTransform::Scale(m,scaleFactorX,scaleFactorY,scaleFactorZ,0);
  op->OpRun();
  op->SetNewAbsMatrix(m);
  op->Reset();

  albaVMESurface *surfaceOutput = albaVMESurface::SafeDownCast(op->GetInput());
  surfaceOutput->GetOutput()->Update();
  surfaceOutput->Update();

  vtkPolyData *cubeOutput = vtkPolyData::SafeDownCast(surfaceOutput->GetOutput()->GetVTKData());

  double endPoint[3];
  cubeOutput->GetPoint(0,endPoint);

  CPPUNIT_ASSERT( endPoint[0] == startPoint[0] );
  CPPUNIT_ASSERT( endPoint[1] == startPoint[1] );
  CPPUNIT_ASSERT( endPoint[2] == startPoint[2] );  

  albaDEL(op);
}
//-----------------------------------------------------------
void albaOpScaleDatasetTest::TestAccept() 
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMESurface> surface;
  albaSmartPointer<albaVMEVolumeGray> volume;
  albaSmartPointer<albaVMERoot> root;
  albaOpScaleDataset *op = new albaOpScaleDataset();
  op->TestModeOn();

  CPPUNIT_ASSERT( op->Accept(surface) );
  CPPUNIT_ASSERT( op->Accept(volume) );
  CPPUNIT_ASSERT( !op->Accept(root) );

  albaDEL(op);
}
