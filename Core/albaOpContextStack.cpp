/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpContextStack
 Authors: Silvano Imboden
 
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


#include "albaOp.h"
#include "albaOpStack.h"
#include "albaOpContextStack.h"
//----------------------------------------------------------------------------
//albaOpContext
//----------------------------------------------------------------------------
struct albaOpContext
{
  albaOpStack 		m_ReDo;
  albaOpStack 		m_UnDo;
  albaOp				 *m_Caller;
  albaOpContext *m_Next;
};
//----------------------------------------------------------------------------
albaOpContextStack::albaOpContextStack()
//----------------------------------------------------------------------------
{
  m_Context = NULL;
  Push(NULL);
}	
//----------------------------------------------------------------------------
albaOpContextStack::~albaOpContextStack() 
//----------------------------------------------------------------------------
{
  while(m_Context) Pop();
}	
//----------------------------------------------------------------------------
void albaOpContextStack::Clear() 
//----------------------------------------------------------------------------
{
  while(m_Context) Pop();
  Push(NULL);
}	
//----------------------------------------------------------------------------
albaOp* albaOpContextStack::Caller() 
//----------------------------------------------------------------------------
{
  return m_Context->m_Caller;
}	
//----------------------------------------------------------------------------
void albaOpContextStack::Push(albaOp *caller)
//----------------------------------------------------------------------------
{
  albaOpContext *context = new albaOpContext;
  context->m_Next = m_Context;
  context->m_Caller = caller;  
  m_Context = context;
}	
//----------------------------------------------------------------------------
void albaOpContextStack::Pop()
//----------------------------------------------------------------------------
{
  assert(m_Context);
  albaOpContext *c = m_Context;
  m_Context = m_Context->m_Next;
  c->m_UnDo.Clear();
  c->m_ReDo.Clear();
  delete c;
}	
//----------------------------------------------------------------------------
void 	 albaOpContextStack::Undo_Push(albaOp* op)
//----------------------------------------------------------------------------
{
  assert(m_Context);
  m_Context->m_UnDo.Push(op);  
}	
//----------------------------------------------------------------------------
albaOp* albaOpContextStack::Undo_Pop()
//----------------------------------------------------------------------------
{
  assert(m_Context);
  return m_Context->m_UnDo.Pop();  
}	
//----------------------------------------------------------------------------
void 	 albaOpContextStack::Undo_Clear()
//----------------------------------------------------------------------------
{
  assert(m_Context);
  m_Context->m_UnDo.Clear();  
}	
//----------------------------------------------------------------------------
bool 	 albaOpContextStack::Undo_IsEmpty()
//----------------------------------------------------------------------------
{
  assert(m_Context);
  return m_Context->m_UnDo.IsEmpty();  
}	
//----------------------------------------------------------------------------
void 	 albaOpContextStack::Redo_Push(albaOp* op)
//----------------------------------------------------------------------------
{
  assert(m_Context);
  m_Context->m_ReDo.Push(op);  
}	
//----------------------------------------------------------------------------
albaOp* albaOpContextStack::Redo_Pop()
//----------------------------------------------------------------------------
{
  assert(m_Context);
  return m_Context->m_ReDo.Pop();  
}	
//----------------------------------------------------------------------------
void 	 albaOpContextStack::Redo_Clear()
//----------------------------------------------------------------------------
{
  assert(m_Context);
  m_Context->m_ReDo.Clear();  
}	
//----------------------------------------------------------------------------
bool 	 albaOpContextStack::Redo_IsEmpty()
//----------------------------------------------------------------------------
{
  assert(m_Context);
  return m_Context->m_ReDo.IsEmpty();  
}	
