/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpTest.cpp,v $
Language:  C++
Date:      $Date: 2009-05-25 14:49:40 $
Version:   $Revision: 1.2.2.1 $
Authors:   Paolo Quadrani
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
#include "mafOpTest.h"

#include "mafSmartPointer.h"
#include "mafOp.h"
#include "mafVMEGroup.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafGUI.h"

#include <iostream>
#define TEST_RESULT CPPUNIT_ASSERT(result);

//-------------------------------------------------------------------------
/** class for testing re-parenting. */
class mafOpDummy: public mafOp
//-------------------------------------------------------------------------
{
public:
  mafOpDummy(wxString label = "DummyOp",  bool canundo = false, int opType = OPTYPE_OP, bool inputPreserving = false);
};
mafOpDummy::mafOpDummy(wxString label,  bool canundo, int opType, bool inputPreserving)
{
  
  m_Canundo = canundo;
  m_OpType = opType;
  m_InputPreserving = inputPreserving;
}

//----------------------------------------------------------------------------
void mafOpTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
  m_Op = NULL;
  m_Op = new mafOp();
}
//----------------------------------------------------------------------------
void mafOpTest::tearDown()
//----------------------------------------------------------------------------
{
  if(m_Op != NULL) m_Op->Delete();
}
//----------------------------------------------------------------------------
void mafOpTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafOp *op;
  op = new mafOp();
  op->Delete();
}
//----------------------------------------------------------------------------
void mafOpTest::TestSetListener()
//----------------------------------------------------------------------------
{
  mafOp *op;
  op = new mafOp("DummyTest");
  
  m_Op->SetListener(op);

  op->Delete();
}
//----------------------------------------------------------------------------
void mafOpTest::TestOnEvent()
//----------------------------------------------------------------------------
{
  mafOpDummy *op;
  op = new mafOpDummy("DummyTest");
  op->SetListener(m_Op);
  mafEvent a;
  op->OnEvent(&a);

  result = true;

  TEST_RESULT
    op->Delete();
}
//----------------------------------------------------------------------------
void mafOpTest::TestGetType()
//----------------------------------------------------------------------------
{
  mafOpDummy *op;
  op = new mafOpDummy("DummyTest");

  result = op->GetType() == OPTYPE_OP;

  TEST_RESULT
  op->Delete();
}
//----------------------------------------------------------------------------
void mafOpTest::TestCopy()
//----------------------------------------------------------------------------
{
  mafOp*op;
  op = new mafOp("DummyTest");

  result = op->Copy() == NULL;

  TEST_RESULT

  op->Delete();
}
//----------------------------------------------------------------------------
void mafOpTest::TestOpRun()
//----------------------------------------------------------------------------
{
  m_Op->OpRun();
  result = true;
  TEST_RESULT
}
//----------------------------------------------------------------------------
void mafOpTest::TestSetParameters()
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
void mafOpTest::TestOpDo()
//----------------------------------------------------------------------------
{
  m_Op->OpDo();
  result = true;
  TEST_RESULT
}
//----------------------------------------------------------------------------
void mafOpTest::TestOpUndo()
//----------------------------------------------------------------------------
{
  m_Op->OpUndo();
  result = true;
  TEST_RESULT
}
//----------------------------------------------------------------------------
void mafOpTest::TestGetGui()
//----------------------------------------------------------------------------
{
  result = m_Op->GetGui() == NULL;
  TEST_RESULT
}
//----------------------------------------------------------------------------
void mafOpTest::TestSetGetInput()
//----------------------------------------------------------------------------
{
  mafVMEGroup *group;
  mafNEW(group);

  m_Op->SetInput(group);
  result = m_Op->GetInput() == group;
  TEST_RESULT

  mafDEL(group);
}
//----------------------------------------------------------------------------
void mafOpTest::TestSetGetOutput()
//----------------------------------------------------------------------------
{
  mafVMEGroup *group;
  mafNEW(group);

  m_Op->SetOutput(group);
  result = m_Op->GetOutput() == group;
  TEST_RESULT

  mafDEL(group);
}
//----------------------------------------------------------------------------
void mafOpTest::TestAccept()
//----------------------------------------------------------------------------
{
  mafVMEGroup *group;
  mafNEW(group);
  result = !m_Op->Accept(group);
  TEST_RESULT
  mafDEL(group);
}
//----------------------------------------------------------------------------
void mafOpTest::TestCanUndo()
//----------------------------------------------------------------------------
{
  mafOpDummy *op;
  op = new mafOpDummy("DummyTest",true);
  result = op->CanUndo();
  TEST_RESULT
  op->Delete();
}
//----------------------------------------------------------------------------
void mafOpTest::TestIsInputPreserving()
//----------------------------------------------------------------------------
{
  mafOpDummy *op;
  op = new mafOpDummy("DummyTest",true,OPTYPE_OP,true);
  result = op->CanUndo();
  TEST_RESULT
  op->Delete();
}
//----------------------------------------------------------------------------
void mafOpTest::TestOkEnabled()
//----------------------------------------------------------------------------
{
  result = !m_Op->OkEnabled();
  TEST_RESULT
}
//----------------------------------------------------------------------------
void mafOpTest::TestForceStopWithOk()
//----------------------------------------------------------------------------
{
  m_Op->ForceStopWithOk();
  result = true;
  TEST_RESULT

}
//----------------------------------------------------------------------------
void mafOpTest::TestForceStopWithCancel()
//----------------------------------------------------------------------------
{
  m_Op->ForceStopWithCancel();
  result = true;
  TEST_RESULT
}
//----------------------------------------------------------------------------
void mafOpTest::TestIsCompatible()
//----------------------------------------------------------------------------
{
  result = m_Op->IsCompatible((long)0xFFFF);
  TEST_RESULT
}
//----------------------------------------------------------------------------
void mafOpTest::TestGetActions()
//----------------------------------------------------------------------------
{
  result = m_Op->GetActions() == NULL;
  TEST_RESULT
}
//----------------------------------------------------------------------------
void mafOpTest::TestSetGetMouse()
//----------------------------------------------------------------------------
{
  mafDeviceButtonsPadMouse *mouse;
  mouse = mafDeviceButtonsPadMouse::New();
  m_Op->SetMouse(mouse);
  result = m_Op->GetMouse() == mouse;
  TEST_RESULT
  mouse->Delete();
}
//----------------------------------------------------------------------------
void mafOpTest::TestCollaborate()
//----------------------------------------------------------------------------
{
  result = true;
  m_Op->Collaborate(result);
  TEST_RESULT
}
//----------------------------------------------------------------------------
void mafOpTest::TestTestModeOn()
//----------------------------------------------------------------------------
{
  m_Op->TestModeOn();
  result = m_Op->GetTestMode() == true;
  TEST_RESULT
}
//----------------------------------------------------------------------------
void mafOpTest::TestTestModeOff()
//----------------------------------------------------------------------------
{
  m_Op->TestModeOff();
  result = m_Op->GetTestMode() == false;
  TEST_RESULT
}
//----------------------------------------------------------------------------
void mafOpTest::TestSetGetCanundo()
//----------------------------------------------------------------------------
{
  m_Op->SetCanundo(false);
  result = m_Op->GetCanundo() == false;
  TEST_RESULT
}
