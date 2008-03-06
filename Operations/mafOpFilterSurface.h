/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpFilterSurface.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpFilterSurface_H__
#define __mafOpFilterSurface_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;
class mafEvent;

//----------------------------------------------------------------------------
// mafOpFilterSurface :
//----------------------------------------------------------------------------
/** */
class mafOpFilterSurface: public mafOp
{
public:
	mafOpFilterSurface(const wxString &label = "FilterSurface");
	~mafOpFilterSurface(); 
	virtual void OnEvent(mafEventBase *maf_event);

  mafTypeMacro(mafOpFilterSurface, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);
	
	int	m_TopologyFlag;
	int	m_Reduction;
	int	m_Angle;
	int	m_EdgeSplit;
	int	m_FlipNormals;
	int	m_Iterations;

	bool m_ClearInterfaceFlag;
	bool m_PreviewResultFlag;

	vtkPolyData											*m_ResultPolydata;
	vtkPolyData											*m_OriginalPolydata;

	/** Clean the surface. */
	void OnClean();

	/** Extract all the connected surfaces. */
	void OnVtkConnect();

	/** Smooth the surface. */
	void OnSmooth();

	/** Decimate the surface. */
	void OnDecimate();

	/** Converts the surface's triangles into triangle strip. */
	void OnStripper();

	/** Triangulate the surface. */
	void OnTriangulate();

	/** Generate the surface's normals. */
	void OnGenerateNormals();

	/** Reset surface's normals. */
	void OnResetNormals();

	/** Make the preview of the surface filtering. */
	void OnPreview();  

	/** Clear all the surface applied filtering. */
	void OnClear();
};
#endif
