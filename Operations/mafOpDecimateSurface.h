/*=========================================================================

 Program: MAF2
 Module: mafOpDecimateSurface
 Authors: Daniele Giunchi - Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpDecimateSurface_H__
#define __mafOpDecimateSurface_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;
class mafEvent;

//----------------------------------------------------------------------------
// mafOpDecimateSurface :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpDecimateSurface: public mafOp
{
public:
	mafOpDecimateSurface(const wxString label = "FilterSurface");
	~mafOpDecimateSurface(); 
	virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mafOpDecimateSurface, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafVME*node);

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
