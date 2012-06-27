/*=========================================================================

 Program: MAF2
 Module: mafOpContextStack
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafOpContextStack_H__
#define __mafOpContextStack_H__

#include "mafDefines.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class  mafOp;
struct mafOpContext;
//----------------------------------------------------------------------------
// mafOpContextStack :
//----------------------------------------------------------------------------
/**
  mafOpContextStack represents the stack used for implementing Undo and Redo mechanism 
*/
class MAF_EXPORT mafOpContextStack
{
public:
         mafOpContextStack();
virtual ~mafOpContextStack(); 
	
  mafOp* Caller(); 
  void	 Clear(); 
  void   Push(mafOp *caller);
  void   Pop();

	void 	 Undo_Push(mafOp* op);
	mafOp* Undo_Pop();
	void 	 Undo_Clear();
	bool 	 Undo_IsEmpty();
	
	void 	 Redo_Push(mafOp* op);
	mafOp* Redo_Pop();
	void 	 Redo_Clear();
	bool 	 Redo_IsEmpty();

protected:
  mafOpContext *m_Context;
};
#endif
