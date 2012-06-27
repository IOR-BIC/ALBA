/*=========================================================================

 Program: MAF2
 Module: mafOpAddLandmarkTest
 Authors: Matteo Giacomoni
 
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
#include "mafOpAddLandmarkTest.h"

#include "mafOpAddLandmark.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMEGroup.h"

#include "vtkMAFSmartPointer.h"
#include "vtkDataSet.h"
#include "vtkSphereSource.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
class DummyObserver : public mafObserver
  //----------------------------------------------------------------------------
{
public:

  DummyObserver() {};
  ~DummyObserver(){};

  virtual void OnEvent(mafEventBase *maf_event);

protected:

};
//----------------------------------------------------------------------------
void DummyObserver::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    //mafEvent *eventToCopy = new mafEvent(e->GetSender(),e->GetId(),e->GetVme());

    if (e->GetId() == VME_REMOVE)
    {
      e->GetVme()->ReparentTo(NULL);
    }
  }
}

//----------------------------------------------------------------------------
void mafOpAddLandmarkTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpAddLandmarkTest::setUp()
//----------------------------------------------------------------------------
{
  m_Result = false;
}
//----------------------------------------------------------------------------
void mafOpAddLandmarkTest::tearDown()
//----------------------------------------------------------------------------
{
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafOpAddLandmarkTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafOpAddLandmark op;
}
//----------------------------------------------------------------------------
void mafOpAddLandmarkTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafOpAddLandmark *op = new mafOpAddLandmark();
  cppDEL(op);
}
//----------------------------------------------------------------------------
void mafOpAddLandmarkTest::TestOpUndo()
//----------------------------------------------------------------------------
{
  mafOpAddLandmark *op = new mafOpAddLandmark();
  double lnd2[3]={1.0,0.0,2.0};
  double lnd1[3]={0.0,0.0,0.0};

  mafSmartPointer<mafVMESurface> surface;
  vtkMAFSmartPointer<vtkSphereSource> sphere;
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
  op->OpDo();

  op->GetCloud()->Update();

  op->OpUndo();

  m_Result = surface->GetNumberOfChildren() == 0;
  TEST_RESULT;

  cppDEL(op);

  op = new mafOpAddLandmark();

  mafSmartPointer<mafVMELandmarkCloud> landmarkCloud;
  landmarkCloud->TestModeOn();
  landmarkCloud->ReparentTo(surface);
  landmarkCloud->Open();
  mafSmartPointer<mafVMELandmark> landmark;
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
  op->OpDo();

  op->GetCloud()->Update();

  op->OpUndo();

  m_Result = surface->GetNumberOfChildren() == 1;
  TEST_RESULT;
  m_Result = op->GetCloud()->GetNumberOfChildren() == 1;
  TEST_RESULT;

  cppDEL(op);

  op = new mafOpAddLandmark();

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
  op->OpDo();

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
void mafOpAddLandmarkTest::TestAddLandmark()
//----------------------------------------------------------------------------
{
  mafOpAddLandmark *op = new mafOpAddLandmark();

  double lnd1[3]={0.0,0.0,0.0};
  double lnd2[3]={1.0,0.0,2.0};

  mafSmartPointer<mafVMESurface> surface;
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->GetOutput()->Update();
  surface->Update();

  //Input Surface
  op->TestModeOn();
  op->Accept(surface);
  op->SetInput(surface);
  op->OpRun();

  op->SetLandmarkName("one");
  op->AddLandmark(lnd1);
  op->SetLandmarkName("two");
  op->AddLandmark(lnd2);
  op->OpDo();

  op->GetCloud()->Update();

  m_Result = op->GetCloud()->GetChild(0)->IsA("mafVMELandmark");
  TEST_RESULT;
  double lnd1Out[3],lnd2Out[3];
  mafVMELandmark::SafeDownCast(op->GetCloud()->GetChild(0))->GetPoint(lnd1Out);
  m_Result = lnd1[0] == lnd1Out[0] && lnd1[1] == lnd1Out[1] && lnd1[2] == lnd1Out[2];
  TEST_RESULT;
  mafVMELandmark::SafeDownCast(op->GetCloud()->GetChild(1))->GetPoint(lnd2Out);
  m_Result = lnd2[0] == lnd2Out[0] && lnd2[1] == lnd2Out[1] && lnd2[2] == lnd2Out[2];
  TEST_RESULT;

  cppDEL(op);

  mafSmartPointer<mafVMELandmark> landmark;
  landmark->SetPoint(5.0,5.0,0.0);
  mafSmartPointer<mafVMELandmarkCloud> landmarkCloud;
  landmarkCloud->TestModeOn();
  landmarkCloud->Open();

  landmarkCloud->ReparentTo(surface);
  landmark->ReparentTo(landmarkCloud);

  landmark->GetOutput()->Update();
  landmark->Update();

  cppDEL(op);

  op = new mafOpAddLandmark();

  //Input Landmark
  op->TestModeOn();
  op->Accept(landmark);
  op->SetInput(landmark);
  op->OpRun();

  op->SetLandmarkName("one");
  op->AddLandmark(lnd1);
  op->SetLandmarkName("two");
  op->AddLandmark(lnd2);
  op->OpDo();
 
  op->GetCloud()->Update();

  m_Result = op->GetCloud()->GetNumberOfChildren() == 3;
  TEST_RESULT;
  mafVMELandmark::SafeDownCast(op->GetCloud()->GetChild(1))->GetPoint(lnd1Out);
  m_Result = lnd1[0] == lnd1Out[0] && lnd1[1] == lnd1Out[1] && lnd1[2] == lnd1Out[2];
  TEST_RESULT;
  mafVMELandmark::SafeDownCast(op->GetCloud()->GetChild(2))->GetPoint(lnd2Out);
  m_Result = lnd2[0] == lnd2Out[0] && lnd2[1] == lnd2Out[1] && lnd2[2] == lnd2Out[2];
  TEST_RESULT;

  cppDEL(op);

  op = new mafOpAddLandmark();

  //Input LandmarkCloud
  op->TestModeOn();
  op->Accept(landmarkCloud);
  op->SetInput(landmarkCloud);
  op->OpRun();

  op->SetLandmarkName("one");
  op->AddLandmark(lnd1);
  op->SetLandmarkName("two");
  op->AddLandmark(lnd2);
  op->OpDo();

  op->GetCloud()->Update();

  m_Result = op->GetCloud()->GetNumberOfChildren() == 3;
  TEST_RESULT;
  mafVMELandmark::SafeDownCast(op->GetCloud()->GetChild(1))->GetPoint(lnd1Out);
  m_Result = lnd1[0] == lnd1Out[0] && lnd1[1] == lnd1Out[1] && lnd1[2] == lnd1Out[2];
  TEST_RESULT;
  mafVMELandmark::SafeDownCast(op->GetCloud()->GetChild(2))->GetPoint(lnd2Out);
  m_Result = lnd2[0] == lnd2Out[0] && lnd2[1] == lnd2Out[1] && lnd2[2] == lnd2Out[2];
  TEST_RESULT;

  cppDEL(op);
}
//----------------------------------------------------------------------------
void mafOpAddLandmarkTest::TestOpRun()
//----------------------------------------------------------------------------
{
  mafOpAddLandmark *op = new mafOpAddLandmark();

  mafSmartPointer<mafVMESurface> surface;
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  surface->SetData(sphere->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->GetOutput()->Update();
  surface->Update();

  //Input Surface
  op->TestModeOn();
  op->Accept(surface);
  op->SetInput(surface);
  op->OpRun();

  m_Result = surface->GetChild(0)->IsA("mafVMELandmarkCloud");
  TEST_RESULT;
  m_Result = op->GetPickedVme() == surface.GetPointer();
  TEST_RESULT;
  m_Result = op->GetCloud() == surface->GetChild(0);
  TEST_RESULT;


  mafSmartPointer<mafVMELandmark> landmark;
  mafSmartPointer<mafVMELandmarkCloud> landmarkCloud;
  landmarkCloud->TestModeOn();
  landmarkCloud->Open();

  landmarkCloud->ReparentTo(surface);
  landmark->ReparentTo(landmarkCloud);

  landmark->GetOutput()->Update();
  landmark->Update();

  cppDEL(op);

  op = new mafOpAddLandmark();

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

  op = new mafOpAddLandmark();

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

  cppDEL(op);
}
//----------------------------------------------------------------------------
void mafOpAddLandmarkTest::TestAccept()
//----------------------------------------------------------------------------
{
  mafOpAddLandmark *op = new mafOpAddLandmark();
  
  //Surface is accepted as input
  mafSmartPointer<mafVMESurface> surface;
  m_Result = op->Accept(surface);
  TEST_RESULT;
  m_Result = op->GetPickingActiveFlag();
  TEST_RESULT;

  //Volume is accepted as input
  mafSmartPointer<mafVMEVolumeGray> volume;
  m_Result = op->Accept(volume);
  TEST_RESULT;
  m_Result = op->GetPickingActiveFlag();
  TEST_RESULT;

  //Surface Parametric is accepted as input
  mafSmartPointer<mafVMESurfaceParametric> surfaceParametric;
  m_Result = op->Accept(surfaceParametric);
  TEST_RESULT;
  m_Result = op->GetPickingActiveFlag();
  TEST_RESULT;

  //LandmarkCloud isn't accepted as input
  mafSmartPointer<mafVMELandmarkCloud> landmarkCloud;
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
  landmarkCloud->Open();

  //A landmark without a landmarkcloud is a valid input
  mafSmartPointer<mafVMELandmark> landmark;
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
  mafSmartPointer<mafVMEGroup> group;
  m_Result = op->Accept(group);
  TEST_RESULT;

  //NULL isn't a valid input
  m_Result = !op->Accept(NULL);
  TEST_RESULT;

  cppDEL(op);
}