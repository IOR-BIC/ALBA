/*=========================================================================

 Program: MAF2
 Module: mafOpStackTest
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
#include "mafOpStackTest.h"

#include "mafOpStack.h"
#include "mafOp.h"

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
void mafOpStackTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpStackTest::setUp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpStackTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpStackTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafOpStack *stack = new mafOpStack;

  delete stack;
}
//----------------------------------------------------------------------------
void mafOpStackTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafOpStack stack;
}
//----------------------------------------------------------------------------
void mafOpStackTest::TestIsEmpty()
//----------------------------------------------------------------------------
{
  mafOpStack stack;
  
  result = stack.IsEmpty();

  TEST_RESULT;

  mafOpDummy *op = new mafOpDummy();
  stack.Push(op);

  result = !stack.IsEmpty();

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafOpStackTest::TestPush()
//----------------------------------------------------------------------------
{
  mafOpStack stack;

  result = stack.IsEmpty();

  TEST_RESULT;

  mafOpDummy *op = new mafOpDummy();
  stack.Push(op);

  result = !stack.IsEmpty();

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafOpStackTest::TestPop()
//----------------------------------------------------------------------------
{
  mafOpStack stack;

  mafOpDummy *op1 = new mafOpDummy();
  stack.Push(op1);
  mafOpDummy *op2 = new mafOpDummy();
  stack.Push(op2);

  result = op2 == stack.Pop();

  TEST_RESULT;

  delete op2;
}
//----------------------------------------------------------------------------
void mafOpStackTest::TestClear()
//----------------------------------------------------------------------------
{
  mafOpStack stack;

  mafOpDummy *op = new mafOpDummy();
  stack.Push(op);

  stack.Clear();

  result = stack.IsEmpty();

  TEST_RESULT;
}