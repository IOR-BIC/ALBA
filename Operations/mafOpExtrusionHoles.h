/*=========================================================================

 Program: MAF2
 Module: mafOpExtrusionHoles
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafOpExtrusionHoles_H__
#define __mafOpExtrusionHoles_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"
#include "vtkAppendPolyData.h"
#include "vtkFeatureEdges.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafGUIDialog;
class mafGUIButton;
class mafRWI;
class mmiSelectPoint;
class mafVMESurface;
class vtkHoleConnectivity;
class vtkPolyDataMapper;
class vtkActor;
class vtkSphereSource;
class vtkGlyph3D;
class vtkAppendPolyData;
class vtkPlaneSource;
class vtkMAFExtrudeToCircle;

//----------------------------------------------------------------------------
// mafOpExtrusionHoles :
//----------------------------------------------------------------------------
/** 
Perform extrusion of holes in a surface.
CTRL modifier must be used in order to select a hole in render window while
performing mouse dragging.
*/
class MAF_EXPORT mafOpExtrusionHoles: public mafOp
{
public:
	mafOpExtrusionHoles(const wxString &label = "Extrude Holes");
	~mafOpExtrusionHoles(); 

	virtual void OnEvent(mafEventBase *maf_event);

	mafTypeMacro(mafOpExtrusionHoles, mafOp);

	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool Accept(mafVME*node);

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
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);

	void CreateOpDialog();
	void DeleteOpDialog();
	void CreatePolydataPipeline();

	mafGUIDialog							*m_Dialog;
	mafGUIButton							*m_ButtonOk;
	mafRWI								*m_Rwi;
	mmiSelectPoint				*m_Picker;

	vtkPolyData						*m_ResultPolydata;
	vtkPolyData						*m_OriginalPolydata;
	mafVMESurface					*m_ResultSurface;
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
	vtkMAFExtrudeToCircle	    *m_ExtrusionFilter;

	vtkSphereSource				*m_Sphere;
	vtkGlyph3D						*m_Glyph;

	vtkPlaneSource *m_Plane;

	double m_MaxBounds;

  int m_ReverseExtrusion;
  int m_NumVertices;
};
#endif
