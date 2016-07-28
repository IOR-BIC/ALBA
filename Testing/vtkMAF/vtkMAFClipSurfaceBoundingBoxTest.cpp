/*=========================================================================

 Program: MAF2
 Module: vtkMAFClipSurfaceBoundingBoxTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>

#include "vtkMAFClipSurfaceBoundingBoxTest.h"
#include "vtkMAFClipSurfaceBoundingBox.h"
#include "vtkMAFSmartPointer.h"

#include "vtkSphereSource.h"
#include "vtkPlaneSource.h"

#include "vtkPolyDataMapper.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMAFSmartPointer.h"
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
std::string vtkMAFClipSurfaceBoundingBoxTest::ConvertInt(int number)
//--------------------------------------------------
{
  std::stringstream stringStream;
  stringStream << number;//add number to the stream
  return stringStream.str();//return a string with the contents of the stream
}
//--------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::BeforeTest()
//--------------------------------------------------
{
	vtkNEW(m_Renderer);
	vtkNEW(m_RenderWindow);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(640, 480);
	m_RenderWindow->SetPosition(100, 0);

	m_Renderer->SetBackground(0.0, 0.0, 0.0);

  m_SphereInput = vtkSphereSource::New();
  m_PlaneMask = vtkPlaneSource::New();
}

//--------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::AfterTest()
//--------------------------------------------------
{
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);

  m_SphereInput->Delete();
  m_PlaneMask->Delete();
}
//--------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::TestDynamicAllocation()
//--------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFClipSurfaceBoundingBox> filter;
  vtkMAFClipSurfaceBoundingBox *filter2 = vtkMAFClipSurfaceBoundingBox::New();
  filter2->Delete();
}
//--------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::TestSetGetMask()
//--------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFClipSurfaceBoundingBox> filter;
  filter->SetMask(m_PlaneMask->GetOutput());

  CPPUNIT_ASSERT(filter->GetMask() == m_PlaneMask->GetOutput());
}
//--------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::TestSetGetClipInside()
//--------------------------------------------------
{
  vtkMAFSmartPointer<vtkMAFClipSurfaceBoundingBox> filter;
  filter->SetClipInside(TRUE);

  CPPUNIT_ASSERT(filter->GetClipInside() == TRUE);  
  filter->SetClipInside(FALSE);
  CPPUNIT_ASSERT(filter->GetClipInside() == FALSE);  
}
//--------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::TestExecutionClipInsideOff()
//--------------------------------------------------
{
  TestExecution(FALSE);
}
//--------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::TestExecutionClipInsideOn()
//--------------------------------------------------
{
  TestExecution(TRUE);
}
//--------------------------------------------------
void vtkMAFClipSurfaceBoundingBoxTest::TestExecution(int clipInside)
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

  vtkMAFSmartPointer<vtkMAFClipSurfaceBoundingBox> filter;
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
void vtkMAFClipSurfaceBoundingBoxTest::RenderData(vtkActorCollection *actorCollection)
//------------------------------------------------------------
{
  vtkCamera *camera = m_Renderer->GetActiveCamera();
  camera->SetPosition(20.0,3.0,20.0);
  camera->Modified();

  vtkMAFSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(m_RenderWindow);

  actorCollection->InitTraversal();
  for (vtkProp *actor = actorCollection->GetNextProp(); actor != NULL; actor = actorCollection->GetNextProp()) 
  {
    m_Renderer->AddActor(actor);  
  }

  m_RenderWindow->Render();
	COMPARE_IMAGES("RenderData", m_TestNumber);

  vtkTimerLog::CleanupLog();
}