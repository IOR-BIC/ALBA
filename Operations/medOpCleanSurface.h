/*=========================================================================

 Program: MAF2Medical
 Module: medOpCleanSurface
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpCleanSurface_H__
#define __medOpCleanSurface_H__

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

/**
  class name: medOpCleanSurface
    Operation that apply cvtkCleanPolyData Filter to the input surface.  
*/
class MAF_EXPORT medOpCleanSurface: public mafOp
{
public:
  /** constructor */
	medOpCleanSurface(const wxString &label = "CleanSurface");
  /** destructor */
	~medOpCleanSurface();
  /** Answer to the messages coming from interface. */ 
	virtual void OnEvent(mafEventBase *maf_event);

  /** RTTI macro */
	mafTypeMacro(medOpCleanSurface, mafOp);

  /** Return a copy of the operation.*/
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

  /** Clean the surface. */
  void OnClean();

protected:
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
