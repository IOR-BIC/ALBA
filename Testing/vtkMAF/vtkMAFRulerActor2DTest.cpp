/*=========================================================================

 Program: MAF2
 Module: vtkMAFRulerActor2DTest
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
#include "vtkMAFRulerActor2D.h"
#include "vtkMAFRulerActor2DTest.h"

#include "vtkRenderWindowInteractor.h"
#include "vtkMAFSmartPointer.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkPointData.h"


//----------------------------------------------------------------------------
void vtkMAFRulerActor2DTest::BeforeTest()
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
void vtkMAFRulerActor2DTest::AfterTest()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);
}

//------------------------------------------------------------
void vtkMAFRulerActor2DTest::RenderData(vtkActor2D *actor, char* testName)
{
  vtkMAFSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(m_RenderWindow);

	m_Renderer->AddActor2D(actor);
	m_RenderWindow->Render();

	COMPARE_IMAGES(testName);
}
//------------------------------------------------------------
void vtkMAFRulerActor2DTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkMAFRulerActor2D *to = vtkMAFRulerActor2D::New();
  to->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetColor()
//--------------------------------------------
{
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();
  actor->SetColor(0.5,0.5,0.5);

  RenderData(actor, "TestSetColor");
  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetLabelScaleVisibility()
//--------------------------------------------
{
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();

  actor->SetLabelScaleVisibility(false);
  RenderData(actor, "TestSetLabelScaleVisibility");

  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetLabelAxesVisibility()
//--------------------------------------------
{
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();

  actor->SetLabelAxesVisibility(false);
  RenderData(actor, "TestSetLabelAxesVisibility");

  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetAxesVisibility()
//--------------------------------------------
{
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();

  actor->SetAxesVisibility(false);
  RenderData(actor, "TestSetAxesVisibility");

  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetTickVisibility()
//--------------------------------------------
{
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();

  actor->SetTickVisibility(false);
  RenderData(actor, "TestSetTickVisibility");

  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetLegend()
//--------------------------------------------
{
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();

  actor->SetLegend("Test Legend");
  RenderData(actor, "TestSetLegend");

  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetGetScaleFactor()
//--------------------------------------------
{
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();

  actor->SetScaleFactor(0.5);
  RenderData(actor, "TestSetGetScaleFactor");

  CPPUNIT_ASSERT(actor->GetScaleFactor() == 0.5);
  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestUseGlobalAxes()
//--------------------------------------------
{
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();
  actor->UseGlobalAxes(true);

  RenderData(actor, "TestUseGlobalAxes");

  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetInverseTicks()
//--------------------------------------------
{
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();
  actor->SetInverseTicks(true);

  RenderData(actor, "TestSetInverseTicks");

  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetAttachPosition()
//--------------------------------------------
{
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();
  actor->SetAttachPositionFlag(true);
  double position[3] = {1,2,0};
  actor->SetAttachPosition(position);

  RenderData(actor, "TestSetAttachPosition");

  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestChangeRulerMarginsAndLengths()
//--------------------------------------------
{
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();
  actor->ChangeRulerMarginsAndLengths(2,3,5,7,2,3);

  RenderData(actor, "TestChangeRulerMarginsAndLengths");

  actor->Delete();
}
//--------------------------------------------
void vtkMAFRulerActor2DTest::TestSetText()
//--------------------------------------------
{
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();
  const char *label[3]={"Label 1","Label 2","Label 3"};
  actor->SetText(label);

  RenderData(actor, "TestSetText");

  actor->Delete();
}

//----------------------------------------------------------------------------
void vtkMAFRulerActor2DTest::TestPrintSelf()
//----------------------------------------------------------------------------
{
  vtkMAFRulerActor2D *actor;
  actor = vtkMAFRulerActor2D::New();
  actor->PrintSelf(std::cout, 3);
  actor->Delete();
}
