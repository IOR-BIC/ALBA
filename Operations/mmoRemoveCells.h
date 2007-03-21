/*========================================================================= 
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoRemoveCells.h,v $
  Language:  C++
  Date:      $Date: 2007-03-21 11:36:07 $
  Version:   $Revision: 1.2 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoRemoveCells_H__
#define __mmoRemoveCells_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "vtkIdList.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmgDialog;
class mmgFloatSlider;
class mafVMESurface;
class mafNode;
class mafRWI;

class vtkActor;
class vtkContourVolumeMapper;
class vtkPolyDataMapper;
class vtkOutlineCornerFilter;
class vtkRenderer;
class vtkCell;

class mmiSelectCell;
class vtkImageData;
class vtkTexture;
class vtkPolyData;
class vtkVolumeSlicer;
class vtkFixedCutter;
class vtkPlane;
class vtkVolume;
class vtkRemoveCellsFilter;

//----------------------------------------------------------------------------
// mmoRemoveCells :
//----------------------------------------------------------------------------
/** */
class mmoRemoveCells: public mafOp
{
public:
	mmoRemoveCells(wxString label = "RemoveCells");
 ~mmoRemoveCells(); 
	
  mafTypeMacro(mmoRemoveCells, mafOp);

  mafOp* Copy();
  void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode* vme);

	/** Builds operation's interface by calling CreateOpDialog() method. */
  void OpRun();

  void RemoveCells();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();

  void SetSeed(vtkIdType cellSeed);
  void MarkCells();

  /** Set the selection region radius */
  double GetDiameter() const { return m_Diameter; };
  void SetDiameter(double val) {m_Diameter = val;};

protected:

  double GetCellsDistance(vtkIdType c1,vtkIdType c2);

  void FindTriangleCellCenter(vtkIdType id, double center[3]);

  // used to support algorithm execution
  vtkIdList *m_NeighborCellPointIds;
  int *m_VisitedCells;
  vtkIdType *m_VisitedPoints;
  int m_RegionNumber;
  vtkIdType m_PointNumber;    
  vtkIdType m_CellSeed;
  vtkIdList *m_Wave;
  vtkIdList *m_Wave2;
  vtkIdList *PointIds;
  vtkIdList *CellIds;
  double m_Diameter;
 
  double m_MinBrushSize;
  double m_MaxBrushMSize;
  int m_UnselectCells;

  mmgDialog		*m_Dialog;
	mafRWI      *m_Rwi;
  vtkPolyDataMapper *m_PolydataMapper;
  vtkActor          *m_PolydataActor;
  mmiSelectCell *m_SelectCellInteractor;
  vtkRemoveCellsFilter *m_rcf;
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
  void TraverseMeshAndMark(double radius);
	void MarkCellsInRadius(double radius);

  void InitializeMesh();
  double EstimateTrianglesDimension();

};
#endif
