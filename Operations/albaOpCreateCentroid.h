/*=========================================================================
Program:   HTO-Planner
Module:    albaOpCreateCentroid.h
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

#ifndef __albaOpCreateCentroid_H__
#define __albaOpCreateCentroid_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDecl.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// Forward references :


//----------------------------------------------------------------------------
// Class Name: albaOpCreateCentroid
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpCreateCentroid : public albaOp
{
public:
	/** Constructor. */
	albaOpCreateCentroid(wxString label = "Create Centroid Landmark");

	/** Destructor. */
	~albaOpCreateCentroid();

	/** RTTI macro. */
	albaTypeMacro(albaOpCreateCentroid, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Receive events coming from the user interface.*/
	void OnEvent(albaEventBase *alba_event);
 
protected:

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool InternalAccept(albaVME *node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	
};
#endif
