/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeCustomTest
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
#include "albaDataPipeCustomTest.h"

#include "albaDataPipeCustom.h"
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

#define EPSILON 0.1



//----------------------------------------------------------------------------
void albaDataPipeCustomTest::TestFixture()
{
}

//----------------------------------------------------------------------------
void albaDataPipeCustomTest::TestDynamicAllocation()
{
  albaSmartPointer<albaDataPipeCustom> di;

	albaDataPipeCustom * di2;
	albaNEW(di2);
	albaDEL(di2);
}
//----------------------------------------------------------------------------
void albaDataPipeCustomTest::TestOnEvent()
{
  //catch events inside vme : VME_OUTPUT_DATA_PREUPDATE, VME_OUTPUT_DATA_UPDATE

  albaSmartPointer<albaVMESurfaceTestClass> surfaceTest1;

  albaSmartPointer<albaDataPipeCustom> di;
  di->SetVME(surfaceTest1);
  di->OnEvent(&albaEvent(di->GetVTKDataPipe(),VME_OUTPUT_DATA_PREUPDATE)); //sender must be datapipe

  CPPUNIT_ASSERT(surfaceTest1->Name.Equals("CATCHED"));

  albaSmartPointer<albaVMESurfaceTestClass> surfaceTest2;
  di->SetVME(surfaceTest2);
  di->OnEvent(&albaEvent(di->GetVTKDataPipe(),VME_OUTPUT_DATA_UPDATE)); //sender must be datapipe

	CPPUNIT_ASSERT(surfaceTest2->Name.Equals("CATCHED"));
 }
//----------------------------------------------------------------------------
void albaDataPipeCustomTest::TestGetVTKData()
{
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->SetThetaResolution(100);
  sphere->SetPhiResolution(100);
  sphere->SetRadius(5.0);
  sphere->Update();

  albaSmartPointer<albaDataPipeCustom> di;

  di->GetVTKDataPipe()->SetInputData(sphere->GetOutput());

  vtkDataSet *dataset;
  dataset = di->GetVTKData();

  double numberOfPoints = dataset->GetNumberOfPoints();

	CPPUNIT_ASSERT(numberOfPoints == sphere->GetOutput()->GetNumberOfPoints());
}
//----------------------------------------------------------------------------
void albaDataPipeCustomTest::TestGetVTKDataPipe()
{
  //control that vtkALBADataPipe is different from NULL
  albaSmartPointer<albaDataPipeCustom> di;

	CPPUNIT_ASSERT(di->GetVTKDataPipe() != NULL);
}

//----------------------------------------------------------------------------
void albaDataPipeCustomTest::TestUpdate()
{
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->SetThetaResolution(100);
  sphere->SetPhiResolution(100);
  sphere->SetRadius(5.0);
  sphere->Update();

  albaSmartPointer<albaDataPipeCustom> di;

  di->GetVTKDataPipe()->SetInputData(sphere->GetOutput());

  vtkDataSet *dataset;
  dataset = di->GetVTKData();

  double numberOfPoints = dataset->GetNumberOfPoints();
  
  numberOfPoints = dataset->GetNumberOfPoints();
	CPPUNIT_ASSERT(numberOfPoints == sphere->GetOutput()->GetNumberOfPoints());
}
//----------------------------------------------------------------------------
void albaDataPipeCustomTest::TestUpdateBounds()
{
  albaSmartPointer<albaVMESurfaceTestClass> surfaceTest;
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->SetThetaResolution(100);
  sphere->SetPhiResolution(100);
  sphere->SetRadius(5.0);
  sphere->Update();

  surfaceTest->SetData(sphere->GetOutput(), 0.0);
  surfaceTest->Update();

  albaSmartPointer<albaDataPipeCustom> di;
  di->GetVTKDataPipe()->SetInputData(sphere->GetOutput());
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

	CPPUNIT_ASSERT((fabs(controlledBounds[0] - bounds->m_Bounds[0]) < EPSILON) && \
		(fabs(controlledBounds[1] - bounds->m_Bounds[1]) < EPSILON) && \
		(fabs(controlledBounds[2] - bounds->m_Bounds[2]) < EPSILON) && \
		(fabs(controlledBounds[3] - bounds->m_Bounds[3]) < EPSILON) && \
		(fabs(controlledBounds[4] - bounds->m_Bounds[4]) < EPSILON) && \
		(fabs(controlledBounds[5] - bounds->m_Bounds[5]) < EPSILON));

}

//----------------------------------------------------------------------------
void albaDataPipeCustomTest::TestSetInput()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->SetThetaResolution(100);
  sphere->SetPhiResolution(100);
  sphere->SetRadius(5.0);
  sphere->Update();

  albaSmartPointer<albaDataPipeCustom> di;

  di->SetInput(sphere->GetOutput());
  di->Update();

  vtkDataSet *dataset;
  dataset = di->GetVTKData();

  double numberOfPoints = dataset->GetNumberOfPoints();
  CPPUNIT_ASSERT(numberOfPoints == sphere->GetOutput()->GetNumberOfPoints());
}
//----------------------------------------------------------------------------
void albaDataPipeCustomTest::TestSetNthInput()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkSphereSource> sphere1;
  sphere1->SetThetaResolution(3);
  sphere1->SetPhiResolution(3);
  sphere1->SetRadius(5.0);
  sphere1->Update();

  vtkALBASmartPointer<vtkSphereSource> sphere2;
  sphere2->SetThetaResolution(5);
  sphere2->SetPhiResolution(5);
  sphere2->SetRadius(10.0);
  sphere2->Update();

  vtkALBASmartPointer<vtkSphereSource> sphere3;
  sphere3->SetThetaResolution(10);
  sphere3->SetPhiResolution(10);
  sphere3->SetRadius(15.0);
  sphere3->Update();

  albaSmartPointer<albaDataPipeCustom> di;

  di->SetNthInput(0, sphere1->GetOutput());
  di->SetNthInput(1, sphere2->GetOutput());
  di->SetNthInput(2, sphere3->GetOutput());
  di->Update();

  vtkPolyData *dataset1 = (vtkPolyData *)di->GetVTKDataPipe()->GetOutput(0);
  vtkPolyData *dataset2 = (vtkPolyData *)di->GetVTKDataPipe()->GetOutput(1);
  vtkPolyData *dataset3 = (vtkPolyData *)di->GetVTKDataPipe()->GetOutput(2);

  double numberOfPoints1 = dataset1->GetNumberOfPoints();
	CPPUNIT_ASSERT(numberOfPoints1 == sphere1->GetOutput()->GetNumberOfPoints());
 

  double numberOfPoints2 = dataset2->GetNumberOfPoints();
	CPPUNIT_ASSERT(numberOfPoints2 == sphere2->GetOutput()->GetNumberOfPoints());


  double numberOfPoints3 = dataset3->GetNumberOfPoints();
  CPPUNIT_ASSERT(numberOfPoints3 == sphere3->GetOutput()->GetNumberOfPoints());
}