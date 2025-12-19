/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAClipSurfaceBoundingBoxTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include <cppunit/config/SourcePrefix.h>

#include "vtkALBAClipSurfaceBoundingBoxTest.h"
#include "vtkALBAClipSurfaceBoundingBox.h"
#include "vtkALBASmartPointer.h"

#include "vtkSphereSource.h"
#include "vtkPlaneSource.h"

#include "vtkPolyDataMapper.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkActor.h"
#include "vtkActorCollection.h"
#include "vtkCamera.h"
#include "vtkPointData.h"
#include "vtkTimerLog.h"

enum ID_TEST_LIST
{
  ID_EXECUTION_TEST = 0,
};
//--------------------------------------------------
std::string vtkALBAClipSurfaceBoundingBoxTest::ConvertInt(int number)
//--------------------------------------------------
{
  std::stringstream stringStream;
  stringStream << number;//add number to the stream
  return stringStream.str();//return a string with the contents of the stream
}
//--------------------------------------------------
void vtkALBAClipSurfaceBoundingBoxTest::BeforeTest()
//--------------------------------------------------
{
	InitializeRenderWindow();

  m_SphereInput = vtkSphereSource::New();
  m_PlaneMask = vtkPlaneSource::New();
}

//--------------------------------------------------
void vtkALBAClipSurfaceBoundingBoxTest::AfterTest()
//--------------------------------------------------
{
  m_SphereInput->Delete();
  m_PlaneMask->Delete();
}
//--------------------------------------------------
void vtkALBAClipSurfaceBoundingBoxTest::TestDynamicAllocation()
//--------------------------------------------------
{
  vtkALBASmartPointer<vtkALBAClipSurfaceBoundingBox> filter;
  vtkALBAClipSurfaceBoundingBox *filter2 = vtkALBAClipSurfaceBoundingBox::New();
  filter2->Delete();
}
//--------------------------------------------------
void vtkALBAClipSurfaceBoundingBoxTest::TestSetGetMask()
//--------------------------------------------------
{
  vtkALBASmartPointer<vtkALBAClipSurfaceBoundingBox> filter;
  filter->SetMask(m_PlaneMask->GetOutput());

  CPPUNIT_ASSERT(filter->GetMask() == m_PlaneMask->GetOutput());
}
//--------------------------------------------------
void vtkALBAClipSurfaceBoundingBoxTest::TestSetGetClipInside()
//--------------------------------------------------
{
  vtkALBASmartPointer<vtkALBAClipSurfaceBoundingBox> filter;
  filter->SetClipInside(true);

  CPPUNIT_ASSERT(filter->GetClipInside() == true);  
  filter->SetClipInside(false);
  CPPUNIT_ASSERT(filter->GetClipInside() == false);  
}
//--------------------------------------------------
void vtkALBAClipSurfaceBoundingBoxTest::TestExecutionClipInsideOff()
//--------------------------------------------------
{
  TestExecution(false);
}
//--------------------------------------------------
void vtkALBAClipSurfaceBoundingBoxTest::TestExecutionClipInsideOn()
//--------------------------------------------------
{
  TestExecution(true);
}
//--------------------------------------------------
void vtkALBAClipSurfaceBoundingBoxTest::TestExecution(int clipInside)
//--------------------------------------------------
{
  m_TestNumber = ID_EXECUTION_TEST + clipInside;

  m_SphereInput->SetRadius(5.);
  m_SphereInput->SetCenter(0.,0.,0.);
  m_SphereInput->Update();

  int numberOfPoints = m_SphereInput->GetOutput()->GetNumberOfPoints();

  //set correct bounds
  m_PlaneMask->SetOrigin(0.,0.,0.);
  m_PlaneMask->SetPoint1(5.0,0.0,0.0);
  m_PlaneMask->SetPoint2(0.0,0.0,5.0);
  m_PlaneMask->Update();

  vtkALBASmartPointer<vtkALBAClipSurfaceBoundingBox> filter;
  filter->SetInput(m_SphereInput->GetOutput());
  filter->SetMask(m_PlaneMask->GetOutput());
  filter->SetClipInside(clipInside);
  filter->Update();

  vtkPolyData *result = filter->GetOutput();

  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInput(result);
  mapper->Update();

  vtkPolyDataMapper *mapperMask = vtkPolyDataMapper::New();
  mapperMask->SetInput(m_PlaneMask->GetOutput());
  mapperMask->Update();

  vtkActor *actor;
  actor = vtkActor::New();
  actor->SetMapper(mapper);

  vtkActor *actorMask;
  actorMask = vtkActor::New();
  actorMask->SetMapper(mapperMask);

  vtkActorCollection *coll = vtkActorCollection::New();
  coll->AddItem(actor);
  coll->AddItem(actorMask);
  RenderData(coll);

  coll->Delete();
  actor->Delete();
  mapper->Delete();

  actorMask->Delete();
  mapperMask->Delete();
}
//------------------------------------------------------------
void vtkALBAClipSurfaceBoundingBoxTest::RenderData(vtkActorCollection *actorCollection)
//------------------------------------------------------------
{
  vtkCamera *camera = m_Renderer->GetActiveCamera();
  camera->SetPosition(20.0,3.0,20.0);
  camera->Modified();


  actorCollection->InitTraversal();
  for (vtkProp *actor = actorCollection->GetNextProp(); actor != NULL; actor = actorCollection->GetNextProp()) 
  {
    m_Renderer->AddActor(actor);  
  }

  m_Renderer->ResetCamera(); 
  m_RenderWindow->Render();
	COMPARE_IMAGES("RenderData", m_TestNumber);

  vtkTimerLog::CleanupLog();
}