/*=========================================================================
Program:   HTO-Planner
Module:    albaOpEmpty.h
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

#ifndef __albaOpEmpty_H__
#define __albaOpEmpty_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDecl.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// Forward references :


//----------------------------------------------------------------------------
// Class Name: albaOpEmpty
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpEmpty : public albaOp
{
public:
	/** Constructor. */
	albaOpEmpty(wxString label = "Op Empty");

	/** Destructor. */
	~albaOpEmpty();

	/** RTTI macro. */
	albaTypeMacro(albaOpEmpty, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Receive events coming from the user interface.*/
	void OnEvent(albaEventBase *alba_event);
  
	/** Return an xpm-icon that can be used to represent this operation */
	virtual char** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool InternalAccept(albaVME *node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	

	/** Create the Operation GUI */
	virtual void CreateGui();
};
#endif
