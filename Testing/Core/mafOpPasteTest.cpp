/*=========================================================================

 Program: MAF2
 Module: mafOpPasteTest
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
#include "mafOpPasteTest.h"

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
void mafOpPasteTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpPasteTest::setUp()
//----------------------------------------------------------------------------
{
  m_OpPaste=new mafOpPaste("PASTE");
  m_OpPaste->ClipboardClear(); //m_Clipboard is a static variable, so is necessary reinitialize before any test
}
//----------------------------------------------------------------------------
void mafOpPasteTest::tearDown()
//----------------------------------------------------------------------------
{
  mafDEL(m_OpPaste);
}
//----------------------------------------------------------------------------
void mafOpPasteTest::TestAccept()
//----------------------------------------------------------------------------
{
  mafVMEGroup *groupParent;
  mafNEW(groupParent);
  mafVMEGroup *groupChild;
  mafNEW(groupChild);
  CPPUNIT_ASSERT(m_OpPaste->Accept(groupParent)==false);
  m_OpPaste->SetClipboard(groupChild);
  CPPUNIT_ASSERT(m_OpPaste->Accept(NULL)==false);
  CPPUNIT_ASSERT(m_OpPaste->Accept(groupParent)==true);

  mafDEL(groupChild);
  mafDEL(groupParent);
}
//----------------------------------------------------------------------------
void mafOpPasteTest::TestOpDo()
//----------------------------------------------------------------------------
{
  mafVMEGroup *groupParent;
  mafNEW(groupParent);
  mafVMEGroup *groupChild;
  mafNEW(groupChild);
  m_OpPaste->SetInput(groupParent);
  m_OpPaste->SetClipboard(groupChild);

  m_OpPaste->OpDo();

  CPPUNIT_ASSERT(groupChild->GetParent()==groupParent);

  mafDEL(groupChild);
  mafDEL(groupParent);
}
//----------------------------------------------------------------------------
void mafOpPasteTest::TestOpUndo()
//----------------------------------------------------------------------------
{
  mafVMEGroup *groupParent;
  mafNEW(groupParent);
  mafVMEGroup *groupChild;
  mafNEW(groupChild);

  DummyObserver *observer = new DummyObserver();

  m_OpPaste->SetListener(observer);
  m_OpPaste->SetInput(groupParent);
  m_OpPaste->SetClipboard(groupChild);

  m_OpPaste->OpDo();
  m_OpPaste->OpUndo();

  CPPUNIT_ASSERT(m_OpPaste->GetClipboard() == groupChild);
  CPPUNIT_ASSERT(observer->GetEvent(0)->GetSender()==m_OpPaste);
  CPPUNIT_ASSERT(observer->GetEvent(0)->GetVme()==groupChild);
  CPPUNIT_ASSERT(observer->GetEvent(0)->GetId()==VME_REMOVE);

  delete observer;
  mafDEL(groupChild);
  mafDEL(groupParent);
}