/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpFlipNormals
 Authors: Matteo Giacomoni - Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpFlipNormals_H__
#define __albaOpFlipNormals_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"
#include "vtkIdList.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class albaRWI;
class albaVME;
class albaGUIDialog;
class albaInteractorSelectCell;

class vtkActor;
class vtkPolyData;
class vtkALBACellsFilter;
class vtkCellCenters;
class vtkArrowSource;
class vtkPolyDataMapper;
class vtkGlyph3D;

/**
class name : albaOpFlipNormals

Perform cells selection and flip of the normals.
CTRL modifier must be used in order to select cells in render window while
performing mouse dragging.
This operation assumes that input polygonal data is made of triangles only.
*/
class ALBA_EXPORT albaOpFlipNormals: public albaOp
{
public:

  /** constructor. */
	albaOpFlipNormals(wxString label = "Flip Normals");

  /** destructor. */
	~albaOpFlipNormals(); 

  /** RTTI macro */
	albaTypeMacro(albaOpFlipNormals, albaOp);

	/*virtual*/ albaOp* Copy();

  /** Precess events coming from other objects */
	/*virtual*/ void OnEvent(albaEventBase *alba_event);

	/** Builds operation's interface by calling CreateOpDialog() method. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Makes the undo for the operation. */
	/*virtual*/ void OpUndo();

	/** set the seed ie the cell originating the selection area */
	void SetSeed(vtkIdType cellSeed);
	
  /** get the seed ie the cell originating the selection area */
  vtkIdType GetSeed(){return m_CellSeed;};

	/** Set the selection region radius */
	double GetDiameter() const { return m_Diameter; };

  /** Get the selection region radius */
	void SetDiameter(double val) {m_Diameter = val;};

	/** Mark cells */
	void MarkCells();

	/** performs flip normals */
	void FlipNormals();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	double GetCellsDistance(vtkIdType c1,vtkIdType c2);

	void FindTriangleCellCenter(vtkIdType id, double center[3]);

	// used to support algorithm execution
	vtkIdList						*m_NeighborCellPointIds;
	int									*m_VisitedCells;
	vtkIdType						*m_VisitedPoints;
	int									 m_RegionNumber;
	vtkIdType						 m_PointNumber;    
	vtkIdType						 m_CellSeed;
	vtkIdList						*m_Wave;
	vtkIdList						*m_Wave2;
	double							 m_Diameter;

	vtkGlyph3D					*m_NormalGlyph;
	vtkPolyDataMapper		*m_NormalMapper;
	vtkActor						*m_NormalActor;
	vtkCellCenters			*m_CenterPointsFilter;
	vtkArrowSource			*m_NormalArrow;
	vtkPolyData					*m_Centers;

	double							 m_MinBrushSize;
	double							 m_MaxBrushMSize;
	int                  m_UnselectCells;

	albaGUIDialog						*m_Dialog;
	albaRWI							*m_Rwi;
	vtkPolyDataMapper		*m_PolydataMapper;
	vtkActor						*m_PolydataActor;
	albaInteractorSelectCell				*m_SelectCellInteractor;
	vtkALBACellsFilter			*m_CellFilter;
	vtkPolyData					*m_Mesh;

	vtkPolyData					*m_ResultPolydata;
	vtkPolyData					*m_OriginalPolydata;

	/** 
	Builds operation's interface and visualization pipeline. */
	void CreateOpDialog();

	/** 
	Remove operation's interface. */
	void DeleteOpDialog();

	void CreateNormalsPipe();
	void CreateSurfacePipeline();
	void TraverseMeshAndMark(double radius);
	void MarkCellsInRadius(double radius);
	void InitializeMesh();
	void ModifyAllNormal();

  friend class albaOpFlipNormalsTest;

};
#endif
