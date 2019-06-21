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
#ifndef __albaOpContextStack_H__
#define __albaOpContextStack_H__

#include "albaDefines.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class  albaOp;
struct albaOpContext;
//----------------------------------------------------------------------------
// albaOpContextStack :
//----------------------------------------------------------------------------
/**
  albaOpContextStack represents the stack used for implementing Undo and Redo mechanism 
*/
class ALBA_EXPORT albaOpContextStack
{
public:
         albaOpContextStack();
virtual ~albaOpContextStack(); 
	
  albaOp* Caller(); 
  void	 Clear(); 
  void   Push(albaOp *caller);
  void   Pop();

	void 	 Undo_Push(albaOp* op);
	albaOp* Undo_Pop();
	void 	 Undo_Clear();
	bool 	 Undo_IsEmpty();
	
	void 	 Redo_Push(albaOp* op);
	albaOp* Redo_Pop();
	void 	 Redo_Clear();
	bool 	 Redo_IsEmpty();

protected:
  albaOpContext *m_Context;
};
#endif
