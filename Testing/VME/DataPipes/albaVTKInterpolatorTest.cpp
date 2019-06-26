/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVTKInterpolatorTest
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
#include "albaVTKInterpolatorTest.h"

#include "albaVTKInterpolator.h"
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

/** a vme test  class used to test event reception. */
class albaVMETestClass: public albaVMESurface
{
public:
	albaVMETestClass(const char *name=NULL):Name(name) {}
	albaTypeMacro(albaVMETestClass,albaVMESurface);

	virtual void OnEvent(albaEventBase *event) {Name = "CATCHED";};

	albaString     Name;
};

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMETestClass)
//-------------------------------------------------------------------------


//----------------------------------------------------------------------------
void albaVTKInterpolatorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVTKInterpolatorTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void albaVTKInterpolatorTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVTKInterpolatorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVTKInterpolator> di;

	albaVTKInterpolator *di2;
	albaNEW(di2);
	albaDEL(di2);
}
//----------------------------------------------------------------------------
void albaVTKInterpolatorTest::TestAccept()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMETestClass> surfaceTest;
  vtkALBASmartPointer<vtkSphereSource> sphere;
  
  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  albaSmartPointer<albaVTKInterpolator> di;
  result = di->Accept(surfaceTest);
  TEST_RESULT;

  result = !(di->Accept(NULL));
  TEST_RESULT;
}

//----------------------------------------------------------------------------
void albaVTKInterpolatorTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  //catch events inside vme : VME_OUTPUT_DATA_PREUPDATE, VME_OUTPUT_DATA_UPDATE

  albaSmartPointer<albaVMETestClass> surfaceTest1;

  albaSmartPointer<albaVTKInterpolator> di;
  di->SetVME(surfaceTest1);
  di->OnEvent(&albaEvent(di->GetVTKDataPipe(),VME_OUTPUT_DATA_PREUPDATE)); //sender must be datapipe
  
  result = surfaceTest1->Name.Equals("CATCHED");
  TEST_RESULT;

  albaSmartPointer<albaVMETestClass> surfaceTest2;
  di->SetVME(surfaceTest2);
  di->OnEvent(&albaEvent(di->GetVTKDataPipe(),VME_OUTPUT_DATA_UPDATE)); //sender must be datapipe

  result = surfaceTest2->Name.Equals("CATCHED");
  TEST_RESULT;

}

//----------------------------------------------------------------------------
void albaVTKInterpolatorTest::TestGetVTKData()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMETestClass> surfaceTest;
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->SetThetaResolution(100);
  sphere->SetPhiResolution(100);
  sphere->SetRadius(5.0);
  sphere->Update();

  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  albaSmartPointer<albaVTKInterpolator> di;
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
void albaVTKInterpolatorTest::TestGetCurrentItem()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMETestClass> surfaceTest;
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  int numberOfSpherePoints = sphere->GetOutput()->GetNumberOfPoints();

  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  albaSmartPointer<albaVTKInterpolator> di;
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
void albaVTKInterpolatorTest::TestUpdate()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMETestClass> surfaceTest;
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->SetThetaResolution(100);
  sphere->SetPhiResolution(100);
  sphere->SetRadius(5.0);
  sphere->Update();

  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  

  albaSmartPointer<albaVTKInterpolator> di;
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
void albaVTKInterpolatorTest::GetVTKDataPipe()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVTKInterpolator> di;
  result = NULL != di->GetVTKDataPipe();
  TEST_RESULT;
}