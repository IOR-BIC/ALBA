/*=========================================================================

 Program: MAF2Medical
 Module: medOpSmoothSurface
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpSmoothSurface_H__
#define __medOpSmoothSurface_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;
class mafEvent;

//----------------------------------------------------------------------------
// medOpSmoothSurface :
//----------------------------------------------------------------------------
/** 
class name:  medOpSmoothSurface.
Class which supplies the possibility to smooth a surface, giving a number of iteration of smooth, clear the result or
preview the output.
*/
class MAF_EXPORT medOpSmoothSurface: public mafOp
{
public:
  /** object constructor */
	medOpSmoothSurface(const wxString &label = "FilterSurface");
  /** object destructor */  
	~medOpSmoothSurface();
  /** method for catch the dispatched events */
	virtual void OnEvent(mafEventBase *maf_event);
  /** RTTI macro */
	mafTypeMacro(medOpSmoothSurface, mafOp);
  /** method for clone object */
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

  /** Smooth the surface. */
  void OnSmooth();

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);
  /** implement gui and create widgets */
  void CreateGui();

	int	m_Iterations;

	bool m_ClearInterfaceFlag;
	bool m_PreviewResultFlag;
	int  m_BoundarySmoothing;

	vtkPolyData											*m_ResultPolydata;
	vtkPolyData											*m_OriginalPolydata;

	/** Make the preview of the surface filtering. */
	void OnPreview();  

	/** Clear all the surface applied filtering. */
	void OnClear();
};
#endif
