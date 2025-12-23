/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAGridActorTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
#include "vtkALBAGridActor.h"
#include "vtkALBAGridActorTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkALBASmartPointer.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkPointData.h"

//----------------------------------------------------------------------------
void vtkALBAGridActorTest::BeforeTest()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void vtkALBAGridActorTest::AfterTest()
//----------------------------------------------------------------------------
{
}

//------------------------------------------------------------
void vtkALBAGridActorTest::RenderData(vtkActor *actor, char* testName)
{
  vtkCamera *camera = m_Renderer->GetActiveCamera();
  camera->ParallelProjectionOn();
  camera->Modified();

  vtkALBASmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(m_RenderWindow);

  m_Renderer->AddActor(actor);

	m_Renderer->ResetCamera();
  m_RenderWindow->Render();
	COMPARE_IMAGES(testName);
}
//------------------------------------------------------------
void vtkALBAGridActorTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkALBAGridActor *to = vtkALBAGridActor::New();
  to->Delete();
}
//--------------------------------------------
void vtkALBAGridActorTest::TestSetGridNormal()
//--------------------------------------------
{
  vtkALBAGridActor *actor;
  actor = vtkALBAGridActor::New();

  actor->SetGridNormal(GRID_Y);

  RenderData(actor, "TestSetGridNormal");
  actor->Delete();
}
//--------------------------------------------
void vtkALBAGridActorTest::TestSetGridPosition()
//--------------------------------------------
{
  vtkALBAGridActor *actor;
  actor = vtkALBAGridActor::New();

  actor->SetGridNormal(GRID_X);
  actor->SetGridPosition(0.9);

  RenderData(actor, "TestSetGridPosition");
  actor->Delete();
}
//--------------------------------------------
void vtkALBAGridActorTest::TestSetGridColor()
//--------------------------------------------
{
  vtkALBAGridActor *actor;
  actor = vtkALBAGridActor::New();

  actor->SetGridColor(1.0,0.0,0.0);

  RenderData(actor, "TestSetGridColor");
  actor->Delete();
}
//--------------------------------------------
void vtkALBAGridActorTest::TestGetLabelActor()
//--------------------------------------------
{
  vtkALBAGridActor *actor;
  actor = vtkALBAGridActor::New();

  RenderData(actor, "TestGetLabelActor");

	CPPUNIT_ASSERT(strcmp("0.2",actor->GetLabelActor()->GetInput())==0);

  actor->Delete();
}

//----------------------------------------------------------------------------
void vtkALBAGridActorTest::TestPrintSelf()
//----------------------------------------------------------------------------
{
  vtkALBAGridActor *actor;
  actor = vtkALBAGridActor::New();
  actor->PrintSelf(std::cout, 3);
  actor->Delete();
}