/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpTriangulateSurface
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpTriangulateSurface_H__
#define __albaOpTriangulateSurface_H__

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
/** */
class ALBA_EXPORT albaOpTriangulateSurface: public albaOp
{
public:
	albaOpTriangulateSurface(const wxString &label = "Triangulate Surface");
	~albaOpTriangulateSurface(); 
	virtual void OnEvent(albaEventBase *alba_event);

	albaTypeMacro(albaOpTriangulateSurface, albaOp);

	albaOp* Copy();

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

  /** Triangulate the surface. */
  void OnTriangle();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  void CreateGui();

	bool m_ClearInterfaceFlag;
	bool m_PreviewResultFlag;

	vtkPolyData											*m_ResultPolydata;
	vtkPolyData											*m_OriginalPolydata;


	/** Make the preview of the surface filtering. */
	void OnPreview();  

	/** Clear all the surface applied filtering. */
	void OnClear();
};
#endif
