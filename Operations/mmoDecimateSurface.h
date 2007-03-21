/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoDecimateSurface.h,v $
  Language:  C++
  Date:      $Date: 2007-03-21 16:31:28 $
  Version:   $Revision: 1.1 $
  Authors:   Daniele Giunchi - Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoDecimateSurface_H__
#define __mmoDecimateSurface_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;
class mafEvent;

//----------------------------------------------------------------------------
// mmoDecimateSurface :
//----------------------------------------------------------------------------
/** */
class mmoDecimateSurface: public mafOp
{
public:
	mmoDecimateSurface(const wxString label = "FilterSurface");
	~mmoDecimateSurface(); 
	virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mmoDecimateSurface, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

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
