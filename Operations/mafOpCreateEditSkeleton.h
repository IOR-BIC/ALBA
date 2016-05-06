/*=========================================================================

 Program: MAF2
 Module: mafOpCreateEditSkeleton
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCreateEditSkeleton_H__
#define __mafOpCreateEditSkeleton_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMEPolylineGraph;
class mafGeometryEditorPolylineGraph;
class vtkPolyData;

/**
class name: mafOpCreateEditSkeleton
Class for creating and editing the skeleton polyline.
*/
class MAF_EXPORT mafOpCreateEditSkeleton: public mafOp
{
public:
  /** constructor */
	mafOpCreateEditSkeleton(wxString label = "Create/Edit Skeleton");
  /** destructor */
	~mafOpCreateEditSkeleton(); 

  /** RTTI macro */
	mafTypeMacro(mafOpCreateEditSkeleton, mafOp);

  /** Return a copy of itself, this needs to put the operation into the undo stack. */
	mafOp* Copy();

  /** Precess events coming from other objects */
	virtual void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
	virtual bool Accept(mafVME* vme);

	/** Builds operation's interface by calling CreateOpDialog() method. */
	virtual void OpRun();

	/** Execute the operation. */
	virtual void OpDo();

	/** Makes the undo for the operation. */
	virtual void OpUndo();

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	virtual void OpStop(int result);

  /** Internally used to create a new instance of the GUI.*/
	void CreateGui();

	mafVMEPolylineGraph	*m_Skeleton;
	vtkPolyData					*m_ResultPolydata;

	mafGeometryEditorPolylineGraph *m_Editor;
};
#endif
