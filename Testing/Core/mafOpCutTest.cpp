/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpCutTest.cpp,v $
Language:  C++
Date:      $Date: 2010-06-04 12:55:32 $
Version:   $Revision: 1.1.2.2 $
Authors:   Matteo Giacomoni
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
class DummyObserver : public mafObserver
//----------------------------------------------------------------------------
{
public:

  DummyObserver() {};
  ~DummyObserver();

  virtual void OnEvent(mafEventBase *maf_event);

  mafEvent* GetEvent(int i){return m_ListEvent[i];};

protected:

  std::vector<mafEvent*> m_ListEvent;
};

//----------------------------------------------------------------------------
void DummyObserver::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    mafEvent *eventToCopy = new mafEvent(e->GetSender(),e->GetId(),e->GetVme());
    m_ListEvent.push_back(eventToCopy);
  }
}
//----------------------------------------------------------------------------
DummyObserver::~DummyObserver()
//----------------------------------------------------------------------------
{
  for(int i=0;i<m_ListEvent.size();i++)
  {
    delete m_ListEvent[i];
  }
  m_ListEvent.clear();
}
//----------------------------------------------------------------------------
void mafOpCutTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpCutTest::setUp()
//----------------------------------------------------------------------------
{
  m_OpCut=new mafOpCut("CUT");
}
//----------------------------------------------------------------------------
void mafOpCutTest::tearDown()
//----------------------------------------------------------------------------
{
  mafDEL(m_OpCut);

  delete wxLog::SetActiveTarget(NULL);
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

  groupChild->SetParent(NULL); //OpDo send only a message.

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

  surfaceTimeVarying->SetParent(NULL); //OpDo send only a message.

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

  volume_parent->SetParent(NULL); //OpDo send only a message.

  CPPUNIT_ASSERT(volume_parent->GetParent()==NULL);

  m_OpCut->OpUndo();

  CPPUNIT_ASSERT(volume_parent->GetParent()==storage.GetRoot());

  m_OpCut->ClipboardClear();
}