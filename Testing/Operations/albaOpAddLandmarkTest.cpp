/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpAddLandmarkTest
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

#include <cppunit/config/SourcePrefix.h>
#include "albaOperationsTests.h"

#include "albaOpAddLandmarkTest.h"

#include "albaOpAddLandmark.h"
#include "albaVMESurface.h"
#include "albaVMEVolumeGray.h"
#include "albaVMESurfaceParametric.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMEGroup.h"

#include "vtkALBASmartPointer.h"
#include "vtkDataSet.h"
#include "vtkSphereSource.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void albaOpAddLandmarkTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpAddLandmarkTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaOpAddLandmark op;
}
//----------------------------------------------------------------------------
void albaOpAddLandmarkTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaOpAddLandmark *op = new albaOpAddLandmark();
  cppDEL(op);
}
//----------------------------------------------------------------------------
void albaOpAddLandmarkTest::TestOpUndo()
//----------------------------------------------------------------------------
{
  albaOpAddLandmark *op = new albaOpAddLandmark();
  double lnd2[3]={1.0,0.0,2.0};
  double lnd1[3]={0.0,0.0,0.0};

  albaSmartPointer<albaVMESurface> surface;
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->GetOutput()->Update();
  surface->Update();

  DummyObserver *observer = new DummyObserver();
  //Input Surface
  op->TestModeOn();
  op->SetListener(observer);
  op->Accept(surface);
  op->SetInput(surface);
  op->OpRun();

  op->SetLandmarkName("one");
  op->AddLandmark(lnd1);
  op->SetLandmarkName("two");
  op->AddLandmark(lnd2);
	op->OpStop(ALBA_OK);

  op->GetCloud()->Update();

  op->OpUndo();

  m_Result = surface->GetNumberOfChildren() == 0;
  TEST_RESULT;

  cppDEL(op);

  op = new albaOpAddLandmark();

  albaSmartPointer<albaVMELandmarkCloud> landmarkCloud;
  landmarkCloud->TestModeOn();
  landmarkCloud->ReparentTo(surface);
  albaSmartPointer<albaVMELandmark> landmark;
  landmark->SetName("FIX");
  landmark->SetPoint(1.0,1.0,2.0);
  landmark->ReparentTo(landmarkCloud);

  //Input Landmark
  op->TestModeOn();
  op->SetListener(observer);
  op->Accept(landmark);
  op->SetInput(landmark);
  op->OpRun();

  op->SetLandmarkName("one");
  op->AddLandmark(lnd1);
  op->SetLandmarkName("two");
  op->AddLandmark(lnd2);
	op->OpStop(ALBA_OK);

  op->GetCloud()->Update();

  op->OpUndo();

  m_Result = surface->GetNumberOfChildren() == 1;
  TEST_RESULT;
  m_Result = op->GetCloud()->GetNumberOfChildren() == 1;
  TEST_RESULT;

  cppDEL(op);

  op = new albaOpAddLandmark();

  //Input LandmarkCloud
  op->TestModeOn();
  op->SetListener(observer);
  op->Accept(landmarkCloud);
  op->SetInput(landmarkCloud);
  op->OpRun();
  
  op->SetLandmarkName("one");
  op->AddLandmark(lnd1);
  op->SetLandmarkName("two");
  op->AddLandmark(lnd2);
  op->OpStop(ALBA_OK);

  op->GetCloud()->Update();

  op->OpUndo();

  m_Result = surface->GetNumberOfChildren() == 1;
  TEST_RESULT;
  m_Result = landmarkCloud->GetNumberOfChildren() == 1;
  TEST_RESULT;

  cppDEL(observer);
  cppDEL(op);
}
//----------------------------------------------------------------------------
void albaOpAddLandmarkTest::TestAddLandmark()
//----------------------------------------------------------------------------
{
  albaOpAddLandmark *op = new albaOpAddLandmark();

  double lnd1[3]={0.0,0.0,0.0};
  double lnd2[3]={1.0,0.0,2.0};

  albaSmartPointer<albaVMESurface> surface;
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->GetOutput()->Update();
  surface->Update();

	DummyObserver *observer = new DummyObserver();
	op->SetListener(observer);

  //Input Surface
  op->TestModeOn();
  op->Accept(surface);
  op->SetInput(surface);
  op->OpRun();

  op->SetLandmarkName("one");
  op->AddLandmark(lnd1);
  op->SetLandmarkName("two");
	op->SelectLandmarkByName("Add_New_Landmark");
  op->AddLandmark(lnd2);
  op->OpDo();

  op->GetCloud()->Update();

	CPPUNIT_ASSERT(op->GetCloud()->GetNumberOfLandmarks() == 2);

  m_Result = op->GetCloud()->GetChild(0)->IsA("albaVMELandmark");
  TEST_RESULT;
  double lnd1Out[3],lnd2Out[3];
  albaVMELandmark::SafeDownCast(op->GetCloud()->GetChild(0))->GetPoint(lnd1Out);
  m_Result = lnd1[0] == lnd1Out[0] && lnd1[1] == lnd1Out[1] && lnd1[2] == lnd1Out[2];
  TEST_RESULT;
  albaVMELandmark::SafeDownCast(op->GetCloud()->GetChild(1))->GetPoint(lnd2Out);
  m_Result = lnd2[0] == lnd2Out[0] && lnd2[1] == lnd2Out[1] && lnd2[2] == lnd2Out[2];
  TEST_RESULT;

  cppDEL(op);

  albaSmartPointer<albaVMELandmark> landmark;
  landmark->SetPoint(5.0,5.0,0.0);
  albaSmartPointer<albaVMELandmarkCloud> landmarkCloud;
  landmarkCloud->TestModeOn();

  landmarkCloud->ReparentTo(surface);
  landmark->ReparentTo(landmarkCloud);

  landmark->GetOutput()->Update();
  landmark->Update();

  cppDEL(op);

  op = new albaOpAddLandmark();

	op->SetListener(observer);

  //Input Landmark
  op->TestModeOn();
  op->Accept(landmark);
  op->SetInput(landmark);
  op->OpRun();

  op->SetLandmarkName("one");
  op->AddLandmark(lnd1);
  op->SetLandmarkName("two");
	op->SelectLandmarkByName("Add_New_Landmark");
  op->AddLandmark(lnd2);
  op->OpDo();
 
  op->GetCloud()->Update();

  m_Result = op->GetCloud()->GetNumberOfChildren() == 3;
  TEST_RESULT;
  albaVMELandmark::SafeDownCast(op->GetCloud()->GetChild(1))->GetPoint(lnd1Out);
  m_Result = lnd1[0] == lnd1Out[0] && lnd1[1] == lnd1Out[1] && lnd1[2] == lnd1Out[2];
  TEST_RESULT;
  albaVMELandmark::SafeDownCast(op->GetCloud()->GetChild(2))->GetPoint(lnd2Out);
  m_Result = lnd2[0] == lnd2Out[0] && lnd2[1] == lnd2Out[1] && lnd2[2] == lnd2Out[2];
  TEST_RESULT;

  cppDEL(op);

  op = new albaOpAddLandmark();

	op->SetListener(observer);

	//Input LandmarkCloud
  op->TestModeOn();
  op->Accept(landmarkCloud);
  op->SetInput(landmarkCloud);
  op->OpRun();

  op->SetLandmarkName("one");
  op->AddLandmark(lnd1);
  op->SetLandmarkName("two");
  op->SelectLandmarkByName("Add_New_Landmark");
  op->AddLandmark(lnd2);
  op->OpDo();

  op->GetCloud()->Update();

  m_Result = op->GetCloud()->GetNumberOfChildren() == 5;
  TEST_RESULT;
  albaVMELandmark::SafeDownCast(op->GetCloud()->GetChild(1))->GetPoint(lnd1Out);
  m_Result = lnd1[0] == lnd1Out[0] && lnd1[1] == lnd1Out[1] && lnd1[2] == lnd1Out[2];
  TEST_RESULT;
  albaVMELandmark::SafeDownCast(op->GetCloud()->GetChild(2))->GetPoint(lnd2Out);
  m_Result = lnd2[0] == lnd2Out[0] && lnd2[1] == lnd2Out[1] && lnd2[2] == lnd2Out[2];
  TEST_RESULT;

	cppDEL(observer);
  cppDEL(op);
}
//----------------------------------------------------------------------------
void albaOpAddLandmarkTest::TestOpRun()
//----------------------------------------------------------------------------
{
  albaOpAddLandmark *op = new albaOpAddLandmark();

  albaSmartPointer<albaVMESurface> surface;
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->GetOutput()->Update();
  surface->Update();

	DummyObserver *observer = new DummyObserver();
	op->SetListener(observer);

  //Input Surface
  op->TestModeOn();
  op->Accept(surface);
  op->SetInput(surface);
  op->OpRun();

  m_Result = surface->GetChild(0)->IsA("albaVMELandmarkCloud");
  TEST_RESULT;
  m_Result = op->GetPickedVme() == surface.GetPointer();
  TEST_RESULT;
  m_Result = op->GetCloud() == surface->GetChild(0);
  TEST_RESULT;


  albaSmartPointer<albaVMELandmark> landmark;
  albaSmartPointer<albaVMELandmarkCloud> landmarkCloud;
  landmarkCloud->TestModeOn();

  landmarkCloud->ReparentTo(surface);
  landmark->ReparentTo(landmarkCloud);

  landmark->GetOutput()->Update();
  landmark->Update();

  cppDEL(op);

  op = new albaOpAddLandmark();

	op->SetListener(observer);

  //Input Landmark
  op->TestModeOn();
  op->Accept(landmark);
  op->SetInput(landmark);
  op->OpRun();

  m_Result = landmarkCloud->GetNumberOfChildren() == 1;
  TEST_RESULT;
  m_Result = landmark->GetChild(0) == NULL;
  TEST_RESULT;
  m_Result = op->GetPickedVme() == surface.GetPointer();
  TEST_RESULT;
  m_Result = op->GetCloud() == landmarkCloud.GetPointer();
  TEST_RESULT;

  cppDEL(op);

  op = new albaOpAddLandmark();

	op->SetListener(observer);

  //Input LandmarkCloud
  op->TestModeOn();
  op->Accept(landmarkCloud);
  op->SetInput(landmarkCloud);
  op->OpRun();

  m_Result = landmarkCloud->GetNumberOfChildren() == 1;
  TEST_RESULT;
  m_Result = op->GetPickedVme() == surface.GetPointer();
  TEST_RESULT;
  m_Result = op->GetCloud() == landmarkCloud.GetPointer();
  TEST_RESULT;

	cppDEL(observer);
  cppDEL(op);
}
//----------------------------------------------------------------------------
void albaOpAddLandmarkTest::TestAccept()
//----------------------------------------------------------------------------
{
  albaOpAddLandmark *op = new albaOpAddLandmark();
  
  //Surface is accepted as input
  albaSmartPointer<albaVMESurface> surface;
  m_Result = op->Accept(surface);
  TEST_RESULT;
	
  //Volume is accepted as input
  albaSmartPointer<albaVMEVolumeGray> volume;
  m_Result = op->Accept(volume);
  TEST_RESULT;

  //Surface Parametric is accepted as input
  albaSmartPointer<albaVMESurfaceParametric> surfaceParametric;
  m_Result = op->Accept(surfaceParametric);
  TEST_RESULT;

  //LandmarkCloud isn't accepted as input
  albaSmartPointer<albaVMELandmarkCloud> landmarkCloud;
  landmarkCloud->TestModeOn();
  m_Result = op->Accept(landmarkCloud);
  TEST_RESULT;

  //LandmarkCloud is accepted as input only if is a child of a volume
  landmarkCloud->ReparentTo(volume);
  m_Result = op->Accept(landmarkCloud);
  TEST_RESULT;

  //LandmarkCloud is accepted as input only if is a child of a surface
  landmarkCloud->ReparentTo(surface);
  m_Result = op->Accept(landmarkCloud);
  TEST_RESULT;

  //LandmarkCloud is accepted as input only if is a child of a surface parametric
  landmarkCloud->ReparentTo(surfaceParametric);
  m_Result = op->Accept(landmarkCloud);
  TEST_RESULT;

  landmarkCloud->ReparentTo(NULL);

  //A landmark without a landmarkcloud is a valid input
  albaSmartPointer<albaVMELandmark> landmark;
  m_Result = op->Accept(landmark);
  TEST_RESULT;

  //A landmark with a landmarkcloud is a valid input only if child of a volume/surface/surface parametric
  landmark->ReparentTo(landmarkCloud);
  m_Result = op->Accept(landmark);
  TEST_RESULT;

  landmarkCloud->ReparentTo(volume);
  m_Result = op->Accept(landmark);
  TEST_RESULT;

  landmarkCloud->ReparentTo(surface);
  m_Result = op->Accept(landmark);
  TEST_RESULT;

  landmarkCloud->ReparentTo(surfaceParametric);
  m_Result = op->Accept(landmark);
  TEST_RESULT;

  //Group is a valid input
  albaSmartPointer<albaVMEGroup> group;
  m_Result = op->Accept(group);
  TEST_RESULT;

  //NULL isn't a valid input
  m_Result = !op->Accept(NULL);
  TEST_RESULT;

  cppDEL(op);
}