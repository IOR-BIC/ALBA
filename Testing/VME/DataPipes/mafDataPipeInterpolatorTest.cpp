/*=========================================================================

 Program: MAF2
 Module: mafDataPipeInterpolatorTest
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
#include "mafDataPipeInterpolatorTest.h"

#include "mafDataPipeInterpolator.h"
#include "mafDataPipe.h"
#include "vtkMAFSmartPointer.h"
#include "mafSmartPointer.h"
#include "mafVMESurface.h"
#include "mafOBB.h"
#include "mafVMEItem.h"
#include "mafVMEItemVTK.h"

#include "vtkSphereSource.h"
#include "vtkPolyData.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);
#define EPSILON 0.1
/** a vme test  class used to test event reception. */
class mafDataPipeInterpolatorTestClass: public mafDataPipeInterpolator
{
public:
  mafTypeMacro(mafDataPipeInterpolatorTestClass,mafDataPipeInterpolator);

protected:
  mafDataPipeInterpolatorTestClass(){};
  virtual ~mafDataPipeInterpolatorTestClass(){};

  virtual void PreExecute(){};
  virtual void Execute(){};
};

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafDataPipeInterpolatorTestClass)
//------------------------------------------------------------------------------


//----------------------------------------------------------------------------
void mafDataPipeInterpolatorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafDataPipeInterpolatorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafDataPipeInterpolatorTestClass> di;

	mafDataPipeInterpolatorTestClass * di2;
	mafNEW(di2);
	mafDEL(di2);
}
//----------------------------------------------------------------------------
void mafDataPipeInterpolatorTest::TestAccept()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMESurfaceTestClass> surfaceTest;
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  
  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  mafSmartPointer<mafDataPipeInterpolatorTestClass> di;
  result = di->Accept(surfaceTest);
  TEST_RESULT;

  result = !(di->Accept(NULL));
  TEST_RESULT;
}

//----------------------------------------------------------------------------
void mafDataPipeInterpolatorTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  //catch events inside vme : VME_OUTPUT_DATA_PREUPDATE, VME_OUTPUT_DATA_UPDATE

  mafSmartPointer<mafVMESurfaceTestClass> surfaceTest1;

  mafSmartPointer<mafDataPipeInterpolatorTestClass> di;
  di->SetVME(surfaceTest1);
  di->OnEvent(&mafEvent(this,VME_OUTPUT_DATA_PREUPDATE));
  
  result = surfaceTest1->Name.Equals("CATCHED");
  TEST_RESULT;

  mafSmartPointer<mafVMESurfaceTestClass> surfaceTest2;
  di->SetVME(surfaceTest2);
  di->OnEvent(&mafEvent(this,VME_OUTPUT_DATA_UPDATE));

  result = surfaceTest2->Name.Equals("CATCHED");
  TEST_RESULT;

}

//----------------------------------------------------------------------------
void mafDataPipeInterpolatorTest::TestUpdateBounds()
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

  mafSmartPointer<mafDataPipeInterpolatorTestClass> di;
  di->SetVME(surfaceTest);
  di->UpdateBounds();

  mafOBB *bounds;
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
void mafDataPipeInterpolatorTest::TestGetCurrentItem()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMESurfaceTestClass> surfaceTest;
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  int numberOfSpherePoints = sphere->GetOutput()->GetNumberOfPoints();

  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  mafSmartPointer<mafDataPipeInterpolatorTestClass> di;
  di->SetVME(surfaceTest);
  di->UpdateBounds();

  mafVMEItem *currentItem;
  currentItem = di->GetCurrentItem();
  int numberOfPoints = mafVMEItemVTK::SafeDownCast(currentItem)->GetData()->GetNumberOfPoints();

  result = currentItem != NULL && (numberOfSpherePoints == numberOfPoints);
  //printf("Points %d - %d", numberOfSpherePoints, numberOfPoints);

  TEST_RESULT;

}
//----------------------------------------------------------------------------
void mafDataPipeInterpolatorTest::TestSetTimeStamp()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafDataPipeInterpolatorTestClass> di;

  mafTimeStamp time = 30.;
  di->SetTimeStamp(time);
  result = di->GetTimeStamp() == time;

  TEST_RESULT;
}

//----------------------------------------------------------------------------
void mafDataPipeInterpolatorTest::GetMTime()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafDataPipeInterpolatorTestClass> di;

  long time1, time2;
  time1 = di->GetMTime();
  di->Modified();
  di->Update();
  time2 = di->GetMTime();

  result = time2 > time1;
  TEST_RESULT;

}