/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpEditNormals
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpEditNormals_H__
#define __albaOpEditNormals_H__

#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;
class albaEvent;

//----------------------------------------------------------------------------
// albaOpEditNormals :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpEditNormals: public albaOp
{
public:
	albaOpEditNormals(const wxString &label = "Edit Normals");
	~albaOpEditNormals(); 
	virtual void OnEvent(albaEventBase *alba_event);

	albaTypeMacro(albaOpEditNormals, albaOp);

	albaOp* Copy();

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

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

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
