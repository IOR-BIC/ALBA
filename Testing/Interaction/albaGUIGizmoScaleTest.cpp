/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIGizmoScaleTest
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

#include "albaGUIGizmoScaleTest.h"
#include "albaVMESurface.h"
#include "albaDecl.h"
#include "albaGUIGizmoScale.h"
#include "albaTransform.h"

#include "vtkCubeSource.h"

#include <iostream>

//----------------------------------------------------------------------------
void albaGUIGizmoScaleTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUIGizmoScaleTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_VMECube = albaVMESurface::New();
  m_VMECube->SetName("m_VMECube");
  
  m_VTKCube = vtkCubeSource::New();
  m_VMECube->SetData(m_VTKCube->GetOutput(), -1);  
}
//----------------------------------------------------------------------------
void albaGUIGizmoScaleTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_VMECube);
  vtkDEL(m_VTKCube);
}
//----------------------------------------------------------------------------
void albaGUIGizmoScaleTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  albaGUIGizmoScale *guiGizmoScale = new albaGUIGizmoScale(NULL, true);
  CPPUNIT_ASSERT(guiGizmoScale->m_Listener == NULL);
  cppDEL(guiGizmoScale);

  guiGizmoScale = new albaGUIGizmoScale(m_VMECube, true);
  CPPUNIT_ASSERT(guiGizmoScale->m_Listener == m_VMECube);
  cppDEL(guiGizmoScale);
}

//----------------------------------------------------------------------------
void albaGUIGizmoScaleTest::TestSetAbsScaling()
//----------------------------------------------------------------------------
{
  albaGUIGizmoScale *guiGizmoScale = new albaGUIGizmoScale(NULL, true);
  
  CPPUNIT_ASSERT_EQUAL(guiGizmoScale->m_Scaling[0], 1.0);
  CPPUNIT_ASSERT_EQUAL(guiGizmoScale->m_Scaling[1], 1.0);
  CPPUNIT_ASSERT_EQUAL(guiGizmoScale->m_Scaling[2], 1.0);

  albaMatrix absScaling;
  albaTransform::Scale(absScaling, 2, 3, 4, PRE_MULTIPLY);
  absScaling.Print(cout);
  
  guiGizmoScale->SetAbsScaling(&absScaling);

  CPPUNIT_ASSERT_EQUAL(guiGizmoScale->m_Scaling[0], 2.0);
  CPPUNIT_ASSERT_EQUAL(guiGizmoScale->m_Scaling[1], 3.0);
  CPPUNIT_ASSERT_EQUAL(guiGizmoScale->m_Scaling[2], 4.0);


  cppDEL(guiGizmoScale);
}

//----------------------------------------------------------------------------
void albaGUIGizmoScaleTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  DummyObserver *dummy = new DummyObserver();

  albaGUIGizmoScale *guiGizmoScale = new albaGUIGizmoScale(dummy, true);
  albaEvent eventSent(this, albaGUIGizmoScale::ID_SCALE_X);
  guiGizmoScale->OnEvent(&eventSent);
  
  int dummyReceivedEventID = dummy->GetLastReceivedEventID();

  CPPUNIT_ASSERT(dummyReceivedEventID == albaGUIGizmoScale::ID_SCALE_X);

  eventSent.SetId(albaGUIGizmoScale::ID_SCALE_Y);
  guiGizmoScale->OnEvent(&eventSent);

  dummyReceivedEventID = dummy->GetLastReceivedEventID();

  CPPUNIT_ASSERT(dummyReceivedEventID == albaGUIGizmoScale::ID_SCALE_Y);

  eventSent.SetId(albaGUIGizmoScale::ID_SCALE_Z);
  guiGizmoScale->OnEvent(&eventSent);

  dummyReceivedEventID = dummy->GetLastReceivedEventID();

  CPPUNIT_ASSERT(dummyReceivedEventID == albaGUIGizmoScale::ID_SCALE_Z);

  cppDEL(guiGizmoScale);

  cppDEL(dummy);
}
