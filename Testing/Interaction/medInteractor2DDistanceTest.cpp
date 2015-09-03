/*=========================================================================

 Program: MAF2
 Module: medInteractor2DDistanceTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "medInteractor2DDistanceTest.h"
#include "mafInteractionTests.h"

#include "medInteractor2DDistance.h"
#include "mafGUIFrame.h"

#include "vtkLineSource.h"

#include "wx/module.h"

#define EPSILON 0.001

//-----------------------------------------------------------
void medInteractor2DDistanceTest::BeforeTest()
//-----------------------------------------------------------
{
  m_Win = new mafGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  wxModule::RegisterModules();
  wxModule::InitializeModules();
}
//-----------------------------------------------------------
void medInteractor2DDistanceTest::AfterTest()
//-----------------------------------------------------------
{
  wxModule::CleanUpModules();
  delete m_Win;
}
//-----------------------------------------------------------
void medInteractor2DDistanceTest::TestFixture()
//-----------------------------------------------------------
{

}
//-----------------------------------------------------------
void medInteractor2DDistanceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medInteractor2DDistance *interactor = medInteractor2DDistance::NewTest();

  interactor->Delete();
}
//-----------------------------------------------------------
void medInteractor2DDistanceTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void medInteractor2DDistanceTest::TestSetMeasureType() 
//-----------------------------------------------------------
{
  medInteractor2DDistance *interactor = medInteractor2DDistance::NewTest();

  interactor->SetMeasureType(medInteractor2DDistance::DISTANCE_BETWEEN_POINTS);
  CPPUNIT_ASSERT( interactor->m_MeasureType == medInteractor2DDistance::DISTANCE_BETWEEN_POINTS );
  //////////////////////////////////////////////////////////////////////////
  interactor->SetMeasureTypeToDistanceBetweenLines();
  CPPUNIT_ASSERT( interactor->m_MeasureType == medInteractor2DDistance::DISTANCE_BETWEEN_LINES );
  //////////////////////////////////////////////////////////////////////////
  interactor->SetMeasureTypeToDistanceBetweenPoints();
  CPPUNIT_ASSERT( interactor->m_MeasureType == medInteractor2DDistance::DISTANCE_BETWEEN_POINTS );

  interactor->Delete();
}
//-----------------------------------------------------------
void medInteractor2DDistanceTest::TestGenerateHistogram() 
//-----------------------------------------------------------
{
  medInteractor2DDistance *interactor = medInteractor2DDistance::NewTest();

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

  interactor->Delete();
}
//-----------------------------------------------------------
void medInteractor2DDistanceTest::TestSetManualDistance1() 
//-----------------------------------------------------------
{
  medInteractor2DDistance *interactor = medInteractor2DDistance::NewTest();
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

  interactor->Delete();
}
//-----------------------------------------------------------
void medInteractor2DDistanceTest::TestSetManualDistance2() 
//-----------------------------------------------------------
{
  medInteractor2DDistance *interactor = medInteractor2DDistance::NewTest();
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

  interactor->Delete();
}
//-----------------------------------------------------------
void medInteractor2DDistanceTest::TestIsDisableUndoAndOkCancel() 
//-----------------------------------------------------------
{
  medInteractor2DDistance *interactor = medInteractor2DDistance::NewTest();

  CPPUNIT_ASSERT( interactor->IsDisableUndoAndOkCancel() == false );//Default value

  interactor->Delete();
}
//-----------------------------------------------------------
void medInteractor2DDistanceTest::TestGetLastDistance() 
//-----------------------------------------------------------
{
  medInteractor2DDistance *interactor = medInteractor2DDistance::NewTest();

  CPPUNIT_ASSERT( interactor->GetLastDistance() == 0 );

  interactor->m_Measure.push_back(3.0);
  interactor->m_Measure.push_back(5.0);

  CPPUNIT_ASSERT( interactor->GetLastDistance() == 5.0 );

  interactor->Delete();
}
//-----------------------------------------------------------
void medInteractor2DDistanceTest::TestSetLabel() 
//-----------------------------------------------------------
{
  medInteractor2DDistance *interactor = medInteractor2DDistance::NewTest();

  vtkMAFTextActorMeter *textActor = vtkMAFTextActorMeter::New();
  interactor->m_MeterVector.push_back(textActor);

  CPPUNIT_ASSERT( interactor->GetLabel() == "" );
  //////////////////////////////////////////////////////////////////////////
  interactor->SetLabel("TEST1");
  CPPUNIT_ASSERT( interactor->GetLabel() == "TEST1" );
  //////////////////////////////////////////////////////////////////////////
  interactor->SetLabel("TEST2");
  CPPUNIT_ASSERT( interactor->GetLabel() == "TEST2" );

  textActor->Delete();
  interactor->Delete();
}
