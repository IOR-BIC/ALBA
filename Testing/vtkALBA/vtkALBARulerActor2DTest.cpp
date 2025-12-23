/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBARulerActor2DTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
#include <cppunit/config/SourcePrefix.h>
#include "vtkALBARulerActor2D.h"
#include "vtkALBARulerActor2DTest.h"

#include "vtkRenderWindowInteractor.h"
#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkALBASmartPointer.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkPointData.h"


//----------------------------------------------------------------------------
void vtkALBARulerActor2DTest::BeforeTest()
//----------------------------------------------------------------------------
{
	InitializeRenderWindow();
	
	vtkCamera *camera = m_Renderer->GetActiveCamera();
	camera->ParallelProjectionOn();
	camera->Modified();
}
//----------------------------------------------------------------------------
void vtkALBARulerActor2DTest::AfterTest()
//----------------------------------------------------------------------------
{
}

//------------------------------------------------------------
void vtkALBARulerActor2DTest::RenderData(vtkActor2D *actor, char* testName)
{
	m_Renderer->AddActor2D(actor);

	m_Renderer->ResetCamera();
	m_RenderWindow->Render();
	COMPARE_IMAGES(testName);
}
//------------------------------------------------------------
void vtkALBARulerActor2DTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkALBARulerActor2D *to = vtkALBARulerActor2D::New();
  to->Delete();
}
//--------------------------------------------
void vtkALBARulerActor2DTest::TestSetColor()
//--------------------------------------------
{
  vtkALBARulerActor2D *actor;
  actor = vtkALBARulerActor2D::New();
  actor->SetColor(0.5,0.5,0.5);

  RenderData(actor, "TestSetColor");
  actor->Delete();
}
//--------------------------------------------
void vtkALBARulerActor2DTest::TestSetLabelScaleVisibility()
//--------------------------------------------
{
  vtkALBARulerActor2D *actor;
  actor = vtkALBARulerActor2D::New();

  actor->SetLabelScaleVisibility(false);
  RenderData(actor, "TestSetLabelScaleVisibility");

  actor->Delete();
}
//--------------------------------------------
void vtkALBARulerActor2DTest::TestSetLabelAxesVisibility()
//--------------------------------------------
{
  vtkALBARulerActor2D *actor;
  actor = vtkALBARulerActor2D::New();

  actor->SetLabelAxesVisibility(false);
  RenderData(actor, "TestSetLabelAxesVisibility");

  actor->Delete();
}
//--------------------------------------------
void vtkALBARulerActor2DTest::TestSetAxesVisibility()
//--------------------------------------------
{
  vtkALBARulerActor2D *actor;
  actor = vtkALBARulerActor2D::New();

  actor->SetAxesVisibility(false);
  RenderData(actor, "TestSetAxesVisibility");

  actor->Delete();
}
//--------------------------------------------
void vtkALBARulerActor2DTest::TestSetTickVisibility()
//--------------------------------------------
{
  vtkALBARulerActor2D *actor;
  actor = vtkALBARulerActor2D::New();

  actor->SetTickVisibility(false);
  RenderData(actor, "TestSetTickVisibility");

  actor->Delete();
}
//--------------------------------------------
void vtkALBARulerActor2DTest::TestSetLegend()
//--------------------------------------------
{
  vtkALBARulerActor2D *actor;
  actor = vtkALBARulerActor2D::New();

  actor->SetLegend("Test Legend");
  RenderData(actor, "TestSetLegend");

  actor->Delete();
}
//--------------------------------------------
void vtkALBARulerActor2DTest::TestSetGetScaleFactor()
//--------------------------------------------
{
  vtkALBARulerActor2D *actor;
  actor = vtkALBARulerActor2D::New();

  actor->SetScaleFactor(0.5);
  RenderData(actor, "TestSetGetScaleFactor");

  CPPUNIT_ASSERT(actor->GetScaleFactor() == 0.5);
  actor->Delete();
}
//--------------------------------------------
void vtkALBARulerActor2DTest::TestUseGlobalAxes()
//--------------------------------------------
{
  vtkALBARulerActor2D *actor;
  actor = vtkALBARulerActor2D::New();
  actor->UseGlobalAxes(true);

  RenderData(actor, "TestUseGlobalAxes");

  actor->Delete();
}
//--------------------------------------------
void vtkALBARulerActor2DTest::TestSetInverseTicks()
//--------------------------------------------
{
  vtkALBARulerActor2D *actor;
  actor = vtkALBARulerActor2D::New();
  actor->SetInverseTicks(true);

  RenderData(actor, "TestSetInverseTicks");

  actor->Delete();
}
//--------------------------------------------
void vtkALBARulerActor2DTest::TestSetAttachPosition()
//--------------------------------------------
{
  vtkALBARulerActor2D *actor;
  actor = vtkALBARulerActor2D::New();
  actor->SetAttachPositionFlag(true);
  double position[3] = {1,2,0};
  actor->SetAttachPosition(position);

  RenderData(actor, "TestSetAttachPosition");

  actor->Delete();
}
//--------------------------------------------
void vtkALBARulerActor2DTest::TestChangeRulerMarginsAndLengths()
//--------------------------------------------
{
  vtkALBARulerActor2D *actor;
  actor = vtkALBARulerActor2D::New();
  actor->ChangeRulerMarginsAndLengths(2,3,5,7,2,3);

  RenderData(actor, "TestChangeRulerMarginsAndLengths");

  actor->Delete();
}
//--------------------------------------------
void vtkALBARulerActor2DTest::TestSetText()
//--------------------------------------------
{
  vtkALBARulerActor2D *actor;
  actor = vtkALBARulerActor2D::New();
  const char *label[3]={"Label 1","Label 2","Label 3"};
  actor->SetText(label);

  RenderData(actor, "TestSetText");

  actor->Delete();
}

//----------------------------------------------------------------------------
void vtkALBARulerActor2DTest::TestPrintSelf()
//----------------------------------------------------------------------------
{
  vtkALBARulerActor2D *actor;
  actor = vtkALBARulerActor2D::New();
  actor->PrintSelf(std::cout, 3);
  actor->Delete();
}
