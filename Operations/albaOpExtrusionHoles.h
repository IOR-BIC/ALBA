/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExtrusionHoles
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaOpExtrusionHoles_H__
#define __albaOpExtrusionHoles_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"
#include "vtkAppendPolyData.h"
#include "vtkFeatureEdges.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class albaGUIDialog;
class albaGUIButton;
class albaRWI;
class mmiSelectPoint;
class albaVMESurface;
class vtkHoleConnectivity;
class vtkPolyDataMapper;
class vtkActor;
class vtkSphereSource;
class vtkGlyph3D;
class vtkAppendPolyData;
class vtkPlaneSource;
class vtkALBAExtrudeToCircle;

//----------------------------------------------------------------------------
// albaOpExtrusionHoles :
//----------------------------------------------------------------------------
/** 
Perform extrusion of holes in a surface.
CTRL modifier must be used in order to select a hole in render window while
performing mouse dragging.
*/
class ALBA_EXPORT albaOpExtrusionHoles: public albaOp
{
public:
	albaOpExtrusionHoles(const wxString &label = "Extrude Holes");
	~albaOpExtrusionHoles(); 

	virtual void OnEvent(albaEventBase *alba_event);

	albaTypeMacro(albaOpExtrusionHoles, albaOp);

	albaOp* Copy();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Makes the undo for the operation. */
	/*virtual*/ void OpUndo();

	/** Highlight the hole selected with red color */
	void SelectHole(int pointID);

	/** Perform extrusion and visualize the result */
	void Extrude();

	/** Save the result polydata of extrusion in VME data */
	void SaveExtrusion();

	void ExtractFreeEdge();

	void SetExtrusionFactor(double value){m_ExtrusionFactor = value;};
	double GetExtrusionFactor(){return m_ExtrusionFactor;};

	int GetExtractFreeEdgesNumeberOfPoints(){if(m_ExtractFreeEdges)return m_ExtractFreeEdges->GetOutput()->GetNumberOfPoints(); else return 0;};

	vtkPolyData* GetExtrutedSurface(){return m_ResultPolydata;};

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);

	void CreateOpDialog();
	void DeleteOpDialog();
	void CreatePolydataPipeline();

	albaGUIDialog							*m_Dialog;
	albaGUIButton							*m_ButtonOk;
	albaRWI								*m_Rwi;
	mmiSelectPoint				*m_Picker;

	vtkPolyData						*m_ResultPolydata;
	vtkPolyData						*m_OriginalPolydata;
	albaVMESurface					*m_ResultSurface;
	vtkAppendPolyData			*m_ResultAfterExtrusion;

	double								m_SphereRadius;
	double								m_ExtrusionFactor;

	vtkPolyDataMapper			*m_SurfaceMapper;
	vtkActor							*m_SurfaceActor;
	vtkPolyDataMapper			*m_HolesMapper;
	vtkActor							*m_HolesActor;
	vtkPolyDataMapper			*m_SelectedHoleMapper;
	vtkActor							*m_SelectedHoleActor;

	vtkFeatureEdges						*m_ExtractFreeEdges;
	vtkHoleConnectivity				*m_ExtractHole;
	vtkALBAExtrudeToCircle	    *m_ExtrusionFilter;

	vtkSphereSource				*m_Sphere;
	vtkGlyph3D						*m_Glyph;

	vtkPlaneSource *m_Plane;

	double m_MaxBounds;

  int m_ReverseExtrusion;
  int m_NumVertices;
};
#endif
