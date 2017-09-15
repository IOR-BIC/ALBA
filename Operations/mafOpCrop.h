/*=========================================================================

 Program: MAF2
 Module: mafOpCrop
 Authors: Matteo Giacomoni & Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCrop_H__
#define __mafOpCrop_H__

#include "mafVME.h"
#include "mafOp.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUI;

class mafEventBase;
class mafString;

class mafVMESurface;
class mafVMEGizmo;

class vtkRectilinearGrid;
class vtkProbeFilter;
class vtkImageData;
class mafGizmoROI;

//----------------------------------------------------------------------------
// mafOpCrop :
//----------------------------------------------------------------------------
/** 
Crop a vme gray volume  
*/

class MAF_EXPORT mafOpCrop: public mafOp
{
public:
             
            	 mafOpCrop(const wxString &label=_("Crop") , bool showShadingPlane = false);
	virtual     ~mafOpCrop();
	/*virtual*/ void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mafOpCrop, mafOp);

  mafOp* Copy();

	/*virtual*/ bool Accept(mafVME* node);
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
  /*virtual*/ void OpStop(int result);	
  
  mafGizmoROI *m_GizmoROI; ///< Gizmo used to define sub-volume region to crop
  
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
