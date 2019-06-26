/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUITransformTextEntriesTest
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

#include "albaGUITransformTextEntriesTest.h"
#include "albaVMESurface.h"
#include "albaDecl.h"
#include "albaGUITransformTextEntries.h"

#include "vtkSphereSource.h"

#include <iostream>

//----------------------------------------------------------------------------
void albaGUITransformTextEntriesTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUITransformTextEntriesTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_VMESphere = albaVMESurface::New();
  m_VMESphere->SetName("m_VMESphere");
  
  m_VTKSphere = vtkSphereSource::New();
  m_VMESphere->SetData(m_VTKSphere->GetOutput(), -1);
}
//----------------------------------------------------------------------------
void albaGUITransformTextEntriesTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_VMESphere);
  vtkDEL(m_VTKSphere);
}
//----------------------------------------------------------------------------
void albaGUITransformTextEntriesTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  albaGUITransformTextEntries *guiTransformTextEntries = new albaGUITransformTextEntries(m_VMESphere, NULL, true, true);
  cppDEL(guiTransformTextEntries);
}

//----------------------------------------------------------------------------
void albaGUITransformTextEntriesTest::TestSetAbsPose()
//----------------------------------------------------------------------------
{
  albaGUITransformTextEntries *guiTransformTextEntries = new albaGUITransformTextEntries(m_VMESphere, NULL, true, true);
  
  albaMatrix absPose;
  absPose.SetElement(0,3, 0);
  absPose.SetElement(1,3, 10);
  absPose.SetElement(2,3, 20);

  guiTransformTextEntries->SetAbsPose(&absPose);

  CPPUNIT_ASSERT_EQUAL(guiTransformTextEntries->m_Position[0], 0.0);
  CPPUNIT_ASSERT_EQUAL(guiTransformTextEntries->m_Position[1], 10.0);
  CPPUNIT_ASSERT_EQUAL(guiTransformTextEntries->m_Position[2], 20.0);

  cppDEL(guiTransformTextEntries);
}

//----------------------------------------------------------------------------
void albaGUITransformTextEntriesTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  DummyObserver *dummyObserver = new DummyObserver();

  albaGUITransformTextEntries *guiTransformTextEntries = new albaGUITransformTextEntries(m_VMESphere, dummyObserver, true, true);
  
  //  in response to an ID_TRANSLATE_X event from GUI...(*)
  albaEvent eventSent(this, albaGUITransformTextEntries::ID_TRANSLATE_X);
  guiTransformTextEntries->OnEvent(&eventSent);
  
  int dummyReceivedEventID = dummyObserver->GetLastReceivedEventID();

  // (*)... the observer is notified with a ID_TRANSFORM event 
  // containing the already set VME ABS matrix
  CPPUNIT_ASSERT(dummyReceivedEventID == ID_TRANSFORM);

  cppDEL(guiTransformTextEntries);

  cppDEL(dummyObserver);
}
