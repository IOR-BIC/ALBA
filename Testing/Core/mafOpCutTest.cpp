/*=========================================================================

 Program: MAF2
 Module: mafOpCutTest
 Authors: Matteo Giacomoni
 
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
#include "mafCoreTests.h"
#include "mafOpCutTest.h"

#include "mafOpSelect.h"
#include "mafVMEGroup.h"
#include "mafEvent.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMESurface.h"
#include "mafVMEFactory.h"
#include "mafVMEVolumeGray.h"

#include <vector>

//----------------------------------------------------------------------------
void mafOpCutTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpCutTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_OpCut=new mafOpCut("CUT");
}
//----------------------------------------------------------------------------
void mafOpCutTest::AfterTest()
//----------------------------------------------------------------------------
{
	mafDEL(m_OpCut);
}
//----------------------------------------------------------------------------
void mafOpCutTest::TestOpDo()
//----------------------------------------------------------------------------
{
  mafVMEGroup *groupChild;
  mafNEW(groupChild);
  mafVMEGroup *groupParent;
  mafNEW(groupParent);

  groupParent->AddChild(groupChild);
  groupParent->Modified();
  groupParent->Update();

  groupChild->Modified();
  groupChild->Update();

  DummyObserver *observer = new DummyObserver();

  m_OpCut->SetInput(groupChild);
  m_OpCut->SetListener(observer);
  m_OpCut->OpDo();

  CPPUNIT_ASSERT(observer->GetEvent(0)->GetSender()==m_OpCut);
  CPPUNIT_ASSERT(observer->GetEvent(0)->GetVme()==groupChild);
  CPPUNIT_ASSERT(observer->GetEvent(0)->GetId()==VME_REMOVE);

  CPPUNIT_ASSERT(observer->GetEvent(1)->GetSender()==m_OpCut);
  CPPUNIT_ASSERT(observer->GetEvent(1)->GetVme()==groupParent);
  CPPUNIT_ASSERT(observer->GetEvent(1)->GetId()==VME_SELECTED);

  m_OpCut->ClipboardClear();

  delete observer;
  mafDEL(groupParent);
  mafDEL(groupChild);
}
//----------------------------------------------------------------------------
void mafOpCutTest::TestOpUndo()
//----------------------------------------------------------------------------
{
  mafVMEGroup *groupChild;
  mafNEW(groupChild);
  mafVMEGroup *groupParent;
  mafNEW(groupParent);

  groupParent->AddChild(groupChild);
  groupParent->Modified();
  groupParent->Update();

  groupChild->Modified();
  groupChild->Update();

  m_OpCut->SetInput(groupChild);
  m_OpCut->OpDo();

  groupChild->ReparentTo(NULL); //OpDo send only a message.

  CPPUNIT_ASSERT(groupChild->GetParent()==NULL);

  m_OpCut->OpUndo();

  CPPUNIT_ASSERT(groupChild->GetParent()==groupParent);

  m_OpCut->ClipboardClear();

  mafDEL(groupParent);
  mafDEL(groupChild);  
}
//----------------------------------------------------------------------------
void mafOpCutTest::TestOpDoVMETimeVarying()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_Cut/TestMSFOpCut/TestMSFOpCut.msf";
  mafVMEStorage storage;
  storage.GetRoot()->SetName("root");
  storage.GetRoot()->Initialize();
  storage.SetURL(filename);
  storage.Restore();

  mafVMESurface *surfaceTimeVarying = mafVMESurface::SafeDownCast(storage.GetRoot()->GetFirstChild());
  surfaceTimeVarying->GetOutput()->Update();
  surfaceTimeVarying->Update();

  DummyObserver *observer = new DummyObserver();

  m_OpCut->SetInput(surfaceTimeVarying);
  m_OpCut->SetListener(observer);
  m_OpCut->OpDo();

  CPPUNIT_ASSERT(observer->GetEvent(0)->GetSender()==m_OpCut);
  CPPUNIT_ASSERT(observer->GetEvent(0)->GetVme()==surfaceTimeVarying);
  CPPUNIT_ASSERT(observer->GetEvent(0)->GetId()==VME_REMOVE);

  CPPUNIT_ASSERT(observer->GetEvent(1)->GetSender()==m_OpCut);
  CPPUNIT_ASSERT(observer->GetEvent(1)->GetVme()==storage.GetRoot());
  CPPUNIT_ASSERT(observer->GetEvent(1)->GetId()==VME_SELECTED);

  delete observer;

  m_OpCut->ClipboardClear();
}
//----------------------------------------------------------------------------
void mafOpCutTest::TestOpUndoVMETimeVarying()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_Cut/TestMSFOpCut/TestMSFOpCut.msf";
  mafVMEStorage storage;
  storage.GetRoot()->SetName("root");
  storage.GetRoot()->Initialize();
  storage.SetURL(filename);
  storage.Restore();

  mafVMESurface *surfaceTimeVarying = mafVMESurface::SafeDownCast(storage.GetRoot()->GetFirstChild());
  surfaceTimeVarying->GetOutput()->Update();
  surfaceTimeVarying->Update();
  m_OpCut->SetInput(surfaceTimeVarying);
  m_OpCut->OpDo();

  surfaceTimeVarying->ReparentTo(NULL); //OpDo send only a message.

  CPPUNIT_ASSERT(surfaceTimeVarying->GetParent()==NULL);

  m_OpCut->OpUndo();

  CPPUNIT_ASSERT(surfaceTimeVarying->GetParent()==storage.GetRoot());

  m_OpCut->ClipboardClear();
}
//----------------------------------------------------------------------------
void mafOpCutTest::TestOpUndoVMEWithChildren()
//----------------------------------------------------------------------------
{
  // Added by Losi:
  // This method is added to test the case when the VME to cut has some children with vtk data

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_Cut/TestMSFOpCutWithChildren/TestMSFOpCutWithChildren.msf";
  mafVMEStorage storage;
  storage.GetRoot()->SetName("root");
  storage.GetRoot()->Initialize();
  storage.SetURL(filename);
  storage.Restore();

  mafVMEVolumeGray *volume_parent = mafVMEVolumeGray::SafeDownCast(storage.GetRoot()->GetFirstChild());
  volume_parent->GetOutput()->Update();
  volume_parent->Update();
  m_OpCut->SetInput(volume_parent);
  m_OpCut->OpDo();

  volume_parent->ReparentTo(NULL); //OpDo send only a message.

  CPPUNIT_ASSERT(volume_parent->GetParent()==NULL);

  m_OpCut->OpUndo();

  CPPUNIT_ASSERT(volume_parent->GetParent()==storage.GetRoot());

  m_OpCut->ClipboardClear();
}