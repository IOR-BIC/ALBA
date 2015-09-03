/*=========================================================================

 Program: MAF2
 Module: mafAvatarTest
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
#include "mafAvatarTest.h"
#include "mafAvatar.h"
#include "mafSmartPointer.h"
#include "mafViewVTK.h"
#include "vtkObject.h"
#include "vtkProp3D.h"
#include "vtkRenderer.h"
#include "mafDeviceButtonsPadTracker.h"

mafCxxTypeMacro(ConcreteMafAvatar);

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
void ConcreteMafAvatar::OnEvent(mafEventBase *maf_event)
{
  m_LastReceivedEventID =  maf_event->GetId();
  Superclass::OnEvent(maf_event);
}

//----------------------------------------------------------------------------
int ConcreteMafAvatar::GetLastReceivedEventID()
{
  return m_LastReceivedEventID;
}


//----------------------------------------------------------------------------
void mafAvatarTest::TestFixture()
{

}

//----------------------------------------------------------------------------
void mafAvatarTest::TestConstructorDestructor()
{
  ConcreteMafAvatar *dummyAvatar = NULL;
  
  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  dummyAvatar->Delete();

  mafSmartPointer<ConcreteMafAvatar> cmss;
}

//----------------------------------------------------------------------------
void mafAvatarTest::TestSetGetView()
{
  ConcreteMafAvatar *dummyAvatar = NULL;

  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  mafView *view = NULL;

  view = dummyAvatar->GetView();
  CPPUNIT_ASSERT(view == NULL);

  mafViewVTK *dummyView = new mafViewVTK();
  CPPUNIT_ASSERT(dummyView != NULL);

  dummyAvatar->SetView(dummyView);
  view = dummyAvatar->GetView();

  CPPUNIT_ASSERT(view == dummyView);

  cppDEL(dummyView);
  dummyAvatar->Delete();
}

//----------------------------------------------------------------------------
void mafAvatarTest::TestGetActor3D()
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
void mafAvatarTest::TestPick()
{
  ConcreteMafAvatar *dummyAvatar = NULL;

  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  int picked = dummyAvatar->Pick(10,20);
  CPPUNIT_ASSERT(picked == false);
  dummyAvatar->Delete();
}

//----------------------------------------------------------------------------
void mafAvatarTest::TestOnEvent()
{
  ConcreteMafAvatar *dummyAvatar = NULL;

  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  mafEventBase dummyEventNotHandledByTestedClass;

  int dummyId = -9999;
  dummyEventNotHandledByTestedClass.SetId(dummyId);
  dummyEventNotHandledByTestedClass.SetSender(this);

  dummyAvatar->OnEvent(&dummyEventNotHandledByTestedClass);

  CPPUNIT_ASSERT(dummyAvatar->GetLastReceivedEventID() == dummyId);
  dummyAvatar->Delete();
}

//----------------------------------------------------------------------------
void mafAvatarTest::TestSetGetTracker()
{
  ConcreteMafAvatar *dummyAvatar = NULL;

  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  mafDeviceButtonsPadTracker *tracker = NULL;
  tracker = dummyAvatar->GetTracker();
  CPPUNIT_ASSERT(tracker == NULL);

  mafDeviceButtonsPadTracker *dummyTracker = mafDeviceButtonsPadTracker::New();
  CPPUNIT_ASSERT(dummyTracker);
  
  dummyAvatar->SetTracker(dummyTracker);
  tracker = dummyAvatar->GetTracker();

  CPPUNIT_ASSERT(tracker == dummyTracker);

  dummyAvatar->Delete();
  
  CPPUNIT_ASSERT(dummyTracker);
  dummyTracker->Delete();
}

//----------------------------------------------------------------------------
void mafAvatarTest::TestSetGetMode()
{
  ConcreteMafAvatar *dummyAvatar = NULL;

  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  int mode = dummyAvatar->GetMode();
  CPPUNIT_ASSERT(mode == mafAvatar::MODE_3D);

  dummyAvatar->SetMode(mafAvatar::MODE_2D);
  CPPUNIT_ASSERT(dummyAvatar->GetMode() == mafAvatar::MODE_2D);

  dummyAvatar->SetMode(mafAvatar::MODE_3D);
  CPPUNIT_ASSERT(dummyAvatar->GetMode() == mafAvatar::MODE_3D);

  dummyAvatar->Delete();
}

//----------------------------------------------------------------------------
void mafAvatarTest::TestSetModeTo2D()
{
  ConcreteMafAvatar *dummyAvatar = NULL;

  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  int mode = dummyAvatar->GetMode();
  CPPUNIT_ASSERT(mode == mafAvatar::MODE_3D);

  dummyAvatar->SetModeTo2D();
  
  mode = dummyAvatar->GetMode();
  CPPUNIT_ASSERT(mode == mafAvatar::MODE_2D);

  dummyAvatar->Delete();
}

//----------------------------------------------------------------------------
void mafAvatarTest::TestSetModeTo3D()
{
  ConcreteMafAvatar *dummyAvatar = NULL;

  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  int mode = dummyAvatar->GetMode();
  CPPUNIT_ASSERT(mode == mafAvatar::MODE_3D);

  dummyAvatar->SetModeTo2D();
mode = dummyAvatar->GetMode();
  CPPUNIT_ASSERT(mode == mafAvatar::MODE_2D);

  dummyAvatar->SetModeTo3D();
  mode = dummyAvatar->GetMode();
  CPPUNIT_ASSERT(mode == mafAvatar::MODE_3D);
  dummyAvatar->Delete();

}

//----------------------------------------------------------------------------
void mafAvatarTest::TestGetPicker()
{
  ConcreteMafAvatar *dummyAvatar = NULL;

  dummyAvatar = ConcreteMafAvatar::New();
  CPPUNIT_ASSERT(dummyAvatar);

  vtkAbstractPropPicker *picker = NULL;
  picker = dummyAvatar->GetPicker();

  CPPUNIT_ASSERT(picker == NULL);

  dummyAvatar->Delete();
}

