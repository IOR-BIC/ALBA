/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpDecimateSurface
 Authors: Daniele Giunchi - Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpDecimateSurface_H__
#define __albaOpDecimateSurface_H__

#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;
class albaEvent;

//----------------------------------------------------------------------------
// albaOpDecimateSurface :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpDecimateSurface: public albaOp
{
public:
	albaOpDecimateSurface(const wxString label = "FilterSurface");
	~albaOpDecimateSurface(); 
	virtual void OnEvent(albaEventBase *alba_event);

  albaTypeMacro(albaOpDecimateSurface, albaOp);

  albaOp* Copy();

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

	void SetReduction(int value){m_Reduction = value;};

	/** Decimate the surface. */
	void OnDecimate();

	/** Make the preview of the surface filtering. */
	void OnPreview();  

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);
	
	int	m_TopologyFlag;
	int	m_Reduction;

	bool m_ClearInterfaceFlag;
	bool m_PreviewResultFlag;

	vtkPolyData											*m_ResultPolydata;
	vtkPolyData											*m_OriginalPolydata;

  int m_WireFrame;

	/** Clear all the surface applied filtering. */
	void OnClear();
};
#endif
