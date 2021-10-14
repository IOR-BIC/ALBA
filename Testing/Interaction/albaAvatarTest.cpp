/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAvatarTest
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
#include "albaAvatarTest.h"
#include "albaAvatar.h"
#include "albaSmartPointer.h"
#include "albaViewVTK.h"
#include "vtkObject.h"
#include "vtkProp3D.h"
#include "vtkRenderer.h"
#include "albaDeviceButtonsPadTracker.h"

albaCxxTypeMacro(ConcreteMafAvatar);

//----------------------------------------------------------------------------
ConcreteMafAvatar::ConcreteMafAvatar()
{
	m_LastReceivedEventID = -1;
}

//----------------------------------------------------------------------------
ConcreteMafAvatar::~ConcreteMafAvatar()
{

}

//----------------------------------------------------------------------------
void ConcreteMafAvatar::OnEvent(albaEventBase *alba_event)
{
  m_LastReceivedEventID =  alba_event->GetId();
  Superclass::OnEvent(alba_event);
}

//----------------------------------------------------------------------------
int ConcreteMafAvatar::GetLastReceivedEventID()
{
  return m_LastReceivedEventID;
}


//----------------------------------------------------------------------------
void albaAvatarTest::TestFixture()
{

}

//----------------------------------------------------------------------------
void albaAvatarTest::TestConstructorDestructor()
{
  ConcreteMafAvatar *dummyAvatar = NULL;
  
  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  dummyAvatar->Delete();

  albaSmartPointer<ConcreteMafAvatar> cmss;
}

//----------------------------------------------------------------------------
void albaAvatarTest::TestSetGetView()
{
  ConcreteMafAvatar *dummyAvatar = NULL;

  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  albaView *view = NULL;

  view = dummyAvatar->GetView();
  CPPUNIT_ASSERT(view == NULL);

  albaViewVTK *dummyView = new albaViewVTK();
  CPPUNIT_ASSERT(dummyView != NULL);

  dummyAvatar->SetRendererAndView(NULL,dummyView);
  view = dummyAvatar->GetView();

  CPPUNIT_ASSERT(view == dummyView);

  cppDEL(dummyView);
  dummyAvatar->Delete();
}

//----------------------------------------------------------------------------
void albaAvatarTest::TestGetActor3D()
{
  ConcreteMafAvatar *dummyAvatar = NULL;

  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  vtkProp3D *dummyActor3D = NULL;

  dummyActor3D = dummyAvatar->GetActor3D();
  CPPUNIT_ASSERT(dummyActor3D != NULL);

  dummyAvatar->Delete();
}

//----------------------------------------------------------------------------
void albaAvatarTest::TestPick()
{
  ConcreteMafAvatar *dummyAvatar = NULL;

  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  int picked = dummyAvatar->Pick(10,20);
  CPPUNIT_ASSERT(picked == false);
  dummyAvatar->Delete();
}

//----------------------------------------------------------------------------
void albaAvatarTest::TestOnEvent()
{
  ConcreteMafAvatar *dummyAvatar = NULL;

  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  albaEventBase dummyEventNotHandledByTestedClass;

  int dummyId = -9999;
  dummyEventNotHandledByTestedClass.SetId(dummyId);
  dummyEventNotHandledByTestedClass.SetSender(this);

  dummyAvatar->OnEvent(&dummyEventNotHandledByTestedClass);

  CPPUNIT_ASSERT(dummyAvatar->GetLastReceivedEventID() == dummyId);
  dummyAvatar->Delete();
}

//----------------------------------------------------------------------------
void albaAvatarTest::TestSetGetTracker()
{
  ConcreteMafAvatar *dummyAvatar = NULL;

  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  albaDeviceButtonsPadTracker *tracker = NULL;
  tracker = dummyAvatar->GetTracker();
  CPPUNIT_ASSERT(tracker == NULL);

  albaDeviceButtonsPadTracker *dummyTracker = albaDeviceButtonsPadTracker::New();
  CPPUNIT_ASSERT(dummyTracker);
  
  dummyAvatar->SetTracker(dummyTracker);
  tracker = dummyAvatar->GetTracker();

  CPPUNIT_ASSERT(tracker == dummyTracker);

  dummyAvatar->Delete();
  
  CPPUNIT_ASSERT(dummyTracker);
  dummyTracker->Delete();
}

//----------------------------------------------------------------------------
void albaAvatarTest::TestSetGetMode()
{
  ConcreteMafAvatar *dummyAvatar = NULL;

  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  int mode = dummyAvatar->GetMode();
  CPPUNIT_ASSERT(mode == albaAvatar::MODE_3D);

  dummyAvatar->SetMode(albaAvatar::MODE_2D);
  CPPUNIT_ASSERT(dummyAvatar->GetMode() == albaAvatar::MODE_2D);

  dummyAvatar->SetMode(albaAvatar::MODE_3D);
  CPPUNIT_ASSERT(dummyAvatar->GetMode() == albaAvatar::MODE_3D);

  dummyAvatar->Delete();
}

//----------------------------------------------------------------------------
void albaAvatarTest::TestSetModeTo2D()
{
  ConcreteMafAvatar *dummyAvatar = NULL;

  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  int mode = dummyAvatar->GetMode();
  CPPUNIT_ASSERT(mode == albaAvatar::MODE_3D);

  dummyAvatar->SetModeTo2D();
  
  mode = dummyAvatar->GetMode();
  CPPUNIT_ASSERT(mode == albaAvatar::MODE_2D);

  dummyAvatar->Delete();
}

//----------------------------------------------------------------------------
void albaAvatarTest::TestSetModeTo3D()
{
  ConcreteMafAvatar *dummyAvatar = NULL;

  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  int mode = dummyAvatar->GetMode();
  CPPUNIT_ASSERT(mode == albaAvatar::MODE_3D);

  dummyAvatar->SetModeTo2D();
mode = dummyAvatar->GetMode();
  CPPUNIT_ASSERT(mode == albaAvatar::MODE_2D);

  dummyAvatar->SetModeTo3D();
  mode = dummyAvatar->GetMode();
  CPPUNIT_ASSERT(mode == albaAvatar::MODE_3D);
  dummyAvatar->Delete();

}

//----------------------------------------------------------------------------
void albaAvatarTest::TestGetPicker()
{
  ConcreteMafAvatar *dummyAvatar = NULL;

  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  vtkAbstractPropPicker *picker = NULL;
  picker = dummyAvatar->GetPicker();

  CPPUNIT_ASSERT(picker == NULL);

  dummyAvatar->Delete();
}

