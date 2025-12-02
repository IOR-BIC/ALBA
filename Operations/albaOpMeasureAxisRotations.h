/*=========================================================================
Program:   HTO-Planner
Module:    albaOpMeasureAxisRotations.h
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

#ifndef __albaOpMeasureAxisRotations_H__
#define __albaOpMeasureAxisRotations_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDecl.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// Forward references :
class albaVME;

//----------------------------------------------------------------------------
// Class Name: albaOpMeasureAxisRotations
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpMeasureAxisRotations : public albaOp
{
public:
	/** Constructor. */
	albaOpMeasureAxisRotations(wxString label = "Measure Axis Rotation");

	/** Destructor. */
	~albaOpMeasureAxisRotations();

	/** RTTI macro. */
	albaTypeMacro(albaOpMeasureAxisRotations, albaOp);

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

	void UpdateRotations();

	static bool AcceptRelative(albaVME* node);

	void ChooseRelativeVME();
	albaVME* m_RelativeVME;
	albaString m_RelativeVMEName;

	double m_Rotations[3];

	int m_Modality;
};
#endif
