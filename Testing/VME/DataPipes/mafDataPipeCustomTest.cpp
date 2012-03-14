/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafDataPipeCustomTest.cpp,v $
Language:  C++
Date:      $Date: 2011-05-25 11:58:33 $
Version:   $Revision: 1.1.2.1 $
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
#include "mafDataPipeCustomTest.h"

#include "mafDataPipeCustom.h"
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
#define EPSILON 0.1

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
void mafDataPipeCustomTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafDataPipeCustomTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void mafDataPipeCustomTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafDataPipeCustomTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafDataPipeCustom> di;

	mafDataPipeCustom * di2;
	mafNEW(di2);
	mafDEL(di2);
}
//----------------------------------------------------------------------------
void mafDataPipeCustomTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  //catch events inside vme : VME_OUTPUT_DATA_PREUPDATE, VME_OUTPUT_DATA_UPDATE

  mafSmartPointer<mafVMETestClass> surfaceTest1;

  mafSmartPointer<mafDataPipeCustom> di;
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
void mafDataPipeCustomTest::TestGetVTKData()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->SetThetaResolution(100);
  sphere->SetPhiResolution(100);
  sphere->SetRadius(5.0);
  sphere->Update();

  mafSmartPointer<mafDataPipeCustom> di;

  di->GetVTKDataPipe()->SetInput(sphere->GetOutput());
  di->GetVTKDataPipe()->GetOutput()->Update();

  vtkDataSet *dataset;
  dataset = di->GetVTKData();
  dataset->Update();

  double numberOfPoints = dataset->GetNumberOfPoints();

  result =  numberOfPoints == sphere->GetOutput()->GetNumberOfPoints();

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafDataPipeCustomTest::TestGetVTKDataPipe()
//----------------------------------------------------------------------------
{
  //control that vtkMAFDataPipe is different from NULL
  mafSmartPointer<mafDataPipeCustom> di;
  result = NULL != di->GetVTKDataPipe();

  TEST_RESULT;
}

//----------------------------------------------------------------------------
void mafDataPipeCustomTest::TestUpdate()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->SetThetaResolution(100);
  sphere->SetPhiResolution(100);
  sphere->SetRadius(5.0);
  sphere->Update();

  mafSmartPointer<mafDataPipeCustom> di;

  di->GetVTKDataPipe()->SetInput(sphere->GetOutput());

  vtkDataSet *dataset;
  dataset = di->GetVTKData();

  double numberOfPoints = dataset->GetNumberOfPoints();
  result =  numberOfPoints == sphere->GetOutput()->GetNumberOfPoints();
  
  //must be false because there isn't already update
  result = !result;

  TEST_RESULT;

  di->Update();
   
  //now must be updated
  result = false;
  numberOfPoints = dataset->GetNumberOfPoints();
  result =  numberOfPoints == sphere->GetOutput()->GetNumberOfPoints();

  TEST_RESULT;

}
//----------------------------------------------------------------------------
void mafDataPipeCustomTest::TestUpdateBounds()
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

  mafSmartPointer<mafDataPipeCustom> di;
  di->GetVTKDataPipe()->SetInput(sphere->GetOutput());
  di->GetVTKDataPipe()->GetOutput()->Update();
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
void mafDataPipeCustomTest::TestSetInput()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->SetThetaResolution(100);
  sphere->SetPhiResolution(100);
  sphere->SetRadius(5.0);
  sphere->Update();

  mafSmartPointer<mafDataPipeCustom> di;

  di->SetInput(sphere->GetOutput());
  di->Update();

  vtkDataSet *dataset;
  dataset = di->GetVTKData();
  dataset->Update();

  double numberOfPoints = dataset->GetNumberOfPoints();
  result =  numberOfPoints == sphere->GetOutput()->GetNumberOfPoints();

  TEST_RESULT;

}
//----------------------------------------------------------------------------
void mafDataPipeCustomTest::TestSetNthInput()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkSphereSource> sphere1;
  sphere1->SetThetaResolution(3);
  sphere1->SetPhiResolution(3);
  sphere1->SetRadius(5.0);
  sphere1->Update();

  vtkMAFSmartPointer<vtkSphereSource> sphere2;
  sphere2->SetThetaResolution(5);
  sphere2->SetPhiResolution(5);
  sphere2->SetRadius(10.0);
  sphere2->Update();

  vtkMAFSmartPointer<vtkSphereSource> sphere3;
  sphere3->SetThetaResolution(10);
  sphere3->SetPhiResolution(10);
  sphere3->SetRadius(15.0);
  sphere3->Update();

  mafSmartPointer<mafDataPipeCustom> di;

  di->SetNthInput(0, sphere1->GetOutput());
  di->SetNthInput(1, sphere2->GetOutput());
  di->SetNthInput(2, sphere3->GetOutput());
  di->Update();

  vtkPolyData *dataset1 = (vtkPolyData *)di->GetVTKDataPipe()->GetOutput(0);
  dataset1->Update();
  vtkPolyData *dataset2 = (vtkPolyData *)di->GetVTKDataPipe()->GetOutput(1);
  dataset2->Update();
  vtkPolyData *dataset3 = (vtkPolyData *)di->GetVTKDataPipe()->GetOutput(2);
  dataset3->Update();

  double numberOfPoints1 = dataset1->GetNumberOfPoints();
  result =  numberOfPoints1 == sphere1->GetOutput()->GetNumberOfPoints();
  TEST_RESULT;

  double numberOfPoints2 = dataset2->GetNumberOfPoints();
  result =  numberOfPoints2 == sphere2->GetOutput()->GetNumberOfPoints();
  TEST_RESULT;

  double numberOfPoints3 = dataset3->GetNumberOfPoints();
  result =  numberOfPoints3 == sphere3->GetOutput()->GetNumberOfPoints();
  TEST_RESULT;

}