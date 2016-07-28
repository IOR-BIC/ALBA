/*=========================================================================

 Program: MAF2
 Module: vtkMAFTextOrientatorTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "mafDefines.h"

#include "vtkMAFTextOrientator.h"
#include "vtkMAFTextOrientatorTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "vtkRenderWindowInteractor.h"
#include "vtkMAFSmartPointer.h"
#include "vtkActor2D.h"
#include "vtkPointData.h"

void vtkMAFTextOrientatorTest::TestFixture()
{
}
//----------------------------------------------------------------------------
void vtkMAFTextOrientatorTest::BeforeTest()
//----------------------------------------------------------------------------
{
	vtkNEW(m_Renderer);
	vtkNEW(m_RenderWindow);

	m_RenderWindow->AddRenderer(m_Renderer);
	m_RenderWindow->SetSize(640, 480);
	m_RenderWindow->SetPosition(100, 0);

	m_Renderer->SetBackground(1.0, 1.0, 1.0);
}
//----------------------------------------------------------------------------
void vtkMAFTextOrientatorTest::AfterTest()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);
}

//------------------------------------------------------------
void vtkMAFTextOrientatorTest::RenderData(vtkActor2D *actor, char* testName)
{
  vtkMAFSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(m_RenderWindow);

	m_Renderer->AddActor2D(actor);
	m_RenderWindow->Render();

	COMPARE_IMAGES(testName);
}
//------------------------------------------------------------------
void vtkMAFTextOrientatorTest::SetText(vtkMAFTextOrientator *actor)
//------------------------------------------------------------------
{
  if(actor)
  {
    actor->SetVisibility(true);
    actor->SetTextLeft("L");
    actor->SetTextDown("D");
    actor->SetTextRight("R");
    actor->SetTextUp("U");
  }
}
//------------------------------------------------------------
void vtkMAFTextOrientatorTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkMAFTextOrientator *to = vtkMAFTextOrientator::New();
  to->Delete();
}
//--------------------------------------------
void vtkMAFTextOrientatorTest::TestText()
//--------------------------------------------
{
  vtkMAFTextOrientator *actor;
  actor = vtkMAFTextOrientator::New();

  SetText(actor);

  //test GetText
  CPPUNIT_ASSERT(strcmp(actor->GetTextLeft(),"L")==0);
  CPPUNIT_ASSERT(strcmp(actor->GetTextDown(),"D")==0);
  CPPUNIT_ASSERT(strcmp(actor->GetTextRight(),"R")==0);
  CPPUNIT_ASSERT(strcmp(actor->GetTextUp(),"U")==0);

  RenderData(actor, "TestText");
  actor->Delete();
}
//--------------------------------------------
void vtkMAFTextOrientatorTest::TestSingleActorVisibility()
//--------------------------------------------
{
  vtkMAFTextOrientator *actor;
  actor = vtkMAFTextOrientator::New();

  SetText(actor);
  actor->SetSingleActorVisibility(vtkMAFTextOrientator::ID_ACTOR_LEFT, false); //left label is not visible

  RenderData(actor, "TestSingleActorVisibility");
  actor->Delete();
}
//--------------------------------------------
void vtkMAFTextOrientatorTest::TestTextColor()
//--------------------------------------------
{
  vtkMAFTextOrientator *actor;
  actor = vtkMAFTextOrientator::New();

  SetText(actor);
  actor->SetTextColor(1.0, 0.0, 0.0);

  RenderData(actor, "TestTextColor");
  actor->Delete();
}
//--------------------------------------------
void vtkMAFTextOrientatorTest::TestBackgroundColor()
//--------------------------------------------
{
  vtkMAFTextOrientator *actor;
  actor = vtkMAFTextOrientator::New();

  SetText(actor);
  actor->SetBackgroundColor(1.0, 0.0, 0.0);

  RenderData(actor, "TestBackgroundColor");
  actor->Delete();
}
//--------------------------------------------
void vtkMAFTextOrientatorTest::TestBackgroundVisibility()
//--------------------------------------------
{
  vtkMAFTextOrientator *actor;
  actor = vtkMAFTextOrientator::New();

  SetText(actor);
  actor->SetTextColor(0.0,0.0,0.0);
  actor->SetBackgroundVisibility(false);

  RenderData(actor, "TestBackgroundVisibility");
  actor->Delete();
}
//--------------------------------------------
void vtkMAFTextOrientatorTest::TestScale()
//--------------------------------------------
{
  vtkMAFTextOrientator *actor;
  actor = vtkMAFTextOrientator::New();

  SetText(actor);
  actor->SetScale(2);

  RenderData(actor, "TestScale");
  actor->Delete();
}
