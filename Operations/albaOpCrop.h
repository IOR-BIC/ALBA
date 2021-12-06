/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCrop
 Authors: Matteo Giacomoni & Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpCrop_H__
#define __albaOpCrop_H__

#include "albaVME.h"
#include "albaOp.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUI;

class albaEventBase;
class albaString;

class albaVMESurface;
class albaVMEGizmo;

class vtkRectilinearGrid;
class vtkProbeFilter;
class vtkImageData;
class albaGizmoROI;

//----------------------------------------------------------------------------
// albaOpCrop :
//----------------------------------------------------------------------------
/** 
Crop a vme gray volume  
*/

class ALBA_EXPORT albaOpCrop : public albaOp
{
public:

	albaOpCrop(const wxString &label = _("Crop"), bool showShadingPlane = false);
	virtual     ~albaOpCrop();
	/*virtual*/ void OnEvent(albaEventBase *alba_event);

	albaTypeMacro(albaOpCrop, albaOp);

	albaOp* Copy();

	/*virtual*/ void OpRun();

	/**	Extract the volume cropped*/
	void Crop();

	/** Makes the undo for the operation. */
	/*virtual*/ void OpUndo();

	/*virtual*/ void OpDo();

	/*virtual*/ void CreateGui();

	/**	Set cropping area*/
	void SetCroppingBoxBounds(double bounds[]);

protected:

	void UpdateGui();

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/*virtual*/ void OpStop(int result);

	albaGizmoROI *m_GizmoROI; ///< Gizmo used to define sub-volume region to crop

	vtkRectilinearGrid *m_InputRG; ///< Pointer for Rectilinear Grid volume
	vtkImageData *m_InputSP; ///< Pointer for Structured Points volume

	double m_XSpacing; ///< X Spacing for cropped volume On Structured Points data
	double m_YSpacing; ///< Y Spacing for cropped volume On Structured Points data
	double m_ZSpacing; ///< Z Spacing for cropped volume On Structured Points data
	double m_CroppingBoxBounds[6]; ///< Bounds of cropped volume
	double m_InputBounds[6]; ///< Input volume bounds

	double m_XminXmax[2]; ///< 
	double m_YminYmax[2]; ///< 
	double m_ZminZmax[2]; ///< 

	int m_ShowHandles; ///< Flag used to show/hide crop gizmo handles
	int m_ShowROI; ///< Flag used to show/hide crop gizmo

	vtkRectilinearGrid	*m_OutputRG; ///< Pointer for Rectilinear Grid cropped volume
	vtkImageData *m_OutputSP; ///< Pointer for Structured Points cropped volume

	bool  m_ShowShadingPlane;
};
#endif
