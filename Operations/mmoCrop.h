/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCrop.h,v $
  Language:  C++
  Date:      $Date: 2007-07-11 14:15:08 $
  Version:   $Revision: 1.8 $
  Authors:   Matteo Giacomoni & Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoCrop_H__
#define __mmoCrop_H__

#include "mafNode.h"
#include "mafOp.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmgGui;

class mafEventBase;
class mafString;

class mafVMESurface;
class mafVMEGizmo;

class vtkRectilinearGrid;
class vtkProbeFilter;
class vtkStructuredPoints;
class mafGizmoROI;

//----------------------------------------------------------------------------
// mmoCrop :
//----------------------------------------------------------------------------
/** 
Crop a vme gray volume  
*/

class mmoCrop: public mafOp
{
public:
             
            	 mmoCrop(const wxString &label = _("Crop"));
	virtual     ~mmoCrop();
	virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mmoCrop, mafOp);

  mafOp* Copy();

	bool Accept(mafNode* node);
	void OpRun();	
	
  /**	Extract the volume cropped*/
	void Crop();

	/** Makes the undo for the operation. */
	virtual void OpUndo();

  void OpDo();

  virtual void CreateGui(); 

	/**	Set cropping area*/
	void SetCroppingBoxBounds(double bounds[]);

protected:
  void UpdateGui();
  void OpStop(int result);	
  
  mafGizmoROI *m_GizmoROI; ///< Gizmo used to define sub-volume region to crop
  
	vtkRectilinearGrid *m_InputRG; ///< Pointer for Rectilinear Grid volume
	vtkStructuredPoints *m_InputSP; ///< Pointer for Structured Points volume

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
	vtkStructuredPoints *m_OutputSP; ///< Pointer for Structured Points cropped volume
};
#endif
