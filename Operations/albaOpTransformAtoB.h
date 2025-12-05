/*=========================================================================
Program:   HTO-Planner
Module:    albaOpTransformAtoB.h
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpTransformAtoB_H__
#define __albaOpTransformAtoB_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDecl.h"
#include "albaOp.h"
#include "albaMatrix.h"
//----------------------------------------------------------------------------
// Forward references :


//----------------------------------------------------------------------------
// Class Name: albaOpTransformAtoB
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpTransformAtoB : public albaOp
{
public:
	/** Constructor. */
	albaOpTransformAtoB(wxString label = "Transform A to B");

	/** Destructor. */
	~albaOpTransformAtoB();

	/** RTTI macro. */
	albaTypeMacro(albaOpTransformAtoB, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Makes the undo for the operation. */
	virtual void OpUndo();

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char** GetIcon();

protected:

	/** Accept All VME excluding current */
	static bool AcceptA(albaVME* node);

	/** Accept All VME excluding current */
	static bool AcceptB(albaVME* node);

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool InternalAccept(albaVME *node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	

	albaMatrix m_InputMatrix;

	albaVME* m_BaseVME;
	albaVME* m_TargerVME;
};
#endif
