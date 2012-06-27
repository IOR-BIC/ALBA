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
#ifndef __mafOpStack_H__
#define __mafOpStack_H__
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafOp;
//----------------------------------------------------------------------------
// mafOpStack :
//----------------------------------------------------------------------------
/**
*/
class MAF_EXPORT mafOpStack
{
public:
				mafOpStack();
			 ~mafOpStack(); 
	
	/** Return true is the stack is empty */
	bool 		IsEmpty();

  /** Insert op on top od the stack */
	void 		Push(mafOp* op);
	
  /** Return the op on the top of the stack, and remove it */
	mafOp*	Pop();
	
  /** Empty the stack and call delete on every inserted op */
	void 		Clear();

protected:
	mafOp	 *m_Stack;
};
#endif














