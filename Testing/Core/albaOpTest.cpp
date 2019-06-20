/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpTest
 Authors: Paolo Quadrani
 
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
#include "albaOpTest.h"

#include "albaSmartPointer.h"
#include "albaOp.h"
#include "albaVMEGroup.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaGUI.h"

#include <iostream>
#define TEST_RESULT CPPUNIT_ASSERT(result);


//----------------------------------------------------------------------------
void albaOpTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_Op = new albaOp();
}
//----------------------------------------------------------------------------
void albaOpTest::AfterTest()
//----------------------------------------------------------------------------
{
  if(m_Op != NULL) m_Op->Delete();
}
//----------------------------------------------------------------------------
void albaOpTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaOp *op;
  op = new albaOp();
  op->Delete();
}
//----------------------------------------------------------------------------
void albaOpTest::TestSetListener()
//----------------------------------------------------------------------------
{
  albaOp *op;
  op = new albaOp("DummyTest");
  
  m_Op->SetListener(op);

  op->Delete();
}
//----------------------------------------------------------------------------
void albaOpTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  albaOpDummyHelper *op;
  op = new albaOpDummyHelper("DummyTest");
  op->SetListener(m_Op);
  albaEvent a;
  op->OnEvent(&a);

  result = true;

  TEST_RESULT
    op->Delete();
}
//----------------------------------------------------------------------------
void albaOpTest::TestGetType()
//----------------------------------------------------------------------------
{
  albaOpDummyHelper *op;
  op = new albaOpDummyHelper("DummyTest");

  result = op->GetType() == OPTYPE_OP;

  TEST_RESULT
  op->Delete();
}
//----------------------------------------------------------------------------
void albaOpTest::TestCopy()
//----------------------------------------------------------------------------
{
  albaOp*op;
  op = new albaOp("DummyTest");

  result = op->Copy() == NULL;

  TEST_RESULT

  op->Delete();
}
//----------------------------------------------------------------------------
void albaOpTest::TestOpRun()
//----------------------------------------------------------------------------
{
  m_Op->OpRun();
  result = true;
  TEST_RESULT
}
//----------------------------------------------------------------------------
void albaOpTest::TestSetParameters()
//----------------------------------------------------------------------------
{
  int *pointer;
  pointer = new int(10);
  m_Op->SetParameters(pointer);
  result = true;
  TEST_RESULT
  delete pointer;
}
//----------------------------------------------------------------------------
void albaOpTest::TestOpDo()
//----------------------------------------------------------------------------
{
  m_Op->OpDo();
  result = true;
  TEST_RESULT
}
//----------------------------------------------------------------------------
void albaOpTest::TestOpUndo()
//----------------------------------------------------------------------------
{
  m_Op->OpUndo();
  result = true;
  TEST_RESULT
}
//----------------------------------------------------------------------------
void albaOpTest::TestGetGui()
//----------------------------------------------------------------------------
{
  result = m_Op->GetGui() == NULL;
  TEST_RESULT
}
//----------------------------------------------------------------------------
void albaOpTest::TestSetGetInput()
//----------------------------------------------------------------------------
{
  albaVMEGroup *group;
  albaNEW(group);

  m_Op->SetInput(group);
  result = m_Op->GetInput() == group;
  TEST_RESULT

  albaDEL(group);
}
//----------------------------------------------------------------------------
void albaOpTest::TestSetGetOutput()
//----------------------------------------------------------------------------
{
  albaVMEGroup *group;
  albaNEW(group);

  m_Op->SetOutput(group);
  result = m_Op->GetOutput() == group;
  TEST_RESULT

  albaDEL(group);
}
//----------------------------------------------------------------------------
void albaOpTest::TestAccept()
//----------------------------------------------------------------------------
{
  albaVMEGroup *group;
  albaNEW(group);
  result = !m_Op->Accept(group);
  TEST_RESULT
  albaDEL(group);
}
//----------------------------------------------------------------------------
void albaOpTest::TestCanUndo()
//----------------------------------------------------------------------------
{
  albaOpDummyHelper *op;
  op = new albaOpDummyHelper("DummyTest",true);
  result = op->CanUndo();
  TEST_RESULT
  op->Delete();
}
//----------------------------------------------------------------------------
void albaOpTest::TestIsInputPreserving()
//----------------------------------------------------------------------------
{
  albaOpDummyHelper *op;
  op = new albaOpDummyHelper("DummyTest",true,OPTYPE_OP,true);
  result = op->CanUndo();
  TEST_RESULT
  op->Delete();
}
//----------------------------------------------------------------------------
void albaOpTest::TestOkEnabled()
//----------------------------------------------------------------------------
{
  result = !m_Op->OkEnabled();
  TEST_RESULT
}
//----------------------------------------------------------------------------
void albaOpTest::TestForceStopWithOk()
//----------------------------------------------------------------------------
{
  m_Op->ForceStopWithOk();
  result = true;
  TEST_RESULT

}
//----------------------------------------------------------------------------
void albaOpTest::TestForceStopWithCancel()
//----------------------------------------------------------------------------
{
  m_Op->ForceStopWithCancel();
  result = true;
  TEST_RESULT
}
//----------------------------------------------------------------------------
void albaOpTest::TestIsCompatible()
//----------------------------------------------------------------------------
{
  result = m_Op->IsCompatible((long)0xFFFF);
  TEST_RESULT
}
//----------------------------------------------------------------------------
void albaOpTest::TestGetActions()
//----------------------------------------------------------------------------
{
  result = m_Op->GetActions() == NULL;
  TEST_RESULT
}
//----------------------------------------------------------------------------
void albaOpTest::TestSetGetMouse()
//----------------------------------------------------------------------------
{
  albaDeviceButtonsPadMouse *mouse;
  mouse = albaDeviceButtonsPadMouse::New();
  m_Op->SetMouse(mouse);
  result = m_Op->GetMouse() == mouse;
  TEST_RESULT
  mouse->Delete();
}
//----------------------------------------------------------------------------
void albaOpTest::TestTestModeOn()
//----------------------------------------------------------------------------
{
  m_Op->TestModeOn();
  result = m_Op->GetTestMode() == true;
  TEST_RESULT
}
//----------------------------------------------------------------------------
void albaOpTest::TestTestModeOff()
//----------------------------------------------------------------------------
{
  m_Op->TestModeOff();
  result = m_Op->GetTestMode() == false;
  TEST_RESULT
}
//----------------------------------------------------------------------------
void albaOpTest::TestSetGetCanundo()
//----------------------------------------------------------------------------
{
  m_Op->SetCanundo(false);
  result = m_Op->GetCanundo() == false;
  TEST_RESULT
}
