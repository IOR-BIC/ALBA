/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUIGizmoTranslateTest.cpp,v $
Language:  C++
Date:      $Date: 2009-07-15 14:27:24 $
Version:   $Revision: 1.1.2.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 

//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>

#include "mafGUIGizmoTranslateTest.h"
#include "mafVMESurface.h"
#include "mafDecl.h"
#include "mafGUIGizmoTranslate.h"

#include "vtkSphereSource.h"

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
void mafGUIGizmoTranslateTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIGizmoTranslateTest::setUp()
//----------------------------------------------------------------------------
{
  m_VMESphere = mafVMESurface::New();
  m_VMESphere->SetName("m_VMESphere");
  
  m_VTKSphere = vtkSphereSource::New();
  m_VMESphere->SetData(m_VTKSphere->GetOutput(), -1);
  
}
//----------------------------------------------------------------------------
void mafGUIGizmoTranslateTest::tearDown()
//----------------------------------------------------------------------------
{
  mafDEL(m_VMESphere);
  vtkDEL(m_VTKSphere);
}
//----------------------------------------------------------------------------
void mafGUIGizmoTranslateTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  mafGUIGizmoTranslate *guiGizmoTranslate = new mafGUIGizmoTranslate(NULL,true);
  CPPUNIT_ASSERT(guiGizmoTranslate->m_Listener == NULL);
  cppDEL(guiGizmoTranslate);

  guiGizmoTranslate = new mafGUIGizmoTranslate(m_VMESphere,true);
  CPPUNIT_ASSERT(guiGizmoTranslate->m_Listener == m_VMESphere);
  cppDEL(guiGizmoTranslate);
}

//----------------------------------------------------------------------------
void mafGUIGizmoTranslateTest::TestSetAbsPosition()
//----------------------------------------------------------------------------
{
  mafGUIGizmoTranslate *guiGizmoTranslate = new mafGUIGizmoTranslate(NULL,true);
  
  mafMatrix absPose;
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
void mafGUIGizmoTranslateTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  DummyObserver *dummy = new DummyObserver();

  mafGUIGizmoTranslate *guiGizmoTranslate = new mafGUIGizmoTranslate(dummy,true);
  mafEvent eventSent(this, mafGUIGizmoTranslate::ID_TRANSLATE_X);
  guiGizmoTranslate->OnEvent(&eventSent);
  
  int dummyReceivedEventID = dummy->GetLastReceivedEventID();

  CPPUNIT_ASSERT(dummyReceivedEventID == mafGUIGizmoTranslate::ID_TRANSLATE_X);

  cppDEL(guiGizmoTranslate);

  cppDEL(dummy);
}
