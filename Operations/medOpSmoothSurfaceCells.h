/*=========================================================================

 Program: MAF2Medical
 Module: medOpSmoothSurfaceCells
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpSmoothSurfaceCells_H__
#define __medOpSmoothSurfaceCells_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "medOperationsDefines.h"
#include "mafOp.h"
#include "vtkIdList.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafRWI;
class mafNode;
class mafGUIDialog;
class mafInteractorSelectCell;

class vtkActor;
class vtkPolyData;
class vtkMAFCellsFilter;
class vtkCellCenters;
class vtkArrowSource;
class vtkPolyDataMapper;
class vtkGlyph3D;
class vtkMAFRemoveCellsFilter;

//----------------------------------------------------------------------------
// medOpSmoothSurfaceCells :
//----------------------------------------------------------------------------
/** 
Perform cells selection and flip of the normals.
CTRL modifier must be used in order to select cells in render window while
performing mouse dragging.
This operation assumes that input polygonal data is made of triangles only.
*/
class MED_OPERATION_EXPORT medOpSmoothSurfaceCells: public mafOp
{
public:

	medOpSmoothSurfaceCells(wxString label = "Smooth Cells");
	~medOpSmoothSurfaceCells(); 

	mafTypeMacro(medOpSmoothSurfaceCells, mafOp);

	mafOp* Copy();

	void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* vme);

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

	mafGUIDialog						*m_Dialog;
	mafRWI							*m_Rwi;
	vtkPolyDataMapper		*m_PolydataMapper;
	vtkActor						*m_PolydataActor;
	mafInteractorSelectCell				*m_SelectCellInteractor;
	vtkMAFCellsFilter			*m_CellFilter;
  vtkMAFRemoveCellsFilter *m_RemoveSelectedCells;
  vtkMAFRemoveCellsFilter *m_RemoveUnSelectedCells;
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
