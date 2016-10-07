/*=========================================================================

 Program: MAF2
 Module: vtkMAFSimpleRulerActor2DTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
#include <cppunit/config/SourcePrefix.h>
#include "vtkMAFSimpleRulerActor2D.h"
#include "vtkMAFSimpleRulerActor2DTest.h"

#include "vtkRenderWindowInteractor.h"
#include "vtkMAFSmartPointer.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkPointData.h"

//----------------------------------------------------------------------------
void vtkMAFSimpleRulerActor2DTest::BeforeTest()
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
void vtkMAFSimpleRulerActor2DTest::AfterTest()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);
}

//------------------------------------------------------------------------------
void vtkMAFSimpleRulerActor2DTest::RenderData(vtkActor2D *actor, char* testName)
{
  vtkMAFSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(m_RenderWindow);

	m_Renderer->AddActor2D(actor);
	m_RenderWindow->Render();

	COMPARE_IMAGES(testName);
}
//------------------------------------------------------------------------------
void vtkMAFSimpleRulerActor2DTest::TestDynamicAllocation()
//------------------------------------------------------------------------------
{
  vtkMAFSimpleRulerActor2D *to = vtkMAFSimpleRulerActor2D::New();
  to->Delete();
}
//------------------------------------------------------------------------------
void vtkMAFSimpleRulerActor2DTest::TestSetColor()
//------------------------------------------------------------------------------
{
  vtkMAFSimpleRulerActor2D *actor;
  actor = vtkMAFSimpleRulerActor2D::New();
  actor->SetColor(0.5,0.5,0.5);

  RenderData(actor, "TestSetColor");
  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkMAFSimpleRulerActor2DTest::TestSetLabelScaleVisibility()
//------------------------------------------------------------------------------
{
  vtkMAFSimpleRulerActor2D *actor;
  actor = vtkMAFSimpleRulerActor2D::New();

  actor->SetLabelScaleVisibility(false);
  RenderData(actor, "TestSetLabelScaleVisibility");

  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkMAFSimpleRulerActor2DTest::TestSetLabelAxesVisibility()
//------------------------------------------------------------------------------
{
  vtkMAFSimpleRulerActor2D *actor;
  actor = vtkMAFSimpleRulerActor2D::New();

  actor->SetLabelAxesVisibility(false);
  RenderData(actor, "TestSetLabelAxesVisibility");

  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkMAFSimpleRulerActor2DTest::TestSetAxesVisibility()
//------------------------------------------------------------------------------
{
  vtkMAFSimpleRulerActor2D *actor;
  actor = vtkMAFSimpleRulerActor2D::New();

  actor->SetAxesVisibility(false);
  RenderData(actor, "TestSetAxesVisibility");

  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkMAFSimpleRulerActor2DTest::TestSetTickVisibility()
//------------------------------------------------------------------------------
{
  vtkMAFSimpleRulerActor2D *actor;
  actor = vtkMAFSimpleRulerActor2D::New();

  actor->SetTickVisibility(false);
  RenderData(actor, "TestSetTickVisibility");

  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkMAFSimpleRulerActor2DTest::TestSetLegend()
//------------------------------------------------------------------------------
{
  vtkMAFSimpleRulerActor2D *actor;
  actor = vtkMAFSimpleRulerActor2D::New();

  actor->SetLegend("Test Legend");
  RenderData(actor, "TestSetLegend");

  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkMAFSimpleRulerActor2DTest::TestSetGetScaleFactor()
//------------------------------------------------------------------------------
{
  vtkMAFSimpleRulerActor2D *actor;
  actor = vtkMAFSimpleRulerActor2D::New();

  actor->SetScaleFactor(0.5);
  RenderData(actor, "TestSetGetScaleFactor");

  CPPUNIT_ASSERT(actor->GetScaleFactor() == 0.5);
  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkMAFSimpleRulerActor2DTest::TestUseGlobalAxes()
//------------------------------------------------------------------------------
{
  vtkMAFSimpleRulerActor2D *actor;
  actor = vtkMAFSimpleRulerActor2D::New();
  actor->UseGlobalAxes(true);

  RenderData(actor, "TestUseGlobalAxes");

  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkMAFSimpleRulerActor2DTest::TestSetInverseTicks()
//------------------------------------------------------------------------------
{
  vtkMAFSimpleRulerActor2D *actor;
  actor = vtkMAFSimpleRulerActor2D::New();
  actor->SetInverseTicks(true);

  RenderData(actor, "TestSetInverseTicks");

  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkMAFSimpleRulerActor2DTest::TestSetAttachPosition()
//------------------------------------------------------------------------------
{
  vtkMAFSimpleRulerActor2D *actor;
  actor = vtkMAFSimpleRulerActor2D::New();
  actor->SetAttachPositionFlag(true);
  double position[3] = {1,2,0};
  actor->SetAttachPosition(position);
  RenderData(actor, "TestSetAttachPosition");

  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkMAFSimpleRulerActor2DTest::TestCenterAxesOnScreen()
//------------------------------------------------------------------------------
{
  vtkMAFSimpleRulerActor2D *actor;
  actor = vtkMAFSimpleRulerActor2D::New();
  actor->CenterAxesOnScreen(false);
  RenderData(actor, "TestCenterAxesOnScreen");

  actor->Delete();
}
//------------------------------------------------------------------------------
void vtkMAFSimpleRulerActor2DTest::TestChangeRulerMarginsAndLengths()
//------------------------------------------------------------------------------
{
  vtkMAFSimpleRulerActor2D *actor;
  actor = vtkMAFSimpleRulerActor2D::New();
  actor->ChangeRulerMarginsAndLengths(2,3,5,7,2,3);

  RenderData(actor, "TestChangeRulerMarginsAndLengths");

  actor->Delete();
}

//------------------------------------------------------------------------------
void vtkMAFSimpleRulerActor2DTest::TestFixedTick()
{
	vtkMAFSimpleRulerActor2D *actor;
	actor = vtkMAFSimpleRulerActor2D::New();
	actor->ShowFixedTick(true);

	RenderData(actor, "TestFixedTick");

	actor->Delete();
}

//------------------------------------------------------------------------------
void vtkMAFSimpleRulerActor2DTest::TestPrintSelf()
//------------------------------------------------------------------------------
{
  vtkMAFSimpleRulerActor2D *actor;
  actor = vtkMAFSimpleRulerActor2D::New();
  actor->PrintSelf(std::cout, 3);
  actor->Delete();
}