/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCleanSurface
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpCleanSurface_H__
#define __albaOpCleanSurface_H__

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

/**
  class name: albaOpCleanSurface
    Operation that apply cvtkCleanPolyData Filter to the input surface.  
*/
class ALBA_EXPORT albaOpCleanSurface: public albaOp
{
public:
  /** constructor */
	albaOpCleanSurface(const wxString &label = "Clean Surface");
  /** destructor */
	~albaOpCleanSurface();
  /** Answer to the messages coming from interface. */ 
	virtual void OnEvent(albaEventBase *alba_event);

  /** RTTI macro */
	albaTypeMacro(albaOpCleanSurface, albaOp);

  /** Return a copy of the operation.*/
	albaOp* Copy();

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

  /** Clean the surface. */
  void OnClean();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  /** Create the dialog interface for the importer. */
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
