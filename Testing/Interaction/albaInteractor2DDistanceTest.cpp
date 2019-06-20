/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractor2DDistanceTest
 Authors: Matteo Giacomoni
 
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
#include "albaInteractor2DDistanceTest.h"
#include "albaInteractionTests.h"

#include "albaInteractor2DDistance.h"
#include "albaGUIFrame.h"

#include "vtkLineSource.h"

#include "wx/module.h"

#define EPSILON 0.001

//-----------------------------------------------------------
void albaInteractor2DDistanceTest::BeforeTest()
//-----------------------------------------------------------
{
  m_Win = new albaGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  wxModule::RegisterModules();
  wxModule::InitializeModules();
}
//-----------------------------------------------------------
void albaInteractor2DDistanceTest::AfterTest()
//-----------------------------------------------------------
{
  wxModule::CleanUpModules();
  delete m_Win;
}
//-----------------------------------------------------------
void albaInteractor2DDistanceTest::TestFixture()
//-----------------------------------------------------------
{

}
//-----------------------------------------------------------
void albaInteractor2DDistanceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
	albaInteractor2DDistance *interactor = albaInteractor2DDistance::NewTestInstance();



  albaDEL(interactor);
}
//-----------------------------------------------------------
void albaInteractor2DDistanceTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void albaInteractor2DDistanceTest::TestSetMeasureType() 
//-----------------------------------------------------------
{
	albaInteractor2DDistance *interactor = albaInteractor2DDistance::NewTestInstance();

  interactor->SetMeasureType(albaInteractor2DDistance::DISTANCE_BETWEEN_POINTS);
  CPPUNIT_ASSERT( interactor->m_MeasureType == albaInteractor2DDistance::DISTANCE_BETWEEN_POINTS );
  //////////////////////////////////////////////////////////////////////////
  interactor->SetMeasureTypeToDistanceBetweenLines();
  CPPUNIT_ASSERT( interactor->m_MeasureType == albaInteractor2DDistance::DISTANCE_BETWEEN_LINES );
  //////////////////////////////////////////////////////////////////////////
  interactor->SetMeasureTypeToDistanceBetweenPoints();
  CPPUNIT_ASSERT( interactor->m_MeasureType == albaInteractor2DDistance::DISTANCE_BETWEEN_POINTS );

  albaDEL(interactor);
}
//-----------------------------------------------------------
void albaInteractor2DDistanceTest::TestGenerateHistogram() 
//-----------------------------------------------------------
{
	albaInteractor2DDistance *interactor = albaInteractor2DDistance::NewTestInstance();

  interactor->GenerateHistogram(true);
  CPPUNIT_ASSERT( interactor->m_GenerateHistogram == true );
  //////////////////////////////////////////////////////////////////////////
  interactor->GenerateHistogram(false);
  CPPUNIT_ASSERT( interactor->m_GenerateHistogram == false );
  //////////////////////////////////////////////////////////////////////////
  interactor->GenerateHistogramOn();
  CPPUNIT_ASSERT( interactor->m_GenerateHistogram == true );
  //////////////////////////////////////////////////////////////////////////
  interactor->GenerateHistogramOff();
  CPPUNIT_ASSERT( interactor->m_GenerateHistogram == false );

  albaDEL(interactor);
}
//-----------------------------------------------------------
void albaInteractor2DDistanceTest::TestSetManualDistance1() 
//-----------------------------------------------------------
{
	albaInteractor2DDistance *interactor = albaInteractor2DDistance::NewTestInstance();

  interactor->SetMeasureTypeToDistanceBetweenPoints();

  //////////////////////////////////////////////////////////////////////////
  vtkLineSource *line1 = vtkLineSource::New();
  line1->SetPoint1(0.0,0.0,0.0);
  line1->SetPoint2(4.0,3.0,0.0);
  interactor->m_LineSourceVector1.push_back(line1);
  interactor->m_Measure.push_back(5.0);

  interactor->SetManualDistance(6.0);

  CPPUNIT_ASSERT(interactor->m_Measure[interactor->m_Measure.size()-1] == 6.0);
  double pt2[3];
  interactor->m_LineSourceVector1[interactor->m_LineSourceVector1.size()-1]->GetPoint2(pt2);
  CPPUNIT_ASSERT(pt2[0]+EPSILON > 4.8 && pt2[0]-EPSILON<4.8);
  CPPUNIT_ASSERT(pt2[1]+EPSILON > 3.6 && pt2[1]-EPSILON<3.6);
  line1->Delete();
  //////////////////////////////////////////////////////////////////////////

  albaDEL(interactor);
}
//-----------------------------------------------------------
void albaInteractor2DDistanceTest::TestSetManualDistance2() 
//-----------------------------------------------------------
{
	albaInteractor2DDistance *interactor = albaInteractor2DDistance::NewTestInstance();

  interactor->SetMeasureTypeToDistanceBetweenLines();

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

  interactor->SetManualDistance(3*sqrt(3.0));

  CPPUNIT_ASSERT(interactor->m_Measure[interactor->m_Measure.size()-1] == 3*sqrt(3.0));
  double pt1[3];
  interactor->m_LineSourceVector2[interactor->m_LineSourceVector2.size()-1]->GetPoint1(pt1);
  CPPUNIT_ASSERT(pt1[0]+EPSILON > 0.0 && pt1[0]-EPSILON<0.0);
  CPPUNIT_ASSERT(pt1[1]+EPSILON > 9.0 && pt1[1]-EPSILON<9.0);
  double pt2[3];
  interactor->m_LineSourceVector2[interactor->m_LineSourceVector2.size()-1]->GetPoint2(pt2);
  CPPUNIT_ASSERT(pt2[0]+EPSILON > 1.0 && pt2[0]-EPSILON<1.0);
  CPPUNIT_ASSERT(pt2[1]+EPSILON > 11.0 && pt2[1]-EPSILON<11.0);
  line1->Delete();
  line2->Delete();
  //////////////////////////////////////////////////////////////////////////

  albaDEL(interactor);
}
//-----------------------------------------------------------
void albaInteractor2DDistanceTest::TestIsDisableUndoAndOkCancel() 
//-----------------------------------------------------------
{
	albaInteractor2DDistance *interactor = albaInteractor2DDistance::NewTestInstance();

  CPPUNIT_ASSERT( interactor->IsDisableUndoAndOkCancel() == false );//Default value

  albaDEL(interactor);
}
//-----------------------------------------------------------
void albaInteractor2DDistanceTest::TestGetLastDistance() 
//-----------------------------------------------------------
{
	albaInteractor2DDistance *interactor = albaInteractor2DDistance::NewTestInstance();

  CPPUNIT_ASSERT( interactor->GetLastDistance() == 0 );

  interactor->m_Measure.push_back(3.0);
  interactor->m_Measure.push_back(5.0);

  CPPUNIT_ASSERT( interactor->GetLastDistance() == 5.0 );

  albaDEL(interactor);
}
//-----------------------------------------------------------
void albaInteractor2DDistanceTest::TestSetLabel() 
//-----------------------------------------------------------
{
	albaInteractor2DDistance *interactor = albaInteractor2DDistance::NewTestInstance();

  vtkALBATextActorMeter *textActor = vtkALBATextActorMeter::New();
  interactor->m_MeterVector.push_back(textActor);

  CPPUNIT_ASSERT( interactor->GetLabel() == "" );
  //////////////////////////////////////////////////////////////////////////
  interactor->SetLabel("TEST1");
  CPPUNIT_ASSERT( interactor->GetLabel() == "TEST1" );
  //////////////////////////////////////////////////////////////////////////
  interactor->SetLabel("TEST2");
  CPPUNIT_ASSERT( interactor->GetLabel() == "TEST2" );

  textActor->Delete();
  albaDEL(interactor);
}
