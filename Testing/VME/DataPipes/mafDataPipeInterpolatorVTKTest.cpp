/*=========================================================================

 Program: MAF2
 Module: mafDataPipeInterpolatorVTKTest
 Authors: Daniele Giunchi
 
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
#include "mafDataPipesTests.h"
#include "mafDataPipeInterpolatorVTKTest.h"

#include "mafDataPipeInterpolatorVTK.h"
#include "mafDataPipe.h"
#include "vtkMAFSmartPointer.h"
#include "mafSmartPointer.h"
#include "mafVMESurface.h"
#include "mafOBB.h"
#include "mafVMEItem.h"
#include "mafVMEItemVTK.h"
#include "vtkMAFDataPipe.h"

#include "vtkSphereSource.h"
#include "vtkPolyData.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);




//----------------------------------------------------------------------------
void mafDataPipeInterpolatorVTKTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafDataPipeInterpolatorVTKTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafDataPipeInterpolatorVTK> di;

	mafDataPipeInterpolatorVTK *di2;
	mafNEW(di2);
	mafDEL(di2);
}
//----------------------------------------------------------------------------
void mafDataPipeInterpolatorVTKTest::TestAccept()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMESurfaceTestClass> surfaceTest;
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  
  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  mafSmartPointer<mafDataPipeInterpolatorVTK> di;
  result = di->Accept(surfaceTest);
  TEST_RESULT;

  result = !(di->Accept(NULL));
  TEST_RESULT;
}

//----------------------------------------------------------------------------
void mafDataPipeInterpolatorVTKTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  //catch events inside vme : VME_OUTPUT_DATA_PREUPDATE, VME_OUTPUT_DATA_UPDATE

  mafSmartPointer<mafVMESurfaceTestClass> surfaceTest1;

  mafSmartPointer<mafDataPipeInterpolatorVTK> di;
  di->SetVME(surfaceTest1);
  di->OnEvent(&mafEvent(di->GetVTKDataPipe(),VME_OUTPUT_DATA_PREUPDATE)); //sender must be datapipe
  
  result = surfaceTest1->Name.Equals("CATCHED");
  TEST_RESULT;

  mafSmartPointer<mafVMESurfaceTestClass> surfaceTest2;
  di->SetVME(surfaceTest2);
  di->OnEvent(&mafEvent(di->GetVTKDataPipe(),VME_OUTPUT_DATA_UPDATE)); //sender must be datapipe

  result = surfaceTest2->Name.Equals("CATCHED");
  TEST_RESULT;

}

//----------------------------------------------------------------------------
void mafDataPipeInterpolatorVTKTest::TestGetVTKData()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMESurfaceTestClass> surfaceTest;
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->SetThetaResolution(100);
  sphere->SetPhiResolution(100);
  sphere->SetRadius(5.0);
  sphere->Update();

  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  mafSmartPointer<mafDataPipeInterpolatorVTK> di;
  di->SetVME(surfaceTest);
  di->Update();

  vtkDataSet *dataset;
  dataset = di->GetVTKData();
  dataset->Update();

  double numberOfPoints = dataset->GetNumberOfPoints();

  result =  numberOfPoints == sphere->GetOutput()->GetNumberOfPoints();

  TEST_RESULT;
}

//----------------------------------------------------------------------------
void mafDataPipeInterpolatorVTKTest::TestGetCurrentItem()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMESurfaceTestClass> surfaceTest;
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  int numberOfSpherePoints = sphere->GetOutput()->GetNumberOfPoints();

  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  mafSmartPointer<mafDataPipeInterpolatorVTK> di;
  di->SetVME(surfaceTest);
  di->UpdateBounds();

  mafVMEItem *currentItem;
  currentItem = di->GetCurrentItem();
  int numberOfPoints = mafVMEItemVTK::SafeDownCast(currentItem)->GetData()->GetNumberOfPoints();

  result = currentItem != NULL && currentItem->IsA("mafVMEItemVTK") && (numberOfSpherePoints == numberOfPoints);
  //printf("Points %d - %d", numberOfSpherePoints, numberOfPoints);

  TEST_RESULT;

}
//----------------------------------------------------------------------------
void mafDataPipeInterpolatorVTKTest::TestUpdate()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMESurfaceTestClass> surfaceTest;
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->SetThetaResolution(100);
  sphere->SetPhiResolution(100);
  sphere->SetRadius(5.0);
  sphere->Update();

  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  

  mafSmartPointer<mafDataPipeInterpolatorVTK> di;
  di->SetVME(surfaceTest);
  
  vtkDataSet *dataset;
  dataset = di->GetVTKData();
  dataset->Update();

  di->Update();

  result = di->GetVTKDataPipe()->GetOutput()->GetNumberOfPoints() == sphere->GetOutput()->GetNumberOfPoints() &&
           di->GetVTKDataPipe()->GetOutput()->GetNumberOfPoints() == dataset->GetNumberOfPoints();

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafDataPipeInterpolatorVTKTest::GetVTKDataPipe()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafDataPipeInterpolatorVTK> di;
  result = NULL != di->GetVTKDataPipe();
  TEST_RESULT;
}