/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeIsosurface
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeIsosurface_H__
#define __albaPipeIsosurface_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaPipeGenericPolydata.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkAlgorithmOutput;
class vtkFlyingEdges3D;
class albaVMESurface;
class vtkPolyData;
class vtkContourFilter;

//----------------------------------------------------------------------------
// albaPipeIsosurface :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaPipeIsosurface : public albaPipeGenericPolydata
{
public:
	albaTypeMacro(albaPipeIsosurface, albaPipeGenericPolydata);

	/** IDs for the GUI */
	enum PIPE_ISOSURFACE_WIDGET_ID
	{
		ID_CONTOUR_VALUE = Superclass::ID_LAST,
		ID_GENERATE_ISOSURFACE,
		ID_LAST
	};


	albaPipeIsosurface();
	virtual     ~albaPipeIsosurface();

	/** Set the contour value for contour mapper, return false if contour mapper is NULL, otherwise return true. */
	bool   SetContourValue(float value);

	/** return the contour value. */
	float  GetContourValue();

	/** process events coming from Gui */
	virtual void OnEvent(albaEventBase* alba_event);
	void ExctractIsosurface(albaVMESurface* isoSurfaceVme = NULL);
	void ExctractIsosurface(vtkPolyData* isoSurfacePD);


	void SetAlphaValue(double value);
	double GetAlphaValue() { return m_AlphaValue; };

protected:

	/** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
	virtual albaGUI* CreateGui();

	vtkAlgorithmOutput* GetPolyDataOutputPort();

	vtkFlyingEdges3D* m_ContourFilter;
	vtkContourFilter* m_RGContourFilter;

	double m_DataRange[2];
	double m_ContourValue;
	double m_OldContourValue;
	double m_AlphaValue;
};  
#endif // __albaPipeIsosurface_H__
