/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMML3ParameterViewTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>

#include "albaString.h"

#include "albaOpMML3ParameterViewTest.h"
#include "albaOpMML3ParameterView.h"

//----------------------------------------------------------------------------
void albaOpMML3ParameterViewTest::BeforeTest()
//----------------------------------------------------------------------------
{
	InitializeRenderWindow();
	m_RenderWindow->RemoveRenderer(m_Renderer);
}
//----------------------------------------------------------------------------
void albaOpMML3ParameterViewTest::AfterTest()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaOpMML3ParameterViewTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaOpMML3ParameterView *view = new albaOpMML3ParameterView(m_RenderWindow, m_Renderer);
  CPPUNIT_ASSERT(NULL != view);
  cppDEL(view);
}

//----------------------------------------------------------------------------
void albaOpMML3ParameterViewTest::TestGetValue()
//----------------------------------------------------------------------------
{
  albaOpMML3ParameterView *view = new albaOpMML3ParameterView(m_RenderWindow, m_Renderer);
  float val = view->GetValue(0);
  CPPUNIT_ASSERT(0 == val);
  cppDEL(view);
}

//----------------------------------------------------------------------------
void albaOpMML3ParameterViewTest::TestGetNumberOfDataPoints()
//----------------------------------------------------------------------------
{
  albaOpMML3ParameterView *view = new albaOpMML3ParameterView(m_RenderWindow, m_Renderer);
  CPPUNIT_ASSERT(0 == view->GetNumberOfDataPoints());
  cppDEL(view);
}

//----------------------------------------------------------------------------
void albaOpMML3ParameterViewTest::TestGetMaxY()
//----------------------------------------------------------------------------
{
  albaOpMML3ParameterView *view = new albaOpMML3ParameterView(m_RenderWindow, m_Renderer);
  view->SetRangeY(0,5,10);

  CPPUNIT_ASSERT(10 == view->GetMaxY());
  cppDEL(view);
}

//----------------------------------------------------------------------------
void albaOpMML3ParameterViewTest::TestGetMinY()
//----------------------------------------------------------------------------
{
  albaOpMML3ParameterView *view = new albaOpMML3ParameterView(m_RenderWindow, m_Renderer);
  view->SetRangeY(0,5,10);

  CPPUNIT_ASSERT(0 == view->GetMinY());
  cppDEL(view);
}

//----------------------------------------------------------------------------
void albaOpMML3ParameterViewTest::TestSetLineActorX()
//----------------------------------------------------------------------------
{
  albaOpMML3ParameterView *view = new albaOpMML3ParameterView(m_RenderWindow, m_Renderer);
  view->SetRangeX(10);
  view->SetRangeY(0,5,10);

  view->AddPoint(1,1);
  view->AddPoint(2,2);

  view->SetLineActorX(1);

  // test with render
  m_Renderer->ResetCamera();
  view->Render();

	COMPARE_IMAGES("TestSetLineActorX");
  cppDEL(view);
}

//----------------------------------------------------------------------------
void albaOpMML3ParameterViewTest::TestGetPointsActor()
//----------------------------------------------------------------------------
{
  albaOpMML3ParameterView *view = new albaOpMML3ParameterView(m_RenderWindow, m_Renderer);
  view->SetRangeX(10);
  view->SetRangeY(0,5,10);

  view->AddPoint(1,1);
  view->AddPoint(2,2);

  CPPUNIT_ASSERT(NULL != view->GetPointsActor());

  cppDEL(view);
}

//----------------------------------------------------------------------------
void albaOpMML3ParameterViewTest::TestGetSplineActor()
//----------------------------------------------------------------------------
{
  albaOpMML3ParameterView *view = new albaOpMML3ParameterView(m_RenderWindow, m_Renderer);
  view->SetRangeX(10);
  view->SetRangeY(0,5,10);

  view->AddPoint(1,1);
  view->AddPoint(2,2);

  CPPUNIT_ASSERT(NULL != view->GetSplineActor());

  cppDEL(view);
}

//----------------------------------------------------------------------------
void albaOpMML3ParameterViewTest::TestRemovePoint()
//----------------------------------------------------------------------------
{
  albaOpMML3ParameterView *view = new albaOpMML3ParameterView(m_RenderWindow, m_Renderer);

  view->SetRangeX(10);
  view->SetRangeY(0,5,10);

  view->AddPoint(1,1);
  view->AddPoint(2,2);

  view->RemovePoint(1);
  view->RemovePoint(2);

  CPPUNIT_ASSERT(24 != view->GetValue(0));
  CPPUNIT_ASSERT(0 == view->GetValue(0));
  
  cppDEL(view);
}

//----------------------------------------------------------------------------
void albaOpMML3ParameterViewTest::TestAddPoint()
//----------------------------------------------------------------------------
{
  albaOpMML3ParameterView *view = new albaOpMML3ParameterView(m_RenderWindow, m_Renderer);

  view->SetRangeX(10);
  view->SetRangeY(0,5,10);

  view->AddPoint(1,1);
  view->AddPoint(2,2);
  CPPUNIT_ASSERT(60 == round(view->GetValue(0)));
  cppDEL(view);
}

//----------------------------------------------------------------------------
void albaOpMML3ParameterViewTest::TestRender()
//----------------------------------------------------------------------------
{
  albaOpMML3ParameterView *view = new albaOpMML3ParameterView(m_RenderWindow, m_Renderer);

  view->SetRangeX(10);
  view->SetRangeY(0,5,10);

  view->AddPoint(1,1);
  view->AddPoint(2,2);
  
  m_Renderer->ResetCamera();
  view->Render();

	COMPARE_IMAGES("TestRender");

  cppDEL(view);
}