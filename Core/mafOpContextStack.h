/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpContextStack.h,v $
  Language:  C++
  Date:      $Date: 2005-04-12 14:06:46 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafOpContextStack_H__
#define __mafOpContextStack_H__
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class  mafOp;
struct mafOpContext;
//----------------------------------------------------------------------------
// mafOpContextStack :
//----------------------------------------------------------------------------
class mafOpContextStack
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
  mafOpContext *m_context;
};
#endif
