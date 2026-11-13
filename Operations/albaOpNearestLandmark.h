/*=========================================================================
Program:   HTO-Planner
Module:    albaOpNearestLandmark.h
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

#ifndef __albaOpNearestLandmark_H__
#define __albaOpNearestLandmark_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDecl.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// Forward references :
class albaVMELandmarkCloud;
class vtkPolyData;
class albaVMELandmark;

//----------------------------------------------------------------------------
// Class Name: albaOpNearestLandmark
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpNearestLandmark : public albaOp
{
public:
	/** Constructor. */
	albaOpNearestLandmark(wxString label = "Closest Landmark on Surface");

	/** Destructor. */
	~albaOpNearestLandmark();

	/** RTTI macro. */
	albaTypeMacro(albaOpNearestLandmark, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Create the point of poly closest to lm, as a new landmark in the cloud of lm.
	name is the name of the new landmark, default "Closest <lm> on <surface>"; a progressive number is added if it already exists. */
	static albaVMELandmarkCloud *CreateClosestPoint(vtkPolyData * poly, albaVMELandmark * lm, albaVME * surface, const char *name = NULL);
	
	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Receive events coming from the user interface.*/
	void OnEvent(albaEventBase *alba_event);
 
	/**accept for surfaces and surface parametric*/
	static bool SurfaceAccept(albaVME* node);

	/** Accept for landmarks, used when the input is not a landmark */
	static bool LandmarkAccept(albaVME* node);

protected:

	/** Always true: if the input is not a landmark, OpRun asks to choose one. */
	/*virtual*/ bool InternalAccept(albaVME *node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	
};
#endif
