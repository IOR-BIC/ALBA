/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipePointCloudSlice
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipePointCloudSlice_H__B
#define __albaPipePointCloudSlice_H__B

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipeSlice.h"
#include "albaPipePointCloud.h"

class vtkALBAPointCloudCutter;
class vtkPlane; 

//----------------------------------------------------------------------------
// albaPipePointCloudSlice :
class ALBA_EXPORT albaPipePointCloudSlice : public albaPipePointCloud, public albaPipeSlice
{
public:
	/** IDs for the GUI */
	enum PIPE_POINT_CLOUD_SLICE_ID
	{
		ID_TOLERANCE = Superclass::ID_LAST,
		ID_LAST,
	};

  /** RTTI macro*/
	albaTypeMacro(albaPipePointCloudSlice,albaPipePointCloud);

  /** constructor */
	albaPipePointCloudSlice();
  /** destructor */
	virtual     ~albaPipePointCloudSlice();

	virtual vtkPolyData* GetInputAsPolyData();
  
  /** Set the origin and normal of the slice.
  Both, Origin and Normal may be NULL, if the current value is to be preserved. */
  /*virtual*/ void SetSlice(double* Origin, double* Normal);

	/** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
	albaGUI *CreateGui();

	/** process events coming from Gui */
	virtual void OnEvent(albaEventBase *alba_event);

protected:
  vtkPlane				        *m_Plane;
  vtkALBAPointCloudCutter		    *m_Cutter;
	double m_Tolerance;
};  
#endif // __albaPipePointCloudSlice_H__B
