/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVTKInterpolatorTest.cpp,v $
Language:  C++
Date:      $Date: 2008-06-23 13:03:27 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
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
#include "mafVTKInterpolatorTest.h"

#include "mafVTKInterpolator.h"
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

/** a vme test  class used to test event reception. */
class mafVMETestClass: public mafVMESurface
{
public:
	mafVMETestClass(const char *name=NULL):Name(name) {}
	mafTypeMacro(mafVMETestClass,mafVMESurface);

	virtual void OnEvent(mafEventBase *event) {Name = "CATCHED";};

	mafString     Name;
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMETestClass)
//-------------------------------------------------------------------------


//----------------------------------------------------------------------------
void mafVTKInterpolatorTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVTKInterpolatorTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void mafVTKInterpolatorTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVTKInterpolatorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVTKInterpolator> di;

	mafVTKInterpolator *di2;
	mafNEW(di2);
	mafDEL(di2);
}
//----------------------------------------------------------------------------
void mafVTKInterpolatorTest::TestAccept()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMETestClass> surfaceTest;
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  
  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  mafSmartPointer<mafVTKInterpolator> di;
  result = di->Accept(surfaceTest);
  TEST_RESULT;

  result = !(di->Accept(NULL));
  TEST_RESULT;
}

//----------------------------------------------------------------------------
void mafVTKInterpolatorTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  //catch events inside vme : VME_OUTPUT_DATA_PREUPDATE, VME_OUTPUT_DATA_UPDATE

  mafSmartPointer<mafVMETestClass> surfaceTest1;

  mafSmartPointer<mafVTKInterpolator> di;
  di->SetVME(surfaceTest1);
  di->OnEvent(&mafEvent(di->GetVTKDataPipe(),VME_OUTPUT_DATA_PREUPDATE)); //sender must be datapipe
  
  result = surfaceTest1->Name.Equals("CATCHED");
  TEST_RESULT;

  mafSmartPointer<mafVMETestClass> surfaceTest2;
  di->SetVME(surfaceTest2);
  di->OnEvent(&mafEvent(di->GetVTKDataPipe(),VME_OUTPUT_DATA_UPDATE)); //sender must be datapipe

  result = surfaceTest2->Name.Equals("CATCHED");
  TEST_RESULT;

}

//----------------------------------------------------------------------------
void mafVTKInterpolatorTest::TestGetVTKData()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMETestClass> surfaceTest;
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->SetThetaResolution(100);
  sphere->SetPhiResolution(100);
  sphere->SetRadius(5.0);
  sphere->Update();

  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  mafSmartPointer<mafVTKInterpolator> di;
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
void mafVTKInterpolatorTest::TestGetCurrentItem()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMETestClass> surfaceTest;
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  int numberOfSpherePoints = sphere->GetOutput()->GetNumberOfPoints();

  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  mafSmartPointer<mafVTKInterpolator> di;
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
void mafVTKInterpolatorTest::TestUpdate()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMETestClass> surfaceTest;
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->SetThetaResolution(100);
  sphere->SetPhiResolution(100);
  sphere->SetRadius(5.0);
  sphere->Update();

  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  

  mafSmartPointer<mafVTKInterpolator> di;
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
void mafVTKInterpolatorTest::GetVTKDataPipe()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVTKInterpolator> di;
  result = NULL != di->GetVTKDataPipe();
  TEST_RESULT;
}