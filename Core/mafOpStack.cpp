/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpStack.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-26 11:08:35 $
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

//----------------------------------------------------------------------------
mafOpStack::mafOpStack()
//----------------------------------------------------------------------------
{
	m_stack = NULL;
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
  return (m_stack == NULL);
}
//----------------------------------------------------------------------------
void mafOpStack::Push(mafOp* op)   
//----------------------------------------------------------------------------
{
  op->m_Next = m_stack;
  m_stack = op;
}
//----------------------------------------------------------------------------
mafOp* mafOpStack::Pop()   
//----------------------------------------------------------------------------
{
  mafOp* op = m_stack;
  m_stack = op->m_Next;
  return op;
}
//----------------------------------------------------------------------------
void mafOpStack::Clear()   
//----------------------------------------------------------------------------
{
  while(m_stack)
    delete Pop();
}
