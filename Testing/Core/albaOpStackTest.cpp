/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpStackTest
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

#include "albaCoreTests.h"
#include <cppunit/config/SourcePrefix.h>
#include "albaOpStackTest.h"

#include "albaOpStack.h"
#include "albaOp.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);


//----------------------------------------------------------------------------
void albaOpStackTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpStackTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaOpStack *stack = new albaOpStack;

  delete stack;
}
//----------------------------------------------------------------------------
void albaOpStackTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaOpStack stack;
}
//----------------------------------------------------------------------------
void albaOpStackTest::TestIsEmpty()
//----------------------------------------------------------------------------
{
  albaOpStack stack;
  
  result = stack.IsEmpty();

  TEST_RESULT;

  albaOpDummyHelper *op = new albaOpDummyHelper();
  stack.Push(op);

  result = !stack.IsEmpty();

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaOpStackTest::TestPush()
//----------------------------------------------------------------------------
{
  albaOpStack stack;

  result = stack.IsEmpty();

  TEST_RESULT;

  albaOpDummyHelper *op = new albaOpDummyHelper();
  stack.Push(op);

  result = !stack.IsEmpty();

  TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaOpStackTest::TestPop()
//----------------------------------------------------------------------------
{
  albaOpStack stack;

  albaOpDummyHelper *op1 = new albaOpDummyHelper();
  stack.Push(op1);
  albaOpDummyHelper *op2 = new albaOpDummyHelper();
  stack.Push(op2);

  result = op2 == stack.Pop();

  TEST_RESULT;

  delete op2;
}
//----------------------------------------------------------------------------
void albaOpStackTest::TestClear()
//----------------------------------------------------------------------------
{
  albaOpStack stack;

  albaOpDummyHelper *op = new albaOpDummyHelper();
  stack.Push(op);

  stack.Clear();

  result = stack.IsEmpty();

  TEST_RESULT;
}