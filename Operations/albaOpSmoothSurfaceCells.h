/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSmoothSurfaceCells
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpSmoothSurfaceCells_H__
#define __albaOpSmoothSurfaceCells_H__

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
class vtkALBARemoveCellsFilter;

//----------------------------------------------------------------------------
// albaOpSmoothSurfaceCells :
//----------------------------------------------------------------------------
/** 
Perform cells selection and flip of the normals.
CTRL modifier must be used in order to select cells in render window while
performing mouse dragging.
This operation assumes that input polygonal data is made of triangles only.
*/
class ALBA_EXPORT albaOpSmoothSurfaceCells: public albaOp
{
public:

	albaOpSmoothSurfaceCells(wxString label = "Smooth Cells");
	~albaOpSmoothSurfaceCells(); 

	albaTypeMacro(albaOpSmoothSurfaceCells, albaOp);

	albaOp* Copy();

	void OnEvent(albaEventBase *alba_event);

	/** Builds operation's interface by calling CreateOpDialog() method. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

	/** set the seed ie the cell originating the selection area */
	void SetSeed(vtkIdType cellSeed);

  /** get the seed ie the cell originating the selection area */
  vtkIdType GetSeed();

	/** Get the selection region radius */
	double GetDiameter() const { return m_Diameter; };
  
  /** Set the selection region radius */
	void SetDiameter(double val) {m_Diameter = val;};

	/** Mark cells */
	void MarkCells();

	/** performs flip normals */
	void SmoothCells();

  /** return true is the i-th cell is selected otherwise return false */
  bool CellIsSelected(int i);

  /** return the number of interactions for the smooth filter */
  int GetNumberOfInteractions(){return m_SmoothParameterNumberOfInteractions;};

  /** return the feature angle for the smooth filter */
  double GetFeatureAngle(){return m_SmoothParameterFeatureAngle;};

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	double GetCellsDistance(vtkIdType c1,vtkIdType c2);

	void FindTriangleCellCenter(vtkIdType id, double center[3]);

	// used to support algorithm execution
	vtkIdList						*m_NeighborCellPointIds;
	int									*m_VisitedCells;
	
	int									 m_RegionNumber;   
	vtkIdType						 m_CellSeed;
	vtkIdList						*m_Wave;
	vtkIdList						*m_Wave2;
	double							 m_Diameter;

	double							 m_MinBrushSize;
	double							 m_MaxBrushMSize;

  //smooth parameters
  int               m_SmoothParameterNumberOfInteractions;
  int               m_SmoothParameterBoundary;
  double            m_SmoothParameterFeatureAngle;
  //end smooth parameters
	int                  m_UnselectCells;

	albaGUIDialog						*m_Dialog;
	albaRWI							*m_Rwi;
	vtkPolyDataMapper		*m_PolydataMapper;
	vtkActor						*m_PolydataActor;
	albaInteractorSelectCell				*m_SelectCellInteractor;
	vtkALBACellsFilter			*m_CellFilter;
  vtkALBARemoveCellsFilter *m_RemoveSelectedCells;
  vtkALBARemoveCellsFilter *m_RemoveUnSelectedCells;
	vtkPolyData					*m_Mesh;

	vtkPolyData					*m_ResultPolydata;
	vtkPolyData					*m_OriginalPolydata;

	/** 
	Builds operation's interface and visualization pipeline. */
	void CreateOpDialog();

	/** 
	Remove operation's interface. */
	void DeleteOpDialog();

	void CreateSurfacePipeline();
  void CreateCellFilters();
  void DestroyCellFilters();
	void TraverseMeshAndMark(double radius);
	void MarkCellsInRadius(double radius);
	void InitializeMesh();

};
#endif
