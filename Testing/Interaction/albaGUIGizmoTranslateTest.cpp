/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIGizmoTranslateTest
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

#include "albaGUIGizmoTranslateTest.h"
#include "albaVMESurface.h"
#include "albaDecl.h"
#include "albaGUIGizmoTranslate.h"

#include "vtkSphereSource.h"

#include <iostream>

//----------------------------------------------------------------------------
void albaGUIGizmoTranslateTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUIGizmoTranslateTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_VMESphere = albaVMESurface::New();
  m_VMESphere->SetName("m_VMESphere");
  
  m_VTKSphere = vtkSphereSource::New();
  m_VMESphere->SetData(m_VTKSphere->GetOutput(), -1); 
}
//----------------------------------------------------------------------------
void albaGUIGizmoTranslateTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_VMESphere);
  vtkDEL(m_VTKSphere);
}
//----------------------------------------------------------------------------
void albaGUIGizmoTranslateTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  albaGUIGizmoTranslate *guiGizmoTranslate = new albaGUIGizmoTranslate(NULL,true);
  CPPUNIT_ASSERT(guiGizmoTranslate->m_Listener == NULL);
  cppDEL(guiGizmoTranslate);

  guiGizmoTranslate = new albaGUIGizmoTranslate(m_VMESphere,true);
  CPPUNIT_ASSERT(guiGizmoTranslate->m_Listener == m_VMESphere);
  cppDEL(guiGizmoTranslate);
}

//----------------------------------------------------------------------------
void albaGUIGizmoTranslateTest::TestSetAbsPosition()
//----------------------------------------------------------------------------
{
  albaGUIGizmoTranslate *guiGizmoTranslate = new albaGUIGizmoTranslate(NULL,true);
  
  albaMatrix absPose;
  absPose.SetElement(0,3, 0);
  absPose.SetElement(1,3, 10);
  absPose.SetElement(2,3, 20);

  guiGizmoTranslate->SetAbsPosition(&absPose);

  CPPUNIT_ASSERT_EQUAL(guiGizmoTranslate->m_Position[0], 0.0);
  CPPUNIT_ASSERT_EQUAL(guiGizmoTranslate->m_Position[1], 10.0);
  CPPUNIT_ASSERT_EQUAL(guiGizmoTranslate->m_Position[2], 20.0);

  cppDEL(guiGizmoTranslate);
}

//----------------------------------------------------------------------------
void albaGUIGizmoTranslateTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  DummyObserver *dummy = new DummyObserver();

  albaGUIGizmoTranslate *guiGizmoTranslate = new albaGUIGizmoTranslate(dummy,true);
  albaEvent eventSent(this, albaGUIGizmoTranslate::ID_TRANSLATE_X);
  guiGizmoTranslate->OnEvent(&eventSent);
  
  int dummyReceivedEventID = dummy->GetLastReceivedEventID();

  CPPUNIT_ASSERT(dummyReceivedEventID == albaGUIGizmoTranslate::ID_TRANSLATE_X);

  cppDEL(guiGizmoTranslate);

  cppDEL(dummy);
}
