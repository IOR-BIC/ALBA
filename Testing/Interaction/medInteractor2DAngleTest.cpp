/*=========================================================================

 Program: MAF2
 Module: medInteractor2DAngleTest
 Authors: Roberto Mucci
 
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
#include "medInteractor2DAngleTest.h"
#include "mafInteractionTests.h"

#include "medInteractor2DAngle.h"
#include "mafGUIFrame.h"

#include "vtkLineSource.h"

#include "wx/module.h"

#define EPSILON 0.001

//-----------------------------------------------------------
void medInteractor2DAngleTest::setUp()
//-----------------------------------------------------------
{
  m_Win = new mafGUIFrame("testGui", wxDefaultPosition, wxSize(800, 600));
  wxModule::RegisterModules();
  wxModule::InitializeModules();
}
//-----------------------------------------------------------
void medInteractor2DAngleTest::tearDown()
//-----------------------------------------------------------
{
  wxModule::CleanUpModules();

  delete m_Win;

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medInteractor2DAngleTest::TestFixture()
//-----------------------------------------------------------
{

}
//-----------------------------------------------------------
void medInteractor2DAngleTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medInteractor2DAngle *interactor = medInteractor2DAngle::NewTest();

  interactor->Delete();
}
//-----------------------------------------------------------
void medInteractor2DAngleTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void medInteractor2DAngleTest::TestSetMeasureType() 
//-----------------------------------------------------------
{
  medInteractor2DAngle *interactor = medInteractor2DAngle::NewTest();

  interactor->SetMeasureType(medInteractor2DAngle::ANGLE_BETWEEN_POINTS);
  CPPUNIT_ASSERT( interactor->m_MeasureType == medInteractor2DAngle::ANGLE_BETWEEN_POINTS );

  interactor->SetMeasureTypeToAngleBetweenLines();
  CPPUNIT_ASSERT( interactor->m_MeasureType == medInteractor2DAngle::ANGLE_BETWEEN_LINES );

  interactor->SetMeasureTypeToAngleBetweenPoints();
  CPPUNIT_ASSERT( interactor->m_MeasureType == medInteractor2DAngle::ANGLE_BETWEEN_POINTS );

  interactor->Delete();
}

//-----------------------------------------------------------
void medInteractor2DAngleTest::TestSetManualDistance() 
//-----------------------------------------------------------
{
  medInteractor2DAngle *interactor = medInteractor2DAngle::NewTest();
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

  interactor->Delete();
}
//-----------------------------------------------------------
void medInteractor2DAngleTest::TestIsDisableUndoAndOkCancel() 
//-----------------------------------------------------------
{
  medInteractor2DAngle *interactor = medInteractor2DAngle::NewTest();

  CPPUNIT_ASSERT( interactor->IsDisableUndoAndOkCancel() == false );//Default value

  interactor->Delete();
}
//-----------------------------------------------------------
void medInteractor2DAngleTest::TestGetLastAngle() 
//-----------------------------------------------------------
{
  medInteractor2DAngle *interactor = medInteractor2DAngle::NewTest();

  CPPUNIT_ASSERT( interactor->GetLastAngle() == 0 );

  interactor->m_Measure.push_back(3.0);
  interactor->m_Measure.push_back(5.0);

  CPPUNIT_ASSERT( interactor->GetLastAngle() == 5.0 );

  interactor->Delete();
}