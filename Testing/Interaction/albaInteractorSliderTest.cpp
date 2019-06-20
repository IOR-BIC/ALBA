/*=========================================================================
Program:   ALBA
Module:    albaInteractorSliderTest.cpp
Language:  C++
Date:      $Date: 2019-04-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "albaInteractorSliderTest.h"
#include "albaInteractionTests.h"

#include "albaInteractorSlider.h"
#include "albaGUIFrame.h"

#include "vtkLineSource.h"

#include "wx/module.h"

#define EPSILON 0.001

//-----------------------------------------------------------
void albaInteractorSliderTest::BeforeTest()
{
  m_Win = new albaGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  wxModule::RegisterModules();
  wxModule::InitializeModules();
}
//-----------------------------------------------------------
void albaInteractorSliderTest::AfterTest()
{
  wxModule::CleanUpModules();
  delete m_Win;
}
//-----------------------------------------------------------
void albaInteractorSliderTest::TestFixture()
{

}
//-----------------------------------------------------------
void albaInteractorSliderTest::TestDynamicAllocation() 
{
//   albaInteractorSlider *interactor;
// 	albaNEW(interactor); 
// 
// 	albaDEL(interactor);
}
//-----------------------------------------------------------
void albaInteractorSliderTest::TestStaticAllocation() 
{
}
//-----------------------------------------------------------
void albaInteractorSliderTest::TestSetRange() 
{
// 	albaInteractorSlider *interactor;
// 	albaNEW(interactor);
// 	interactor->m_TestMode = true;
// 
// 	interactor->SetRange(-1.0, 1.0);
// 
// 	albaDEL(interactor);
}

//-----------------------------------------------------------
void albaInteractorSliderTest::TestSetSteps() 
{
// 	albaInteractorSlider *interactor;
// 	albaNEW(interactor);
// 	interactor->m_TestMode = true;
// 	
// 	double min = -1.0;
// 	double max = 1.0;
// 
// 	interactor->SetValue(0.0);
// 
// 	interactor->SetRange(min, max);
// 	interactor->SetSteps(100);
	
  //////////////////////////////////////////////////////////////////////////
// 	double val = 0.0;
// 	CPPUNIT_ASSERT(interactor->GetValue() == val);

  //////////////////////////////////////////////////////////////////////////

/*	albaDEL(interactor);*/
}