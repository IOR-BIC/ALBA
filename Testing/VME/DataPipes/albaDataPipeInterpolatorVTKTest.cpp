/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeInterpolatorVTKTest
 Authors: Daniele Giunchi
 
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
#include "albaDataPipesTests.h"
#include "albaDataPipeInterpolatorVTKTest.h"

#include "albaDataPipeInterpolatorVTK.h"
#include "albaDataPipe.h"
#include "vtkALBASmartPointer.h"
#include "albaSmartPointer.h"
#include "albaVMESurface.h"
#include "albaOBB.h"
#include "albaVMEItem.h"
#include "albaVMEItemVTK.h"
#include "vtkALBADataPipe.h"

#include "vtkSphereSource.h"
#include "vtkPolyData.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);




//----------------------------------------------------------------------------
void albaDataPipeInterpolatorVTKTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaDataPipeInterpolatorVTKTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaDataPipeInterpolatorVTK> di;

	albaDataPipeInterpolatorVTK *di2;
	albaNEW(di2);
	albaDEL(di2);
}
//----------------------------------------------------------------------------
void albaDataPipeInterpolatorVTKTest::TestAccept()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMESurfaceTestClass> surfaceTest;
  vtkALBASmartPointer<vtkSphereSource> sphere;
  
  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  albaSmartPointer<albaDataPipeInterpolatorVTK> di;
  result = di->Accept(surfaceTest);
  TEST_RESULT;

  result = !(di->Accept(NULL));
  TEST_RESULT;
}

//----------------------------------------------------------------------------
void albaDataPipeInterpolatorVTKTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  //catch events inside vme : VME_OUTPUT_DATA_PREUPDATE, VME_OUTPUT_DATA_UPDATE

  albaSmartPointer<albaVMESurfaceTestClass> surfaceTest1;

  albaSmartPointer<albaDataPipeInterpolatorVTK> di;
  di->SetVME(surfaceTest1);
  di->OnEvent(&albaEvent(di->GetVTKDataPipe(),VME_OUTPUT_DATA_PREUPDATE)); //sender must be datapipe
  
  result = surfaceTest1->Name.Equals("CATCHED");
  TEST_RESULT;

  albaSmartPointer<albaVMESurfaceTestClass> surfaceTest2;
  di->SetVME(surfaceTest2);
  di->OnEvent(&albaEvent(di->GetVTKDataPipe(),VME_OUTPUT_DATA_UPDATE)); //sender must be datapipe

  result = surfaceTest2->Name.Equals("CATCHED");
  TEST_RESULT;

}

//----------------------------------------------------------------------------
void albaDataPipeInterpolatorVTKTest::TestGetVTKData()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMESurfaceTestClass> surfaceTest;
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->SetThetaResolution(100);
  sphere->SetPhiResolution(100);
  sphere->SetRadius(5.0);
  sphere->Update();

  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  albaSmartPointer<albaDataPipeInterpolatorVTK> di;
  di->SetVME(surfaceTest);
  di->Update();

  vtkDataSet *dataset;
  dataset = di->GetVTKData();

  double numberOfPoints = dataset->GetNumberOfPoints();

  result =  numberOfPoints == sphere->GetOutput()->GetNumberOfPoints();

  TEST_RESULT;
}

//----------------------------------------------------------------------------
void albaDataPipeInterpolatorVTKTest::TestGetCurrentItem()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMESurfaceTestClass> surfaceTest;
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  int numberOfSpherePoints = sphere->GetOutput()->GetNumberOfPoints();

  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  albaSmartPointer<albaDataPipeInterpolatorVTK> di;
  di->SetVME(surfaceTest);
  di->UpdateBounds();

  albaVMEItem *currentItem;
  currentItem = di->GetCurrentItem();
  int numberOfPoints = albaVMEItemVTK::SafeDownCast(currentItem)->GetData()->GetNumberOfPoints();

  result = currentItem != NULL && currentItem->IsA("albaVMEItemVTK") && (numberOfSpherePoints == numberOfPoints);
  //printf("Points %d - %d", numberOfSpherePoints, numberOfPoints);

  TEST_RESULT;

}
//----------------------------------------------------------------------------
void albaDataPipeInterpolatorVTKTest::TestUpdate()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMESurfaceTestClass> surfaceTest;
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->SetThetaResolution(100);
  sphere->SetPhiResolution(100);
  sphere->SetRadius(5.0);
  sphere->Update();

  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  

  albaSmartPointer<albaDataPipeInterpolatorVTK> di;
  di->SetVME(surfaceTest);
  
  vtkDataSet *dataset;
  dataset = di->GetVTKData();

  di->Update();

  result = di->GetVTKDataPipe()->GetOutput()->GetNumberOfPoints() == sphere->GetOutput()->GetNumberOfPoints() &&
           di->GetVTKDataPipe()->GetOutput()->GetNumberOfPoints() == dataset->GetNumberOfPoints();

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaDataPipeInterpolatorVTKTest::GetVTKDataPipe()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaDataPipeInterpolatorVTK> di;
  result = NULL != di->GetVTKDataPipe();
  TEST_RESULT;
}