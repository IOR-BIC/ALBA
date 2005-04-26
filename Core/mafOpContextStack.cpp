/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpContextStack.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-26 11:08:34 $
  Version:   $Revision: 1.2 $
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
  mafOpStack 		m_ReDo;
  mafOpStack 		m_UnDo;
  mafOp				 *m_Caller;
  mafOpContext *m_Next;
};
//----------------------------------------------------------------------------
mafOpContextStack::mafOpContextStack()
//----------------------------------------------------------------------------
{
  m_Context = NULL;
  Push(NULL);
}	
//----------------------------------------------------------------------------
mafOpContextStack::~mafOpContextStack() 
//----------------------------------------------------------------------------
{
  while(m_Context) Pop();
}	
//----------------------------------------------------------------------------
void mafOpContextStack::Clear() 
//----------------------------------------------------------------------------
{
  while(m_Context) Pop();
  Push(NULL);
}	
//----------------------------------------------------------------------------
mafOp* mafOpContextStack::Caller() 
//----------------------------------------------------------------------------
{
  return m_Context->m_Caller;
}	
//----------------------------------------------------------------------------
void mafOpContextStack::Push(mafOp *caller)
//----------------------------------------------------------------------------
{
  mafOpContext *context = new mafOpContext;
  context->m_Next = m_Context;
  context->m_Caller = caller;  
  m_Context = context;
}	
//----------------------------------------------------------------------------
void mafOpContextStack::Pop()
//----------------------------------------------------------------------------
{
  assert(m_Context);
  mafOpContext *c = m_Context;
  m_Context = m_Context->m_Next;
  c->m_UnDo.Clear();
  c->m_ReDo.Clear();
  delete c;
}	
//----------------------------------------------------------------------------
void 	 mafOpContextStack::Undo_Push(mafOp* op)
//----------------------------------------------------------------------------
{
  assert(m_Context);
  m_Context->m_UnDo.Push(op);  
}	
//----------------------------------------------------------------------------
mafOp* mafOpContextStack::Undo_Pop()
//----------------------------------------------------------------------------
{
  assert(m_Context);
  return m_Context->m_UnDo.Pop();  
}	
//----------------------------------------------------------------------------
void 	 mafOpContextStack::Undo_Clear()
//----------------------------------------------------------------------------
{
  assert(m_Context);
  m_Context->m_UnDo.Clear();  
}	
//----------------------------------------------------------------------------
bool 	 mafOpContextStack::Undo_IsEmpty()
//----------------------------------------------------------------------------
{
  assert(m_Context);
  return m_Context->m_UnDo.IsEmpty();  
}	
//----------------------------------------------------------------------------
void 	 mafOpContextStack::Redo_Push(mafOp* op)
//----------------------------------------------------------------------------
{
  assert(m_Context);
  m_Context->m_ReDo.Push(op);  
}	
//----------------------------------------------------------------------------
mafOp* mafOpContextStack::Redo_Pop()
//----------------------------------------------------------------------------
{
  assert(m_Context);
  return m_Context->m_ReDo.Pop();  
}	
//----------------------------------------------------------------------------
void 	 mafOpContextStack::Redo_Clear()
//----------------------------------------------------------------------------
{
  assert(m_Context);
  m_Context->m_ReDo.Clear();  
}	
//----------------------------------------------------------------------------
bool 	 mafOpContextStack::Redo_IsEmpty()
//----------------------------------------------------------------------------
{
  assert(m_Context);
  return m_Context->m_ReDo.IsEmpty();  
}	
