/*=========================================================================

 Program: MAF2
 Module: mafOpRemoveCells
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpRemoveCells_H__
#define __mafOpRemoveCells_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "vtkIdList.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUIDialog;
class mafGUIFloatSlider;
class mafVMESurface;
class mafVME;
class mafRWI;

class vtkActor;
class vtkPolyDataMapper;
class vtkRenderer;
class vtkCell;

class vtkPolyData;
class vtkMAFRemoveCellsFilter;
class vtkTriangle;

class mafInteractorSelectCell;

//----------------------------------------------------------------------------
// mafOpRemoveCells :
//----------------------------------------------------------------------------
/** 
Perform cells selection and removing on surfaces.
CTRL modifier must be used in order to select cells in render window while
performing mouse dragging.
This operation assumes that input polygonal data is made of triangles only.

@sa mafOpRemoveCellsTest

@todo Extract selection related stuff to class
*/
class MAF_EXPORT mafOpRemoveCells: public mafOp
{
public:
	
  mafOpRemoveCells(wxString label = "RemoveCells");
 ~mafOpRemoveCells(); 
	
  mafTypeMacro(mafOpRemoveCells, mafOp);

  mafOp* Copy();

  void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
  bool Accept(mafVME* vme);

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

  mafGUIDialog		*m_Dialog;
	mafRWI      *m_Rwi;
  vtkPolyDataMapper *m_PolydataMapper;
  vtkActor          *m_PolydataActor;
  mafInteractorSelectCell *m_SelectCellInteractor;
  vtkMAFRemoveCellsFilter *m_Rcf;
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
