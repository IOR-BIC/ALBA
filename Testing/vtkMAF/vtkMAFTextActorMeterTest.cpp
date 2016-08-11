/*=========================================================================

 Program: MAF2
 Module: vtkMAFTextActorMeterTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>
#include "mafDefines.h"
#include "vtkMAFTextActorMeter.h"
#include "vtkMAFTextActorMeterTest.h"

#include "vtkRenderWindowInteractor.h"
#include "vtkMAFSmartPointer.h"
#include "vtkActor2D.h"
#include "vtkCamera.h"
#include "vtkPointData.h"


void vtkMAFTextActorMeterTest::TestFixture()
{
}
//----------------------------------------------------------------------------
void vtkMAFTextActorMeterTest::BeforeTest()
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
void vtkMAFTextActorMeterTest::AfterTest()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);
}
//------------------------------------------------------------
void vtkMAFTextActorMeterTest::RenderData(vtkActor2D *actor, char* testName)
{
  vtkMAFSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(m_RenderWindow);

	m_Renderer->AddActor2D(actor);
	m_RenderWindow->Render();

	COMPARE_IMAGES(testName);
}
//------------------------------------------------------------------
void vtkMAFTextActorMeterTest::SetText(vtkMAFTextActorMeter *actor, const char *text, double position[3])
//------------------------------------------------------------------
{
  if(actor)
  {
    actor->SetVisibility(true);
    actor->SetText(text);
    actor->SetTextPosition(position);
  }
}
//------------------------------------------------------------
void vtkMAFTextActorMeterTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkMAFTextActorMeter *to = vtkMAFTextActorMeter::New();
  to->Delete();
}
//--------------------------------------------
void vtkMAFTextActorMeterTest::TestTextPos1()
//--------------------------------------------
{
  vtkMAFTextActorMeter *actor;
  actor = vtkMAFTextActorMeter::New();

  std::string text = "String Text Test 1";
  double position[3] = {0.1,0.1,0.};
  text += " pos->";
  text += ConvertDouble(position[0]);
  text += " ";
  text += ConvertDouble(position[1]);
  text += " ";
  text += ConvertDouble(position[2]);

  SetText(actor, text.c_str(), position);

  //test GetText
  CPPUNIT_ASSERT(strcmp(actor->GetText(),text.c_str())==0);

  RenderData(actor, "TestTextPos1");
  actor->Delete();
}
//--------------------------------------------
void vtkMAFTextActorMeterTest::TestTextPos2()
//--------------------------------------------
{
  vtkMAFTextActorMeter *actor;
  actor = vtkMAFTextActorMeter::New();

  std::string text = "String Text Test 2";
  double position[3] = {0.05,0.05,0.};
  text += " pos->";
  text += ConvertDouble(position[0]);
  text += " ";
  text += ConvertDouble(position[1]);
  text += " ";
  text += ConvertDouble(position[2]);

  SetText(actor, text.c_str(), position);

  //test GetText
  CPPUNIT_ASSERT(strcmp(actor->GetText(),text.c_str())==0);

  RenderData(actor, "TestTextPos2");
  actor->Delete();
}
//--------------------------------------------
void vtkMAFTextActorMeterTest::TestTextColor()
//--------------------------------------------
{
  vtkMAFTextActorMeter *actor;
  actor = vtkMAFTextActorMeter::New();

  std::string text = "String Text Test Color";
  double position[3] = {0.05,0.05,0.};
  text += " pos->";
  text += ConvertDouble(position[0]);
  text += " ";
  text += ConvertDouble(position[1]);
  text += " ";
  text += ConvertDouble(position[2]);

  SetText(actor, text.c_str(), position);
  actor->SetColor(1.0, 0.0, 0.0);

  RenderData(actor, "TestTextColor");
  actor->Delete();
}

//----------------------------------------------------------------------------
void vtkMAFTextActorMeterTest::TestPrintSelf()
//----------------------------------------------------------------------------
{
  vtkMAFTextActorMeter *actor;
  actor = vtkMAFTextActorMeter::New();
  actor->PrintSelf(std::cout, 3);
  actor->Delete();
}

//--------------------------------------------------
std::string vtkMAFTextActorMeterTest::ConvertDouble(double number)
//--------------------------------------------------
{
  std::ostringstream strs;
  strs << number;
  return strs.str();
}
