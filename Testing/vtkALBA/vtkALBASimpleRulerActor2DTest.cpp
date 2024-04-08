/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBASimpleRulerActor2DTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
#include <cppunit/config/SourcePrefix.h>
#include "vtkALBASimpleRulerActor2D.h"
#include "vtkALBASimpleRulerActor2DTest.h"

#include "vtkRenderWindowInteractor.h"
#include "vtkALBASmartPointer.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkPointData.h"

//----------------------------------------------------------------------------
void vtkALBASimpleRulerActor2DTest::BeforeTest()
//----------------------------------------------------------------------------
{
	vtkNEW(m_Renderer);
	vtkNEW(m_RenderWindow);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(640, 480);
	m_RenderWindow->SetPosition(100, 0);

	vtkCamera *camera = m_Renderer->GetActiveCamera();
	camera->ParallelProjectionOn();
	camera->Modified();

	m_Renderer->SetBackground(0.0, 0.0, 0.0);
}
//----------------------------------------------------------------------------
void vtkALBASimpleRulerActor2DTest::AfterTest()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);
}

//------------------------------------------------------------------------------
void vtkALBASimpleRulerActor2DTest::RenderData(vtkActor2D *actor, char* testName)
{
  vtkALBASmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(m_RenderWindow);

	m_Renderer->AddActor2D(actor);
	m_RenderWindow->Render();

	COMPARE_IMAGES(testName);
}
//------------------------------------------------------------------------------
void vtkALBASimpleRulerActor2DTest::TestDynamicAllocation()
//------------------------------------------------------------------------------
{
  vtkALBASimpleRulerActor2D *to = vtkALBASimpleRulerActor2D::New();
  to->Delete();
}
//------------------------------------------------------------------------------
void vtkALBASimpleRulerActor2DTest::TestSetColor()
//------------------------------------------------------------------------------
{
  vtkALBASimpleRulerActor2D *actor;
  actor = vtkALBASimpleRulerActor2D::New();
  actor->SetColor(0.5,0.5,0.5);

  RenderData(actor, "TestSetColor");
  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkALBASimpleRulerActor2DTest::TestSetLabelScaleVisibility()
//------------------------------------------------------------------------------
{
  vtkALBASimpleRulerActor2D *actor;
  actor = vtkALBASimpleRulerActor2D::New();

  actor->SetLabelScaleVisibility(false);
  RenderData(actor, "TestSetLabelScaleVisibility");

  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkALBASimpleRulerActor2DTest::TestSetLabelAxesVisibility()
//------------------------------------------------------------------------------
{
  vtkALBASimpleRulerActor2D *actor;
  actor = vtkALBASimpleRulerActor2D::New();

  actor->SetLabelAxesVisibility(false);
  RenderData(actor, "TestSetLabelAxesVisibility");

  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkALBASimpleRulerActor2DTest::TestSetAxesVisibility()
//------------------------------------------------------------------------------
{
  vtkALBASimpleRulerActor2D *actor;
  actor = vtkALBASimpleRulerActor2D::New();

  actor->SetAxesVisibility(false);
  RenderData(actor, "TestSetAxesVisibility");

  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkALBASimpleRulerActor2DTest::TestSetTickVisibility()
//------------------------------------------------------------------------------
{
  vtkALBASimpleRulerActor2D *actor;
  actor = vtkALBASimpleRulerActor2D::New();

  actor->SetTickVisibility(false);
  RenderData(actor, "TestSetTickVisibility");

  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkALBASimpleRulerActor2DTest::TestSetLegend()
//------------------------------------------------------------------------------
{
  vtkALBASimpleRulerActor2D *actor;
  actor = vtkALBASimpleRulerActor2D::New();

  actor->SetLegend("Test Legend");
  RenderData(actor, "TestSetLegend");

  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkALBASimpleRulerActor2DTest::TestSetGetScaleFactor()
//------------------------------------------------------------------------------
{
  vtkALBASimpleRulerActor2D *actor;
  actor = vtkALBASimpleRulerActor2D::New();

  actor->SetScaleFactor(0.5);
  RenderData(actor, "TestSetGetScaleFactor");

  CPPUNIT_ASSERT(actor->GetScaleFactor() == 0.5);
  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkALBASimpleRulerActor2DTest::TestUseGlobalAxes()
//------------------------------------------------------------------------------
{
  vtkALBASimpleRulerActor2D *actor;
  actor = vtkALBASimpleRulerActor2D::New();
  actor->UseGlobalAxes(true);

  RenderData(actor, "TestUseGlobalAxes");

  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkALBASimpleRulerActor2DTest::TestSetInverseTicks()
//------------------------------------------------------------------------------
{
  vtkALBASimpleRulerActor2D *actor;
  actor = vtkALBASimpleRulerActor2D::New();
  actor->SetInverseTicks(true);

  RenderData(actor, "TestSetInverseTicks");

  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkALBASimpleRulerActor2DTest::TestSetAttachPosition()
//------------------------------------------------------------------------------
{
  vtkALBASimpleRulerActor2D *actor;
  actor = vtkALBASimpleRulerActor2D::New();
  actor->SetAttachPositionFlag(true);
  double position[3] = {1,2,0};
  actor->SetAttachPosition(position);
  RenderData(actor, "TestSetAttachPosition");

  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkALBASimpleRulerActor2DTest::TestCenterAxesOnScreen()
//------------------------------------------------------------------------------
{
  vtkALBASimpleRulerActor2D *actor;
  actor = vtkALBASimpleRulerActor2D::New();
  actor->CenterAxesOnScreen(false);
  RenderData(actor, "TestCenterAxesOnScreen");

  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkALBASimpleRulerActor2DTest::TestChangeRulerMarginsAndLengths()
//------------------------------------------------------------------------------
{
  vtkALBASimpleRulerActor2D *actor;
  actor = vtkALBASimpleRulerActor2D::New();
  actor->ChangeRulerMarginsAndLengths(2,3,5,7,2,3);

  RenderData(actor, "TestChangeRulerMarginsAndLengths");

  actor->Delete();
}

//------------------------------------------------------------------------------
void vtkALBASimpleRulerActor2DTest::TestFixedTick()
{
	vtkALBASimpleRulerActor2D *actor;
	actor = vtkALBASimpleRulerActor2D::New();
	actor->ShowFixedTick(true);

	RenderData(actor, "TestFixedTick");

	actor->Delete();
}

//------------------------------------------------------------------------------
void vtkALBASimpleRulerActor2DTest::TestPrintSelf()
//------------------------------------------------------------------------------
{
  vtkALBASimpleRulerActor2D *actor;
  actor = vtkALBASimpleRulerActor2D::New();
  actor->PrintSelf(std::cout, vtkIndent(3));
  actor->Delete();
}