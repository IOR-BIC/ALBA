/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpPolylineFromLandmarkCloud
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpPolylineFromLandmarkCloud_H__
#define __albaOpPolylineFromLandmarkCloud_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaVMEPolyline;
class albaVMELandmarkCloud;
class vtkPolyData;

/** albaOpPolylineFromLandmarkCloud - creates an open polyline through the landmarks of a landmark cloud,
in the order they have in the cloud. The polyline is added as a sibling of the cloud. No GUI. */
class ALBA_EXPORT albaOpPolylineFromLandmarkCloud : public albaOp
{
public:
	/** Constructor. */
	albaOpPolylineFromLandmarkCloud(const wxString &label = "Polyline From Landmark Cloud");

	/** Destructor. */
	~albaOpPolylineFromLandmarkCloud();

	/** RTTI macro. */
	albaTypeMacro(albaOpPolylineFromLandmarkCloud, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Create the polyline and stop the operation (no GUI). */
	/*virtual*/ void OpRun();

	/** Add the polyline as a sibling of the landmark cloud. */
	/*virtual*/ void OpDo();

	/** Fill poly with an open polyline through the landmarks of cloud, in cloud coordinates */
	static void CreatePolyData(albaVMELandmarkCloud *cloud, vtkPolyData *poly);

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char **GetIcon();

protected:

	/** Accept landmark clouds with at least two landmarks. */
	/*virtual*/ bool InternalAccept(albaVME *node);

	albaVMEPolyline *m_Polyline;
};
#endif
