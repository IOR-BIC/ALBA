/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpContextStack.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-12 14:06:46 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
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


#include "mafOp.h"
#include "mafOpStack.h"
#include "mafOpContextStack.h"
//----------------------------------------------------------------------------
//mafOpContext
//----------------------------------------------------------------------------
struct mafOpContext
{
  mafOpStack 		m_redo;
  mafOpStack 		m_undo;
  mafOp				 *m_caller;
  mafOpContext *m_next;
};
//----------------------------------------------------------------------------
mafOpContextStack::mafOpContextStack()
//----------------------------------------------------------------------------
{
  m_context = NULL;
  Push(NULL);
}	
//----------------------------------------------------------------------------
mafOpContextStack::~mafOpContextStack() 
//----------------------------------------------------------------------------
{
  while(m_context) Pop();
}	
//----------------------------------------------------------------------------
void mafOpContextStack::Clear() 
//----------------------------------------------------------------------------
{
  while(m_context) Pop();
  Push(NULL);
}	
//----------------------------------------------------------------------------
mafOp* mafOpContextStack::Caller() 
//----------------------------------------------------------------------------
{
  return m_context->m_caller;
}	
//----------------------------------------------------------------------------
void mafOpContextStack::Push(mafOp *caller)
//----------------------------------------------------------------------------
{
  mafOpContext *context = new mafOpContext;
  context->m_next = m_context;
  context->m_caller = caller;  
  m_context = context;
}	
//----------------------------------------------------------------------------
void mafOpContextStack::Pop()
//----------------------------------------------------------------------------
{
  assert(m_context);
  mafOpContext *c = m_context;
  m_context = m_context->m_next;
  c->m_undo.Clear();
  c->m_redo.Clear();
  delete c;
}	
//----------------------------------------------------------------------------
void 	 mafOpContextStack::Undo_Push(mafOp* op)
//----------------------------------------------------------------------------
{
  assert(m_context);
  m_context->m_undo.Push(op);  
}	
//----------------------------------------------------------------------------
mafOp* mafOpContextStack::Undo_Pop()
//----------------------------------------------------------------------------
{
  assert(m_context);
  return m_context->m_undo.Pop();  
}	
//----------------------------------------------------------------------------
void 	 mafOpContextStack::Undo_Clear()
//----------------------------------------------------------------------------
{
  assert(m_context);
  m_context->m_undo.Clear();  
}	
//----------------------------------------------------------------------------
bool 	 mafOpContextStack::Undo_IsEmpty()
//----------------------------------------------------------------------------
{
  assert(m_context);
  return m_context->m_undo.IsEmpty();  
}	
//----------------------------------------------------------------------------
void 	 mafOpContextStack::Redo_Push(mafOp* op)
//----------------------------------------------------------------------------
{
  assert(m_context);
  m_context->m_redo.Push(op);  
}	
//----------------------------------------------------------------------------
mafOp* mafOpContextStack::Redo_Pop()
//----------------------------------------------------------------------------
{
  assert(m_context);
  return m_context->m_redo.Pop();  
}	
//----------------------------------------------------------------------------
void 	 mafOpContextStack::Redo_Clear()
//----------------------------------------------------------------------------
{
  assert(m_context);
  m_context->m_redo.Clear();  
}	
//----------------------------------------------------------------------------
bool 	 mafOpContextStack::Redo_IsEmpty()
//----------------------------------------------------------------------------
{
  assert(m_context);
  return m_context->m_redo.IsEmpty();  
}	
