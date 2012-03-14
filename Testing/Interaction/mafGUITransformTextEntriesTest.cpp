/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUITransformTextEntriesTest.cpp,v $
Language:  C++
Date:      $Date: 2009-10-02 08:56:15 $
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

#include "mafGUITransformTextEntriesTest.h"
#include "mafVMESurface.h"
#include "mafDecl.h"
#include "mafGUITransformTextEntries.h"

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
void mafGUITransformTextEntriesTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUITransformTextEntriesTest::setUp()
//----------------------------------------------------------------------------
{
  m_VMESphere = mafVMESurface::New();
  m_VMESphere->SetName("m_VMESphere");
  
  m_VTKSphere = vtkSphereSource::New();
  m_VMESphere->SetData(m_VTKSphere->GetOutput(), -1);
  
}
//----------------------------------------------------------------------------
void mafGUITransformTextEntriesTest::tearDown()
//----------------------------------------------------------------------------
{
  mafDEL(m_VMESphere);
  vtkDEL(m_VTKSphere);
}
//----------------------------------------------------------------------------
void mafGUITransformTextEntriesTest::TestConstructorDestructor()
//----------------------------------------------------------------------------
{
  mafGUITransformTextEntries *guiTransformTextEntries = new mafGUITransformTextEntries(m_VMESphere, NULL, true, true);
  cppDEL(guiTransformTextEntries);
}

//----------------------------------------------------------------------------
void mafGUITransformTextEntriesTest::TestSetAbsPose()
//----------------------------------------------------------------------------
{
  mafGUITransformTextEntries *guiTransformTextEntries = new mafGUITransformTextEntries(m_VMESphere, NULL, true, true);
  
  mafMatrix absPose;
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
void mafGUITransformTextEntriesTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  DummyObserver *dummyObserver = new DummyObserver();

  mafGUITransformTextEntries *guiTransformTextEntries = new mafGUITransformTextEntries(m_VMESphere, dummyObserver, true, true);
  
  //  in response to an ID_TRANSLATE_X event from GUI...(*)
  mafEvent eventSent(this, mafGUITransformTextEntries::ID_TRANSLATE_X);
  guiTransformTextEntries->OnEvent(&eventSent);
  
  int dummyReceivedEventID = dummyObserver->GetLastReceivedEventID();

  // (*)... the observer is notified with a ID_TRANSFORM event 
  // containing the already set VME ABS matrix
  CPPUNIT_ASSERT(dummyReceivedEventID == ID_TRANSFORM);

  cppDEL(guiTransformTextEntries);

  cppDEL(dummyObserver);
}
