/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpFilterSurface
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpFilterSurface_H__
#define __albaOpFilterSurface_H__

#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;
class albaEvent;

//----------------------------------------------------------------------------
// albaOpFilterSurface :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpFilterSurface: public albaOp
{
public:
	albaOpFilterSurface(const wxString &label = "FilterSurface");
	~albaOpFilterSurface(); 
	virtual void OnEvent(albaEventBase *alba_event);

  albaTypeMacro(albaOpFilterSurface, albaOp);

  albaOp* Copy();

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

  /** Return parameters used by operation. */
  albaString GetParameters();

  /** Return the number of iteration of the smooth filter */
  int GetNumberOfIterations(){return m_Iterations;};

  /** Return target reduction valu of the decimate filter */
  int GetTargetReduction(){return m_Reduction;};

  /** Return preserve topology flag of the decimate filter */
  int GetPreserveTopology(){return m_TopologyFlag;};

  /** Return flip normals flag of the generate normals filter */
  int GetFlipNormals(){return m_FlipNormals;};

  /** Return edge split flag of the generate normals filter */
  int GetEdgeSplit(){return m_EdgeSplit;};

  /** Return angle value of the generate normals filter */
  int GetFeatureAngle(){return m_Angle;};
	
protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  /** Create the GUI */
  void CreateGui();
	
	int	m_TopologyFlag;
	int	m_Reduction;
	int	m_Angle;
	int	m_EdgeSplit;
	int	m_FlipNormals;
	int	m_Iterations;

	bool m_ClearInterfaceFlag;
	bool m_PreviewResultFlag;
  bool m_StripFlag;
  bool m_TriangulateFlag;
  bool m_CleanFlag;
  bool m_ConnectivityFlag;


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

  friend class albaOpFilterSurfaceTest;
};
#endif
