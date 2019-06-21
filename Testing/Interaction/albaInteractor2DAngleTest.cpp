/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractor2DAngleTest
 Authors: Roberto Mucci
 
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
#include "albaInteractor2DAngleTest.h"
#include "albaInteractionTests.h"

#include "albaInteractor2DAngle.h"
#include "albaGUIFrame.h"

#include "vtkLineSource.h"

#include "wx/module.h"

#define EPSILON 0.001

//-----------------------------------------------------------
void albaInteractor2DAngleTest::BeforeTest()
//-----------------------------------------------------------
{
  m_Win = new albaGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  wxModule::RegisterModules();
  wxModule::InitializeModules();
}
//-----------------------------------------------------------
void albaInteractor2DAngleTest::AfterTest()
//-----------------------------------------------------------
{
  wxModule::CleanUpModules();
  delete m_Win;
}
//-----------------------------------------------------------
void albaInteractor2DAngleTest::TestFixture()
//-----------------------------------------------------------
{

}
//-----------------------------------------------------------
void albaInteractor2DAngleTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaInteractor2DAngle *interactor;
	albaNEW(interactor); 

	albaDEL(interactor);
}
//-----------------------------------------------------------
void albaInteractor2DAngleTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void albaInteractor2DAngleTest::TestSetMeasureType() 
//-----------------------------------------------------------
{
  albaInteractor2DAngle *interactor;
	albaNEW(interactor);
	interactor->m_TestMode = true;

  interactor->SetMeasureType(albaInteractor2DAngle::ANGLE_BETWEEN_POINTS);
	CPPUNIT_ASSERT( interactor->m_MeasureType == albaInteractor2DAngle::ANGLE_BETWEEN_POINTS );

  interactor->SetMeasureTypeToAngleBetweenLines();
  CPPUNIT_ASSERT( interactor->m_MeasureType == albaInteractor2DAngle::ANGLE_BETWEEN_LINES );

  interactor->SetMeasureTypeToAngleBetweenPoints();
  CPPUNIT_ASSERT( interactor->m_MeasureType == albaInteractor2DAngle::ANGLE_BETWEEN_POINTS );

	albaDEL(interactor);
}

//-----------------------------------------------------------
void albaInteractor2DAngleTest::TestSetManualDistance() 
//-----------------------------------------------------------
{
	albaInteractor2DAngle *interactor;
	albaNEW(interactor);
	interactor->m_TestMode = true;
	
	interactor->SetMeasureTypeToAngleBetweenPoints();

  //////////////////////////////////////////////////////////////////////////
  vtkLineSource *line1 = vtkLineSource::New();
  line1->SetPoint1(0.0,0.0,0.0);
  line1->SetPoint2(1.0,2.0,0.0);
  interactor->m_LineSourceVector1.push_back(line1);
  vtkLineSource *line2 = vtkLineSource::New();
  line2->SetPoint1(0.0,3.0,0.0);
  line2->SetPoint2(1.0,5.0,0.0);
  interactor->m_LineSourceVector2.push_back(line2);
  interactor->m_Measure.push_back(sqrt(3.0));

  interactor->SetManualAngle(3*sqrt(3.0));

  CPPUNIT_ASSERT(interactor->m_Measure[interactor->m_Measure.size()-1] == 3*sqrt(3.0));
  double pt1[3];
  interactor->m_LineSourceVector2[interactor->m_LineSourceVector2.size()-1]->GetPoint1(pt1);
  CPPUNIT_ASSERT(pt1[0]+EPSILON > 0.0 && pt1[0]-EPSILON<0.0);
  CPPUNIT_ASSERT(pt1[1]+EPSILON > 3.0 && pt1[1]-EPSILON<3.0);
  line1->Delete();
  line2->Delete();
  //////////////////////////////////////////////////////////////////////////

	albaDEL(interactor);
}
//-----------------------------------------------------------
void albaInteractor2DAngleTest::TestIsDisableUndoAndOkCancel() 
//-----------------------------------------------------------
{
	albaInteractor2DAngle *interactor;
	albaNEW(interactor);
	interactor->m_TestMode = true;

  CPPUNIT_ASSERT( interactor->IsDisableUndoAndOkCancel() == false );//Default value

	albaDEL(interactor);
}
//-----------------------------------------------------------
void albaInteractor2DAngleTest::TestGetLastAngle() 
//-----------------------------------------------------------
{
	albaInteractor2DAngle *interactor;
	albaNEW(interactor);
	interactor->m_TestMode = true;

  CPPUNIT_ASSERT( interactor->GetLastAngle() == 0 );

  interactor->m_Measure.push_back(3.0);
  interactor->m_Measure.push_back(5.0);

  CPPUNIT_ASSERT( interactor->GetLastAngle() == 5.0 );

	albaDEL(interactor);
}