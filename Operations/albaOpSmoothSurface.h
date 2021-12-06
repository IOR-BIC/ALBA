/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSmoothSurface
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpSmoothSurface_H__
#define __albaOpSmoothSurface_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;
class albaEvent;

//----------------------------------------------------------------------------
// albaOpSmoothSurface :
//----------------------------------------------------------------------------
/** 
class name:  albaOpSmoothSurface.
Class which supplies the possibility to smooth a surface, giving a number of iteration of smooth, clear the result or
preview the output.
*/
class ALBA_EXPORT albaOpSmoothSurface: public albaOp
{
public:
  /** object constructor */
	albaOpSmoothSurface(const wxString &label = "Smooth Surface");
  /** object destructor */  
	~albaOpSmoothSurface();
  /** method for catch the dispatched events */
	virtual void OnEvent(albaEventBase *alba_event);
  /** RTTI macro */
	albaTypeMacro(albaOpSmoothSurface, albaOp);
  /** method for clone object */
	albaOp* Copy();

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

  /** Smooth the surface. */
  void OnSmooth();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

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
