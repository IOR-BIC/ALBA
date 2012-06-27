/*=========================================================================

 Program: MAF2
 Module: mafGUIGizmoScaleTest
 Authors: Stefano Perticoni
 
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

#include <cppunit/config/SourcePrefix.h>

#include "mafGUIGizmoScaleTest.h"
#include "mafVMESurface.h"
#include "mafDecl.h"
#include "mafGUIGizmoScale.h"
#include "mafTransform.h"

#include "vtkCubeSource.h"

#include <iostream>

void	DummyObserver::OnEvent(mafEventBase *maf_event)
{
  m_LastReceivedEventID =  maf_event->GetId();
}

int DummyObserver::GetLastReceivedEventID()
{
  return m_LastReceivedEventID;
}

//----------------------------------------------------------------------------
void mafGUIGizmoScaleTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIGizmoScaleTest::setUp()
//----------------------------------------------------------------------------
{
  m_VMECube = mafVMESurface::New();
  m_VMECube->SetName("m_VMECube");
  
  m_VTKCube = vtkCubeSource::New();
  m_VMECube->SetData(m_VTKCube->GetOutput(), -1);  
}
//----------------------------------------------------------------------------
void mafGUIGizmoScaleTest::tearDown()
//----------------------------------------------------------------------------
{
  mafDEL(m_VMECube);
  vtkDEL(m_VTKCube);
}
//----------------------------------------------------------------------------
void mafGUIGizmoScaleTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  mafGUIGizmoScale *guiGizmoScale = new mafGUIGizmoScale(NULL, true);
  CPPUNIT_ASSERT(guiGizmoScale->m_Listener == NULL);
  cppDEL(guiGizmoScale);

  guiGizmoScale = new mafGUIGizmoScale(m_VMECube, true);
  CPPUNIT_ASSERT(guiGizmoScale->m_Listener == m_VMECube);
  cppDEL(guiGizmoScale);
}

//----------------------------------------------------------------------------
void mafGUIGizmoScaleTest::TestSetAbsScaling()
//----------------------------------------------------------------------------
{
  mafGUIGizmoScale *guiGizmoScale = new mafGUIGizmoScale(NULL, true);
  
  CPPUNIT_ASSERT_EQUAL(guiGizmoScale->m_Scaling[0], 1.0);
  CPPUNIT_ASSERT_EQUAL(guiGizmoScale->m_Scaling[1], 1.0);
  CPPUNIT_ASSERT_EQUAL(guiGizmoScale->m_Scaling[2], 1.0);

  mafMatrix absScaling;
  mafTransform::Scale(absScaling, 2, 3, 4, PRE_MULTIPLY);
  absScaling.Print(cout);
  
  guiGizmoScale->SetAbsScaling(&absScaling);

  CPPUNIT_ASSERT_EQUAL(guiGizmoScale->m_Scaling[0], 2.0);
  CPPUNIT_ASSERT_EQUAL(guiGizmoScale->m_Scaling[1], 3.0);
  CPPUNIT_ASSERT_EQUAL(guiGizmoScale->m_Scaling[2], 4.0);


  cppDEL(guiGizmoScale);
}

//----------------------------------------------------------------------------
void mafGUIGizmoScaleTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  DummyObserver *dummy = new DummyObserver();

  mafGUIGizmoScale *guiGizmoScale = new mafGUIGizmoScale(dummy, true);
  mafEvent eventSent(this, mafGUIGizmoScale::ID_SCALE_X);
  guiGizmoScale->OnEvent(&eventSent);
  
  int dummyReceivedEventID = dummy->GetLastReceivedEventID();

  CPPUNIT_ASSERT(dummyReceivedEventID == mafGUIGizmoScale::ID_SCALE_X);

  eventSent.SetId(mafGUIGizmoScale::ID_SCALE_Y);
  guiGizmoScale->OnEvent(&eventSent);

  dummyReceivedEventID = dummy->GetLastReceivedEventID();

  CPPUNIT_ASSERT(dummyReceivedEventID == mafGUIGizmoScale::ID_SCALE_Y);

  eventSent.SetId(mafGUIGizmoScale::ID_SCALE_Z);
  guiGizmoScale->OnEvent(&eventSent);

  dummyReceivedEventID = dummy->GetLastReceivedEventID();

  CPPUNIT_ASSERT(dummyReceivedEventID == mafGUIGizmoScale::ID_SCALE_Z);

  cppDEL(guiGizmoScale);

  cppDEL(dummy);
}
