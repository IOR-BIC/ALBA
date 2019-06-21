/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeInterpolatorTest
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
#include "albaDataPipeInterpolatorTest.h"

#include "albaDataPipeInterpolator.h"
#include "albaDataPipe.h"
#include "vtkALBASmartPointer.h"
#include "albaSmartPointer.h"
#include "albaVMESurface.h"
#include "albaOBB.h"
#include "albaVMEItem.h"
#include "albaVMEItemVTK.h"

#include "vtkSphereSource.h"
#include "vtkPolyData.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);
#define EPSILON 0.1
/** a vme test  class used to test event reception. */
class albaDataPipeInterpolatorTestClass: public albaDataPipeInterpolator
{
public:
  albaTypeMacro(albaDataPipeInterpolatorTestClass,albaDataPipeInterpolator);

protected:
  albaDataPipeInterpolatorTestClass(){};
  virtual ~albaDataPipeInterpolatorTestClass(){};

  virtual void PreExecute(){};
  virtual void Execute(){};
};

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaDataPipeInterpolatorTestClass)
//------------------------------------------------------------------------------


//----------------------------------------------------------------------------
void albaDataPipeInterpolatorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaDataPipeInterpolatorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaDataPipeInterpolatorTestClass> di;

	albaDataPipeInterpolatorTestClass * di2;
	albaNEW(di2);
	albaDEL(di2);
}
//----------------------------------------------------------------------------
void albaDataPipeInterpolatorTest::TestAccept()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMESurfaceTestClass> surfaceTest;
  vtkALBASmartPointer<vtkSphereSource> sphere;
  
  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  albaSmartPointer<albaDataPipeInterpolatorTestClass> di;
  result = di->Accept(surfaceTest);
  TEST_RESULT;

  result = !(di->Accept(NULL));
  TEST_RESULT;
}

//----------------------------------------------------------------------------
void albaDataPipeInterpolatorTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  //catch events inside vme : VME_OUTPUT_DATA_PREUPDATE, VME_OUTPUT_DATA_UPDATE

  albaSmartPointer<albaVMESurfaceTestClass> surfaceTest1;

  albaSmartPointer<albaDataPipeInterpolatorTestClass> di;
  di->SetVME(surfaceTest1);
  di->OnEvent(&albaEvent(this,VME_OUTPUT_DATA_PREUPDATE));
  
  result = surfaceTest1->Name.Equals("CATCHED");
  TEST_RESULT;

  albaSmartPointer<albaVMESurfaceTestClass> surfaceTest2;
  di->SetVME(surfaceTest2);
  di->OnEvent(&albaEvent(this,VME_OUTPUT_DATA_UPDATE));

  result = surfaceTest2->Name.Equals("CATCHED");
  TEST_RESULT;

}

//----------------------------------------------------------------------------
void albaDataPipeInterpolatorTest::TestUpdateBounds()
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

  albaSmartPointer<albaDataPipeInterpolatorTestClass> di;
  di->SetVME(surfaceTest);
  di->UpdateBounds();

  albaOBB *bounds;
  bounds = di->GetBounds();

  double controlledBounds[6]; //this is bounds of a sphere with radius 5.0
  controlledBounds[0] = -5.;
  controlledBounds[1] = 5.;

  controlledBounds[2] = -5.;
  controlledBounds[3] = 5.;

  controlledBounds[4] = -5.;
  controlledBounds[5] = 5.;

  result = (fabs(controlledBounds[0] - bounds->m_Bounds[0]) < EPSILON) && \
           (fabs(controlledBounds[1] - bounds->m_Bounds[1]) < EPSILON) && \
           (fabs(controlledBounds[2] - bounds->m_Bounds[2]) < EPSILON) && \
           (fabs(controlledBounds[3] - bounds->m_Bounds[3]) < EPSILON) && \
           (fabs(controlledBounds[4] - bounds->m_Bounds[4]) < EPSILON) && \
           (fabs(controlledBounds[5] - bounds->m_Bounds[5]) < EPSILON);

  TEST_RESULT;
}

//----------------------------------------------------------------------------
void albaDataPipeInterpolatorTest::TestGetCurrentItem()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMESurfaceTestClass> surfaceTest;
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  int numberOfSpherePoints = sphere->GetOutput()->GetNumberOfPoints();

  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  albaSmartPointer<albaDataPipeInterpolatorTestClass> di;
  di->SetVME(surfaceTest);
  di->UpdateBounds();

  albaVMEItem *currentItem;
  currentItem = di->GetCurrentItem();
  int numberOfPoints = albaVMEItemVTK::SafeDownCast(currentItem)->GetData()->GetNumberOfPoints();

  result = currentItem != NULL && (numberOfSpherePoints == numberOfPoints);
  //printf("Points %d - %d", numberOfSpherePoints, numberOfPoints);

  TEST_RESULT;

}
//----------------------------------------------------------------------------
void albaDataPipeInterpolatorTest::TestSetTimeStamp()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaDataPipeInterpolatorTestClass> di;

  albaTimeStamp time = 30.;
  di->SetTimeStamp(time);
  result = di->GetTimeStamp() == time;

  TEST_RESULT;
}

//----------------------------------------------------------------------------
void albaDataPipeInterpolatorTest::GetMTime()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaDataPipeInterpolatorTestClass> di;

  long time1, time2;
  time1 = di->GetMTime();
  di->Modified();
  di->Update();
  time2 = di->GetMTime();

  result = time2 > time1;
  TEST_RESULT;

}