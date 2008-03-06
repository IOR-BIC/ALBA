/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpEditNormals.h,v $
Language:  C++
Date:      $Date: 2008-03-06 11:55:06 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpEditNormals_H__
#define __mafOpEditNormals_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;
class mafEvent;

//----------------------------------------------------------------------------
// mafOpEditNormals :
//----------------------------------------------------------------------------
/** */
class mafOpEditNormals: public mafOp
{
public:
	mafOpEditNormals(const wxString &label = "Edit Normals");
	~mafOpEditNormals(); 
	virtual void OnEvent(mafEventBase *maf_event);

	mafTypeMacro(mafOpEditNormals, mafOp);

	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

	/** Generate the surface's normals. */
	void OnGenerateNormals();

	void SetFlipNormalsOn(){m_FlipNormals=1;};
	void SetFlipNormalsOff(){m_FlipNormals=0;};

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

	int	m_Angle;
	int	m_EdgeSplit;
	int	m_FlipNormals;

	bool m_ClearInterfaceFlag;
	bool m_PreviewResultFlag;

	vtkPolyData											*m_ResultPolydata;
	vtkPolyData											*m_OriginalPolydata;

	/** Reset surface's normals. */
	void OnResetNormals();

	/** Make the preview of the surface filtering. */
	void OnPreview();  

	/** Clear all the surface applied filtering. */
	void OnClear();
};
#endif
