/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpContextStackTest.cpp,v $
Language:  C++
Date:      $Date: 2008-03-06 11:58:39 $
Version:   $Revision: 1.2 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafOpContextStackTest.h"
#include "mafOpContextStack.h"
#include <cppunit/config/SourcePrefix.h>

#include "mafOpReparentTo.h"

//-------------------------------------------------------------------------
void mafOpContextStackTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  mafOpContextStack *cs=new mafOpContextStack();
  delete cs;
   
  CPPUNIT_ASSERT(true);
 
}
//-------------------------------------------------------------------------
void mafOpContextStackTest::Test_Caller_Clear_Push_Pop()
//-------------------------------------------------------------------------
{
  //create stack contest
  mafOpContextStack *cs=new mafOpContextStack();

  //create operation
  mafOpReparentTo *op = new mafOpReparentTo();
  op->TestModeOn();

  mafOpReparentTo *op2 = new mafOpReparentTo();
  op2->TestModeOn();


  bool result = false;
  //test on push
  cs->Push(op);
  result = cs->Caller() == op;
  CPPUNIT_ASSERT(result);

  result = false;
  cs->Push(op2);
  result = cs->Caller() == op2;
  CPPUNIT_ASSERT(result);
  //end test on push

  //test on pop
  result = false;
  cs->Pop();
  result = cs->Caller() == op;
  CPPUNIT_ASSERT(result);
  //end test on pop

  //test on clear
  result = false;
  cs->Clear();
  result = cs->Caller() == NULL;
  CPPUNIT_ASSERT(result);
  //end test on clear

  delete op;
  delete op2;
  delete cs;

  
}

//-------------------------------------------------------------------------
void mafOpContextStackTest::Test_UndoClear_UndoPush_UndoPop_UndoIsEmpty()
//-------------------------------------------------------------------------
{
  //create stack contest
  mafOpContextStack *cs=new mafOpContextStack();

  //create operation
  mafOpReparentTo *op = new mafOpReparentTo();
  op->TestModeOn();

  mafOpReparentTo *op2 = new mafOpReparentTo();
  op2->TestModeOn();
  
  bool result;

  //test on undo_push and undo_pop
  result = false;
  cs->Undo_Push(op);
  cs->Undo_Push(op2);
  mafOpReparentTo *control2 = (mafOpReparentTo *)cs->Undo_Pop();
  mafOpReparentTo *control = (mafOpReparentTo *)cs->Undo_Pop();
  result = (op == control) && (op2 = control2); 
  CPPUNIT_ASSERT(result);
  //end test on undo_push and undo_pop

  //test on undo_clear and undo_isempty
  result = false;
  cs->Undo_Push(op);
  cs->Undo_Push(op2);
  cs->Undo_Clear();
  result = cs->Undo_IsEmpty();
  CPPUNIT_ASSERT(result);
  //end test undo_clear and undo_isempty

  delete cs;
}

//-------------------------------------------------------------------------
void mafOpContextStackTest::Test_RedoClear_RedoPush_RedoPop_RedoIsEmpty()
//-------------------------------------------------------------------------
{
  //create stack contest
  mafOpContextStack *cs=new mafOpContextStack();

  //create operation
  mafOpReparentTo *op = new mafOpReparentTo();
  op->TestModeOn();

  mafOpReparentTo *op2 = new mafOpReparentTo();
  op2->TestModeOn();

  bool result;

  //test on redo_push and redo_pop
  result = false;
  cs->Redo_Push(op);
  cs->Redo_Push(op2);
  mafOpReparentTo *control2 = (mafOpReparentTo *)cs->Redo_Pop();
  mafOpReparentTo *control = (mafOpReparentTo *)cs->Redo_Pop();
  result = (op == control) && (op2 = control2); 
  CPPUNIT_ASSERT(result);
  //end test on redo_push and redo_pop

  //test on redo_clear and redo_isempty
  result = false;
  cs->Redo_Push(op);
  cs->Redo_Push(op2);
  cs->Redo_Clear();
  result = cs->Redo_IsEmpty();
  CPPUNIT_ASSERT(result);
  //end test redo_clear and redo_isempty

  delete cs;
  
}
