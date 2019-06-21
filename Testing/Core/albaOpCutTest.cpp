/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCutTest
 Authors: Matteo Giacomoni
 
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
#include "albaCoreTests.h"
#include "albaOpCutTest.h"

#include "albaOpSelect.h"
#include "albaVMEGroup.h"
#include "albaEvent.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMESurface.h"
#include "albaVMEFactory.h"
#include "albaVMEVolumeGray.h"

#include <vector>
#include "albaServiceLocator.h"
#include "albaFakeLogicForTest.h"

//----------------------------------------------------------------------------
void albaOpCutTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpCutTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_OpCut=new albaOpCut("CUT");
}
//----------------------------------------------------------------------------
void albaOpCutTest::AfterTest()
//----------------------------------------------------------------------------
{
	albaDEL(m_OpCut);
}
//----------------------------------------------------------------------------
void albaOpCutTest::TestOpDo()
//----------------------------------------------------------------------------
{
  albaVMEGroup *groupChild;
  albaNEW(groupChild);
  albaVMEGroup *groupParent;
  albaNEW(groupParent);

  groupParent->AddChild(groupChild);
  groupParent->Modified();
  groupParent->Update();

  groupChild->Modified();
  groupChild->Update();

	CPPUNIT_ASSERT(groupParent->GetNumberOfChildren() == 1);

	DummyObserver *observer = new DummyObserver();
	albaFakeLogicForTest *logic = (albaFakeLogicForTest*)albaServiceLocator::GetLogicManager();
	logic->ClearCalls();

  m_OpCut->SetInput(groupChild);
  m_OpCut->SetListener(observer);
  m_OpCut->OpDo();

  CPPUNIT_ASSERT(groupParent->GetNumberOfChildren() == 0);
	CPPUNIT_ASSERT(logic->GetCall(0).vme == groupChild);
	CPPUNIT_ASSERT(logic->GetCall(0).testFunction == albaFakeLogicForTest::FKL_VME_REMOVE);

	CPPUNIT_ASSERT(observer->GetEvent(0)->GetSender()==m_OpCut);
	CPPUNIT_ASSERT(observer->GetEvent(0)->GetVme()==groupParent); 
	CPPUNIT_ASSERT(observer->GetEvent(0)->GetId()==VME_SELECTED); 
// 	CPPUNIT_ASSERT(logic->GetCall(1).vme == groupParent);
// 	CPPUNIT_ASSERTlogic->GetCall(1).testFunction == albaFakeLogicForTest::VME_SELECTED);

  m_OpCut->ClipboardClear();

  delete observer;
  albaDEL(groupParent);
  albaDEL(groupChild);
}
//----------------------------------------------------------------------------
void albaOpCutTest::TestOpUndo()
//----------------------------------------------------------------------------
{
  albaVMEGroup *groupChild;
  albaNEW(groupChild);
  albaVMEGroup *groupParent;
  albaNEW(groupParent);

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

  albaDEL(groupParent);
  albaDEL(groupChild);  
}
//----------------------------------------------------------------------------
void albaOpCutTest::TestOpDoVMETimeVarying()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_Cut/TestMSFOpCut/TestMSFOpCut.msf";
  albaVMEStorage storage;
  storage.GetRoot()->SetName("root");
  storage.GetRoot()->Initialize();
  storage.SetURL(filename);
  storage.Restore();

  albaVMESurface *surfaceTimeVarying = albaVMESurface::SafeDownCast(storage.GetRoot()->GetFirstChild());
  surfaceTimeVarying->GetOutput()->Update();
  surfaceTimeVarying->Update();

  DummyObserver *observer = new DummyObserver();
	albaFakeLogicForTest *logic = (albaFakeLogicForTest*)albaServiceLocator::GetLogicManager();
	logic->ClearCalls();

  m_OpCut->SetInput(surfaceTimeVarying);
  m_OpCut->SetListener(observer);
  m_OpCut->OpDo();

	CPPUNIT_ASSERT(logic->GetCall(0).vme == surfaceTimeVarying);
	CPPUNIT_ASSERT(logic->GetCall(0).testFunction == albaFakeLogicForTest::FKL_VME_REMOVE);

  CPPUNIT_ASSERT(observer->GetEvent(0)->GetSender()==m_OpCut);
  CPPUNIT_ASSERT(observer->GetEvent(0)->GetVme()==storage.GetRoot());
  CPPUNIT_ASSERT(observer->GetEvent(0)->GetId()==VME_SELECTED);
	// 	CPPUNIT_ASSERT(logic->GetCall(1).vme == groupParent);
	// 	CPPUNIT_ASSERTlogic->GetCall(1).testFunction == albaFakeLogicForTest::VME_SELECTED);

  delete observer;

  m_OpCut->ClipboardClear();
}
//----------------------------------------------------------------------------
void albaOpCutTest::TestOpUndoVMETimeVarying()
//----------------------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_Cut/TestMSFOpCut/TestMSFOpCut.msf";
  albaVMEStorage storage;
  storage.GetRoot()->SetName("root");
  storage.GetRoot()->Initialize();
  storage.SetURL(filename);
  storage.Restore();

  albaVMESurface *surfaceTimeVarying = albaVMESurface::SafeDownCast(storage.GetRoot()->GetFirstChild());
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
void albaOpCutTest::TestOpUndoVMEWithChildren()
//----------------------------------------------------------------------------
{
  // Added by Losi:
  // This method is added to test the case when the VME to cut has some children with vtk data

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_Cut/TestMSFOpCutWithChildren/TestMSFOpCutWithChildren.msf";
  albaVMEStorage storage;
  storage.GetRoot()->SetName("root");
  storage.GetRoot()->Initialize();
  storage.SetURL(filename);
  storage.Restore();

  albaVMEVolumeGray *volume_parent = albaVMEVolumeGray::SafeDownCast(storage.GetRoot()->GetFirstChild());
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