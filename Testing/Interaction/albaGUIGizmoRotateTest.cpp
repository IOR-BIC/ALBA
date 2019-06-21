/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIGizmoRotateTest
 Authors: Stefano Perticoni
 
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
#include "albaInteractionTests.h"

#include "albaGUIGizmoRotateTest.h"
#include "albaVMESurface.h"
#include "albaDecl.h"
#include "albaGUIGizmoRotate.h"
#include "albaTransform.h"

#include "vtkCubeSource.h"

#include <iostream>


//----------------------------------------------------------------------------
void albaGUIGizmoRotateTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUIGizmoRotateTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_VMECube = albaVMESurface::New();
  m_VMECube->SetName("m_VMECube");
  
  m_VTKCube = vtkCubeSource::New();
  m_VMECube->SetData(m_VTKCube->GetOutput(), -1);  
}
//----------------------------------------------------------------------------
void albaGUIGizmoRotateTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_VMECube);
  vtkDEL(m_VTKCube);
}
//----------------------------------------------------------------------------
void albaGUIGizmoRotateTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  albaGUIGizmoRotate *guiGizmoRotate = new albaGUIGizmoRotate(NULL, true);
  CPPUNIT_ASSERT(guiGizmoRotate->m_Listener == NULL);
  cppDEL(guiGizmoRotate);

  guiGizmoRotate = new albaGUIGizmoRotate(m_VMECube, true);
  CPPUNIT_ASSERT(guiGizmoRotate->m_Listener == m_VMECube);
  cppDEL(guiGizmoRotate);
}

//----------------------------------------------------------------------------
void albaGUIGizmoRotateTest::TestSetAbsOrientation()
//----------------------------------------------------------------------------
{
  albaGUIGizmoRotate *guiGizmoRotate = new albaGUIGizmoRotate(NULL, true);
  
  CPPUNIT_ASSERT_EQUAL(guiGizmoRotate->m_Orientation[0], 0.0);
  CPPUNIT_ASSERT_EQUAL(guiGizmoRotate->m_Orientation[1], 0.0);
  CPPUNIT_ASSERT_EQUAL(guiGizmoRotate->m_Orientation[2], 0.0);

  albaMatrix absPose;
  albaTransform::RotateZ(absPose, 45, PRE_MULTIPLY);
  absPose.Print(cout);
  
  guiGizmoRotate->SetAbsOrientation(&absPose);

  CPPUNIT_ASSERT_EQUAL(guiGizmoRotate->m_Orientation[0], 0.0);
  CPPUNIT_ASSERT_EQUAL(guiGizmoRotate->m_Orientation[1], 0.0);
  CPPUNIT_ASSERT_EQUAL(guiGizmoRotate->m_Orientation[2], 45.0);

  cppDEL(guiGizmoRotate);
}

//----------------------------------------------------------------------------
void albaGUIGizmoRotateTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  DummyObserver *dummy = new DummyObserver();

  albaGUIGizmoRotate *guiGizmoRotate = new albaGUIGizmoRotate(dummy, true);
  albaEvent eventSent(this, albaGUIGizmoRotate::ID_ROTATE_X);
  guiGizmoRotate->OnEvent(&eventSent);
  
  int dummyReceivedEventID = dummy->GetLastReceivedEventID();

  CPPUNIT_ASSERT(dummyReceivedEventID == albaGUIGizmoRotate::ID_ROTATE_X);

  eventSent.SetId(albaGUIGizmoRotate::ID_ROTATE_Y);
  guiGizmoRotate->OnEvent(&eventSent);

  dummyReceivedEventID = dummy->GetLastReceivedEventID();

  CPPUNIT_ASSERT(dummyReceivedEventID == albaGUIGizmoRotate::ID_ROTATE_Y);

  eventSent.SetId(albaGUIGizmoRotate::ID_ROTATE_Z);
  guiGizmoRotate->OnEvent(&eventSent);

  dummyReceivedEventID = dummy->GetLastReceivedEventID();

  CPPUNIT_ASSERT(dummyReceivedEventID == albaGUIGizmoRotate::ID_ROTATE_Z);

  cppDEL(guiGizmoRotate);

  cppDEL(dummy);
}
