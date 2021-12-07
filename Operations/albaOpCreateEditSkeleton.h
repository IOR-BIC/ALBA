/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateEditSkeleton
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpCreateEditSkeleton_H__
#define __albaOpCreateEditSkeleton_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMEPolylineGraph;
class albaGeometryEditorPolylineGraph;
class vtkPolyData;

/**
class name: albaOpCreateEditSkeleton
Class for creating and editing the skeleton polyline.
*/
class ALBA_EXPORT albaOpCreateEditSkeleton: public albaOp
{
public:
  /** constructor */
	albaOpCreateEditSkeleton(wxString label = "Create Edit Skeleton");
  /** destructor */
	~albaOpCreateEditSkeleton(); 

  /** RTTI macro */
	albaTypeMacro(albaOpCreateEditSkeleton, albaOp);

  /** Return a copy of itself, this needs to put the operation into the undo stack. */
	albaOp* Copy();

  /** Precess events coming from other objects */
	virtual void OnEvent(albaEventBase *alba_event);

	/** Builds operation's interface by calling CreateOpDialog() method. */
	virtual void OpRun();

	/** Execute the operation. */
	virtual void OpDo();

	/** Makes the undo for the operation. */
	virtual void OpUndo();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	virtual void OpStop(int result);

  /** Internally used to create a new instance of the GUI.*/
	void CreateGui();

	albaVMEPolylineGraph	*m_Skeleton;
	vtkPolyData					*m_ResultPolydata;

	albaGeometryEditorPolylineGraph *m_Editor;
};
#endif
