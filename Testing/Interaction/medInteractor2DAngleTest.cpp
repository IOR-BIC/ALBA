/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medInteractor2DAngleTest.cpp,v $
Language:  C++
Date:      $Date: 2011-03-23 14:55:17 $
Version:   $Revision: 1.1.2.1 $
Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "medInteractor2DAngleTest.h"

#include "medInteractor2DAngle.h"
#include "mafGUIFrame.h"

#include "vtkLineSource.h"

#include "wx/module.h"

#define EPSILON 0.001

//----------------------------------------------------------------------------
class DummyObserver : public mafObserver
  //----------------------------------------------------------------------------
{
public:

  DummyObserver() 
  {
    m_LastReceivedEventID = -1;
  };
  ~DummyObserver()
  {
  };

  void	DummyObserver::OnEvent(mafEventBase *maf_event)
  {
    m_LastReceivedEventID =  maf_event->GetId();
  }

  int DummyObserver::GetLastReceivedEventID()
  {
    return m_LastReceivedEventID;
  }

protected:

  int m_LastReceivedEventID;
};
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