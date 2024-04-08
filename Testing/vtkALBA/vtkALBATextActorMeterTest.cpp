/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBATextActorMeterTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <cppunit/config/SourcePrefix.h>
#include "albaDefines.h"
#include "vtkALBATextActorMeter.h"
#include "vtkALBATextActorMeterTest.h"

#include "vtkRenderWindowInteractor.h"
#include "vtkALBASmartPointer.h"
#include "vtkActor2D.h"
#include "vtkCamera.h"
#include "vtkPointData.h"


void vtkALBATextActorMeterTest::TestFixture()
{
}
//----------------------------------------------------------------------------
void vtkALBATextActorMeterTest::BeforeTest()
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
void vtkALBATextActorMeterTest::AfterTest()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);
}
//------------------------------------------------------------
void vtkALBATextActorMeterTest::RenderData(vtkActor2D *actor, char* testName)
{
  vtkALBASmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(m_RenderWindow);

	m_Renderer->AddActor2D(actor);
	m_RenderWindow->Render();

	COMPARE_IMAGES(testName);
}
//------------------------------------------------------------------
void vtkALBATextActorMeterTest::SetText(vtkALBATextActorMeter *actor, const char *text, double position[3])
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
void vtkALBATextActorMeterTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkALBATextActorMeter *to = vtkALBATextActorMeter::New();
  to->Delete();
}
//--------------------------------------------
void vtkALBATextActorMeterTest::TestTextPos1()
//--------------------------------------------
{
  vtkALBATextActorMeter *actor;
  actor = vtkALBATextActorMeter::New();

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
void vtkALBATextActorMeterTest::TestTextPos2()
//--------------------------------------------
{
  vtkALBATextActorMeter *actor;
  actor = vtkALBATextActorMeter::New();

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
void vtkALBATextActorMeterTest::TestTextColor()
//--------------------------------------------
{
  vtkALBATextActorMeter *actor;
  actor = vtkALBATextActorMeter::New();

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
void vtkALBATextActorMeterTest::TestPrintSelf()
//----------------------------------------------------------------------------
{
  vtkALBATextActorMeter *actor;
  actor = vtkALBATextActorMeter::New();
  actor->PrintSelf(std::cout, vtkIndent(3));
  actor->Delete();
}

//--------------------------------------------------
std::string vtkALBATextActorMeterTest::ConvertDouble(double number)
//--------------------------------------------------
{
  std::ostringstream strs;
  strs << number;
  return strs.str();
}
