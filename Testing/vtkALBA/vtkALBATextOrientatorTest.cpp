/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBATextOrientatorTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include "vtkALBATextOrientator.h"
#include "vtkALBATextOrientatorTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "vtkRenderWindowInteractor.h"
#include "vtkPropCollection.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include "vtkALBASmartPointer.h"
#include "vtkActor2D.h"
#include "vtkPointData.h"

void vtkALBATextOrientatorTest::TestFixture()
{
}
//----------------------------------------------------------------------------
void vtkALBATextOrientatorTest::BeforeTest()
//----------------------------------------------------------------------------
{
  InitializeRenderWindow();
}
//----------------------------------------------------------------------------
void vtkALBATextOrientatorTest::AfterTest()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Renderer);
	vtkDEL(m_RenderWindow);
}

//------------------------------------------------------------
void vtkALBATextOrientatorTest::RenderData(vtkActor2D *actor, char* testName)
{
 	m_Renderer->AddActor2D(actor);
	
	m_Renderer->ResetCamera();
	m_RenderWindow->Render();
  COMPARE_IMAGES(testName);
}
//------------------------------------------------------------------
void vtkALBATextOrientatorTest::SetText(vtkALBATextOrientator *actor)
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
void vtkALBATextOrientatorTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkALBATextOrientator *to = vtkALBATextOrientator::New();
  to->Delete();
}
//--------------------------------------------
void vtkALBATextOrientatorTest::TestText()
//--------------------------------------------
{
  vtkALBATextOrientator *actor;
  actor = vtkALBATextOrientator::New();

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
void vtkALBATextOrientatorTest::TestSingleActorVisibility()
//--------------------------------------------
{
  vtkALBATextOrientator *actor;
  actor = vtkALBATextOrientator::New();

  SetText(actor);
  actor->SetSingleActorVisibility(vtkALBATextOrientator::ID_ACTOR_LEFT, false); //left label is not visible

  RenderData(actor, "TestSingleActorVisibility");
  actor->Delete();
}
//--------------------------------------------
void vtkALBATextOrientatorTest::TestTextColor()
//--------------------------------------------
{
  vtkALBATextOrientator *actor;
  actor = vtkALBATextOrientator::New();

  SetText(actor);
  actor->SetTextColor(1.0, 0.0, 0.0);

  RenderData(actor, "TestTextColor");
  actor->Delete();
}
//--------------------------------------------
void vtkALBATextOrientatorTest::TestBackgroundColor()
//--------------------------------------------
{
  vtkALBATextOrientator *actor;
  actor = vtkALBATextOrientator::New();

  SetText(actor);
  actor->SetBackgroundColor(1.0, 0.0, 0.0);

  RenderData(actor, "TestBackgroundColor");
  actor->Delete();
}
//--------------------------------------------
void vtkALBATextOrientatorTest::TestBackgroundVisibility()
//--------------------------------------------
{
  vtkALBATextOrientator *actor;
  actor = vtkALBATextOrientator::New();

  SetText(actor);
  actor->SetTextColor(0.0,0.0,0.0);
  actor->SetBackgroundVisibility(false);

  RenderData(actor, "TestBackgroundVisibility");
  actor->Delete();
}
//--------------------------------------------
void vtkALBATextOrientatorTest::TestScale()
//--------------------------------------------
{
  vtkALBATextOrientator *actor;
  actor = vtkALBATextOrientator::New();

  SetText(actor);
  actor->SetScale(2);

  RenderData(actor, "TestScale");
  actor->Delete();
}
