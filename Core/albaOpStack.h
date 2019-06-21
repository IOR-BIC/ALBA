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
#ifndef __albaOpStack_H__
#define __albaOpStack_H__
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaOp;
//----------------------------------------------------------------------------
// albaOpStack :
//----------------------------------------------------------------------------
/**
*/
class ALBA_EXPORT albaOpStack
{
public:
				albaOpStack();
			 ~albaOpStack(); 
	
	/** Return true is the stack is empty */
	bool 		IsEmpty();

  /** Insert op on top od the stack */
	void 		Push(albaOp* op);
	
  /** Return the op on the top of the stack, and remove it */
	albaOp*	Pop();
	
  /** Empty the stack and call delete on every inserted op */
	void 		Clear();

protected:
	albaOp	 *m_Stack;
};
#endif














