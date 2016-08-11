/*=========================================================================

 Program: MAF2
 Module: vtkMAFGridActorTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
#include "vtkMAFGridActor.h"
#include "vtkMAFGridActorTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMAFSmartPointer.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkPointData.h"

//----------------------------------------------------------------------------
void vtkMAFGridActorTest::BeforeTest()
//----------------------------------------------------------------------------
{
	vtkNEW(m_Renderer);
	vtkNEW(m_RenderWindow);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(640, 480);
	m_RenderWindow->SetPosition(100, 0);

	m_Renderer->SetBackground(0.0, 0.0, 0.0);
}
//----------------------------------------------------------------------------
void vtkMAFGridActorTest::AfterTest()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);
}

//------------------------------------------------------------
void vtkMAFGridActorTest::RenderData(vtkActor *actor, char* testName)
{
  vtkCamera *camera = m_Renderer->GetActiveCamera();
  camera->ParallelProjectionOn();
  camera->Modified();

  vtkMAFSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(m_RenderWindow);

  m_Renderer->AddActor(actor);
  m_RenderWindow->Render();

	COMPARE_IMAGES(testName);
}
//------------------------------------------------------------
void vtkMAFGridActorTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkMAFGridActor *to = vtkMAFGridActor::New();
  to->Delete();
}
//--------------------------------------------
void vtkMAFGridActorTest::TestSetGridNormal()
//--------------------------------------------
{
  vtkMAFGridActor *actor;
  actor = vtkMAFGridActor::New();

  actor->SetGridNormal(GRID_Y);

  RenderData(actor, "TestSetGridNormal");
  actor->Delete();
}
//--------------------------------------------
void vtkMAFGridActorTest::TestSetGridPosition()
//--------------------------------------------
{
  vtkMAFGridActor *actor;
  actor = vtkMAFGridActor::New();

  actor->SetGridNormal(GRID_X);
  actor->SetGridPosition(0.9);

  RenderData(actor, "TestSetGridPosition");
  actor->Delete();
}
//--------------------------------------------
void vtkMAFGridActorTest::TestSetGridColor()
//--------------------------------------------
{
  vtkMAFGridActor *actor;
  actor = vtkMAFGridActor::New();

  actor->SetGridColor(1.0,0.0,0.0);

  RenderData(actor, "TestSetGridColor");
  actor->Delete();
}
//--------------------------------------------
void vtkMAFGridActorTest::TestGetLabelActor()
//--------------------------------------------
{
  vtkMAFGridActor *actor;
  actor = vtkMAFGridActor::New();

  RenderData(actor, "TestGetLabelActor");

	CPPUNIT_ASSERT(strcmp("0.2",actor->GetLabelActor()->GetInput())==0);

  actor->Delete();
}

//----------------------------------------------------------------------------
void vtkMAFGridActorTest::TestPrintSelf()
//----------------------------------------------------------------------------
{
  vtkMAFGridActor *actor;
  actor = vtkMAFGridActor::New();
  actor->PrintSelf(std::cout, 3);
  actor->Delete();
}