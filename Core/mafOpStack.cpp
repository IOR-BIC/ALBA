/*=========================================================================

 Program: MAF2
 Module: mafOpStack
 Authors: Silvano Imboden
 
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


#include "mafOp.h"
#include "mafOpStack.h"

//----------------------------------------------------------------------------
mafOpStack::mafOpStack()
//----------------------------------------------------------------------------
{
	m_Stack = NULL;
}
//----------------------------------------------------------------------------
mafOpStack::~mafOpStack( ) 
/**  calls delete on every inserted op */
//----------------------------------------------------------------------------
{
  Clear();
}
//----------------------------------------------------------------------------
bool mafOpStack::IsEmpty()   
//----------------------------------------------------------------------------
{
  return (m_Stack == NULL);
}
//----------------------------------------------------------------------------
void mafOpStack::Push(mafOp* op)   
//----------------------------------------------------------------------------
{
  op->m_Next = m_Stack;
  m_Stack = op;
}
//----------------------------------------------------------------------------
mafOp* mafOpStack::Pop()   
//----------------------------------------------------------------------------
{
  mafOp* op = m_Stack;
  m_Stack = op->m_Next;
  return op;
}
//----------------------------------------------------------------------------
void mafOpStack::Clear()   
//----------------------------------------------------------------------------
{
  while(m_Stack)
    delete Pop();
}
