/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpRemoveCells
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpRemoveCells_H__
#define __albaOpRemoveCells_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "vtkIdList.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUIDialog;
class albaGUIFloatSlider;
class albaVMESurface;
class albaVME;
class albaRWI;

class vtkActor;
class vtkPolyDataMapper;
class vtkRenderer;
class vtkCell;

class vtkPolyData;
class vtkALBARemoveCellsFilter;
class vtkTriangle;

class albaInteractorSelectCell;

//----------------------------------------------------------------------------
// albaOpRemoveCells :
//----------------------------------------------------------------------------
/** 
Perform cells selection and removing on surfaces.
CTRL modifier must be used in order to select cells in render window while
performing mouse dragging.
This operation assumes that input polygonal data is made of triangles only.

@sa albaOpRemoveCellsTest

@todo Extract selection related stuff to class
*/
class ALBA_EXPORT albaOpRemoveCells: public albaOp
{
public:
	
  albaOpRemoveCells(wxString label = "RemoveCells");
 ~albaOpRemoveCells(); 
	
  albaTypeMacro(albaOpRemoveCells, albaOp);

  albaOp* Copy();

  void OnEvent(albaEventBase *alba_event);

	/** Builds operation's interface by calling CreateOpDialog() method. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();

  /** testing functions:this could be used from the commandline */

  /** set the seed ie the cell originating the selection area */
  void SetSeed(vtkIdType cellSeed);

  /** Set the selection region radius */
  double GetDiameter() const { return m_Diameter; };
  void SetDiameter(double val) {m_Diameter = val;};

  /** Mark cells to be removed */
  void MarkCells();

  /** performs cells removing */
  void RemoveCells();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  double GetCellsDistance(vtkIdType c1,vtkIdType c2);

  void FindTriangleCellCenter(vtkIdType id, double center[3]);

  // used to support algorithm execution
  vtkIdList *m_NeighborCellPointIdList;
  int *m_VisitedCells;
  vtkIdType *m_VisitedPoints;
  int m_RegionNumber;
  vtkIdType m_PointNumber;    
  vtkIdType m_CellSeed;
  vtkIdList *m_Wave;
  vtkIdList *m_Wave2;
  vtkIdList *m_PointIds;
  vtkIdList *m_CellIds;
  double m_Diameter;
  int   m_AutoClip;
 
  double m_MinBrushSize;
  double m_MaxBrushMSize;
  int m_UnselectCells;

  albaGUIDialog		*m_Dialog;
	albaRWI      *m_Rwi;
  vtkPolyDataMapper *m_PolydataMapper;
  vtkActor          *m_PolydataActor;
  albaInteractorSelectCell *m_SelectCellInteractor;
  vtkALBARemoveCellsFilter *m_Rcf;
  vtkPolyData *m_Mesh;
  
  vtkPolyData											*m_ResultPolydata;
  vtkPolyData											*m_OriginalPolydata;

  /** 
  Builds operation's interface and visualization pipeline. */
  void CreateOpDialog();

	/** 
  Remove operation's interface. */
  void DeleteOpDialog();

  void CreateSurfacePipeline();
  void ExecuteMark(double radius);
	void MarkCellsInRadius(double radius);

  void DestroyHelperStructures();
  void CreateHelperStructures();
  
  vtkIdList* m_TriangeCentreComputationList;
  vtkTriangle *m_DummyTriangle;
  
  // helper points here for fast computation
  double m_P0[3],m_P1[3],m_P2[3];
};
#endif
