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

#include "mafCoreTests.h"
#include <cppunit/config/SourcePrefix.h>
#include "mafOpStackTest.h"

#include "mafOpStack.h"
#include "mafOp.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);


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

  mafOpDummyHelper *op = new mafOpDummyHelper();
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

  mafOpDummyHelper *op = new mafOpDummyHelper();
  stack.Push(op);

  result = !stack.IsEmpty();

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafOpStackTest::TestPop()
//----------------------------------------------------------------------------
{
  mafOpStack stack;

  mafOpDummyHelper *op1 = new mafOpDummyHelper();
  stack.Push(op1);
  mafOpDummyHelper *op2 = new mafOpDummyHelper();
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

  mafOpDummyHelper *op = new mafOpDummyHelper();
  stack.Push(op);

  stack.Clear();

  result = stack.IsEmpty();

  TEST_RESULT;
}