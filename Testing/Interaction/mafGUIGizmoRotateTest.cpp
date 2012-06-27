/*=========================================================================

 Program: MAF2
 Module: mafGUIGizmoRotateTest
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

#include "mafGUIGizmoRotateTest.h"
#include "mafVMESurface.h"
#include "mafDecl.h"
#include "mafGUIGizmoRotate.h"
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
void mafGUIGizmoRotateTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIGizmoRotateTest::setUp()
//----------------------------------------------------------------------------
{
  m_VMECube = mafVMESurface::New();
  m_VMECube->SetName("m_VMECube");
  
  m_VTKCube = vtkCubeSource::New();
  m_VMECube->SetData(m_VTKCube->GetOutput(), -1);  
}
//----------------------------------------------------------------------------
void mafGUIGizmoRotateTest::tearDown()
//----------------------------------------------------------------------------
{
  mafDEL(m_VMECube);
  vtkDEL(m_VTKCube);
}
//----------------------------------------------------------------------------
void mafGUIGizmoRotateTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  mafGUIGizmoRotate *guiGizmoRotate = new mafGUIGizmoRotate(NULL, true);
  CPPUNIT_ASSERT(guiGizmoRotate->m_Listener == NULL);
  cppDEL(guiGizmoRotate);

  guiGizmoRotate = new mafGUIGizmoRotate(m_VMECube, true);
  CPPUNIT_ASSERT(guiGizmoRotate->m_Listener == m_VMECube);
  cppDEL(guiGizmoRotate);
}

//----------------------------------------------------------------------------
void mafGUIGizmoRotateTest::TestSetAbsOrientation()
//----------------------------------------------------------------------------
{
  mafGUIGizmoRotate *guiGizmoRotate = new mafGUIGizmoRotate(NULL, true);
  
  CPPUNIT_ASSERT_EQUAL(guiGizmoRotate->m_Orientation[0], 0.0);
  CPPUNIT_ASSERT_EQUAL(guiGizmoRotate->m_Orientation[1], 0.0);
  CPPUNIT_ASSERT_EQUAL(guiGizmoRotate->m_Orientation[2], 0.0);

  mafMatrix absPose;
  mafTransform::RotateZ(absPose, 45, PRE_MULTIPLY);
  absPose.Print(cout);
  
  guiGizmoRotate->SetAbsOrientation(&absPose);

  CPPUNIT_ASSERT_EQUAL(guiGizmoRotate->m_Orientation[0], 0.0);
  CPPUNIT_ASSERT_EQUAL(guiGizmoRotate->m_Orientation[1], 0.0);
  CPPUNIT_ASSERT_EQUAL(guiGizmoRotate->m_Orientation[2], 45.0);

  cppDEL(guiGizmoRotate);
}

//----------------------------------------------------------------------------
void mafGUIGizmoRotateTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  DummyObserver *dummy = new DummyObserver();

  mafGUIGizmoRotate *guiGizmoRotate = new mafGUIGizmoRotate(dummy, true);
  mafEvent eventSent(this, mafGUIGizmoRotate::ID_ROTATE_X);
  guiGizmoRotate->OnEvent(&eventSent);
  
  int dummyReceivedEventID = dummy->GetLastReceivedEventID();

  CPPUNIT_ASSERT(dummyReceivedEventID == mafGUIGizmoRotate::ID_ROTATE_X);

  eventSent.SetId(mafGUIGizmoRotate::ID_ROTATE_Y);
  guiGizmoRotate->OnEvent(&eventSent);

  dummyReceivedEventID = dummy->GetLastReceivedEventID();

  CPPUNIT_ASSERT(dummyReceivedEventID == mafGUIGizmoRotate::ID_ROTATE_Y);

  eventSent.SetId(mafGUIGizmoRotate::ID_ROTATE_Z);
  guiGizmoRotate->OnEvent(&eventSent);

  dummyReceivedEventID = dummy->GetLastReceivedEventID();

  CPPUNIT_ASSERT(dummyReceivedEventID == mafGUIGizmoRotate::ID_ROTATE_Z);

  cppDEL(guiGizmoRotate);

  cppDEL(dummy);
}
