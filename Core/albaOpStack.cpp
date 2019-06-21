/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpStack
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

//----------------------------------------------------------------------------
albaOpStack::albaOpStack()
//----------------------------------------------------------------------------
{
	m_Stack = NULL;
}
//----------------------------------------------------------------------------
albaOpStack::~albaOpStack( ) 
/**  calls delete on every inserted op */
//----------------------------------------------------------------------------
{
  Clear();
}
//----------------------------------------------------------------------------
bool albaOpStack::IsEmpty()   
//----------------------------------------------------------------------------
{
  return (m_Stack == NULL);
}
//----------------------------------------------------------------------------
void albaOpStack::Push(albaOp* op)   
//----------------------------------------------------------------------------
{
  op->m_Next = m_Stack;
  m_Stack = op;
}
//----------------------------------------------------------------------------
albaOp* albaOpStack::Pop()   
//----------------------------------------------------------------------------
{
  albaOp* op = m_Stack;
  m_Stack = op->m_Next;
  return op;
}
//----------------------------------------------------------------------------
void albaOpStack::Clear()   
//----------------------------------------------------------------------------
{
  while(m_Stack)
    delete Pop();
}
