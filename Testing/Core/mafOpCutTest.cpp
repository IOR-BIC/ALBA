/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpCutTest.cpp,v $
Language:  C++
Date:      $Date: 2008-02-19 10:18:20 $
Version:   $Revision: 1.1 $
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

  mafDEL(groupParent);
  mafDEL(groupChild);
}
