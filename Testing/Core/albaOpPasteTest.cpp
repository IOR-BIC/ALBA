/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpPasteTest
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
#include "albaOpPasteTest.h"

#include "albaOpSelect.h"
#include "albaVMEGroup.h"
#include "albaEvent.h"

#include <vector>
#include "albaServiceLocator.h"
#include "albaFakeLogicForTest.h"


//----------------------------------------------------------------------------
void albaOpPasteTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpPasteTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_OpPaste=new albaOpPaste("PASTE");
  m_OpPaste->ClipboardClear(); //m_Clipboard is a static variable, so is necessary reinitialize before any test
}
//----------------------------------------------------------------------------
void albaOpPasteTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_OpPaste);
}
//----------------------------------------------------------------------------
void albaOpPasteTest::TestAccept()
//----------------------------------------------------------------------------
{
  albaVMEGroup *groupParent;
  albaNEW(groupParent);
  albaVMEGroup *groupChild;
  albaNEW(groupChild);

  CPPUNIT_ASSERT(m_OpPaste->Accept(groupParent)==false);
  m_OpPaste->SetClipboard(groupChild);
  CPPUNIT_ASSERT(m_OpPaste->Accept(NULL)==false);
  CPPUNIT_ASSERT(m_OpPaste->Accept(groupParent)==true);

  albaDEL(groupChild);
  albaDEL(groupParent);
}
//----------------------------------------------------------------------------
void albaOpPasteTest::TestOpDo()
//----------------------------------------------------------------------------
{
  albaVMEGroup *groupParent;
  albaNEW(groupParent);
	albaVMEGroup *groupChild;
	albaNEW(groupChild);
	albaVMEGroup *groupOldParent;
	albaNEW(groupOldParent);

	m_OpPaste->SetSelectionParent(groupOldParent);
	m_OpPaste->SetInput(groupParent);
  m_OpPaste->SetClipboard(groupChild);

  m_OpPaste->OpDo();

  CPPUNIT_ASSERT(groupChild->GetParent()==groupParent);

  albaDEL(groupChild);
  albaDEL(groupParent);
}
//----------------------------------------------------------------------------
void albaOpPasteTest::TestOpUndo()
//----------------------------------------------------------------------------
{
  albaVMEGroup *groupParent;
  albaNEW(groupParent);
  albaVMEGroup *groupChild;
  albaNEW(groupChild);

  DummyObserver *observer = new DummyObserver();
	albaFakeLogicForTest *logic = (albaFakeLogicForTest*)albaServiceLocator::GetLogicManager();
	logic->ClearCalls();

  m_OpPaste->SetListener(observer);
  m_OpPaste->SetInput(groupParent);
  m_OpPaste->SetClipboard(groupChild);

  m_OpPaste->OpDo();
  m_OpPaste->OpUndo();

	CPPUNIT_ASSERT(logic->GetCall(1).vme == groupChild);
	CPPUNIT_ASSERT(logic->GetCall(1).testFunction == albaFakeLogicForTest::FKL_VME_REMOVE);

  delete observer;
  albaDEL(groupChild);
  albaDEL(groupParent);
}