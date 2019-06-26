/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGeometryEditorPolylineGraph
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGeometryEditorPolylineGraph_H__
#define __albaGeometryEditorPolylineGraph_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaObserver.h"
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"
#include "albaPolylineGraph.h"

#include "vtkSystemIncludes.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMEPolylineEditor;
class albaVMEPolylineGraph;
class albaPolylineGraph;
class albaGUI;
class albaInteractorPicker;
class albaInteractor;
class vtkSphereSource;
class vtkGlyph3D;
class vtkTubeFilter;
class vtkAppendPolyData;
class vtkPolyData;

#define UNDEFINED_POINT_ID -1
#define UNDEFINED_BRANCH_ID -1
/**
  class name: albaGeometryEditorPolylineGraph
  This class allows the editing of a polylinegraph (class created by BES).
  It creates ISA, can add, insert, remove, move point in the polyline graph,
  it can also add, remove branch in which points are.
*/
class ALBA_EXPORT albaGeometryEditorPolylineGraph: public albaObserver, public albaServiceClient 
{
public:
  /** contructor*/
	albaGeometryEditorPolylineGraph(albaVME *input=NULL, albaObserver *listener = NULL, albaVMEPolylineGraph *polyline=NULL,bool testMode=false);
	virtual ~albaGeometryEditorPolylineGraph(); 

  enum EDITOR_GRAPH_ID
  {
    ID_POINT_TOOL = MINID,
    ID_BRANCH_TOOL,
    ID_ACTION,
    ID_BUTTON_POINT_DELETE,
    ID_BUTTON_BRANCH_DELETE,
    ID_SPHERE_RADIUS,
  };

  enum POINT_TOOL_ID
  {
	ID_ADD_POINT = 0,
	ID_INSERT_POINT,
	ID_MOVE_POINT,
	ID_SELECT_POINT,
  };
  enum BRANCH_TOOL_ID
  {
	ID_ADD_BRANCH = 0,
	ID_SELECT_BRANCH,
  };
  enum ACTION_ID
  {
	ID_POINT_ACTION = 0,
	ID_BRANCH_ACTION,
  };

	/** Set the event receiver object*/
	void  SetListener(albaObserver *Listener) {m_Listener = Listener;};

	/** Events handling*/        
	virtual void OnEvent(albaEventBase *alba_event);

	/** Show On/Off VME Editor */
	void Show(bool show);

	/** Return the current GUI */
	albaGUI* GetGui();

	/** Add a new vertex at the selected branch */
	int AddNewVertex(double vertex[3],vtkIdType branch=UNDEFINED_BRANCH_ID);

	/** Return the result of the editing */
	vtkPolyData* GetOutput();

	/** Delete a branch and his child branch */
	void DeleteBranch(vtkIdType branchID);

	/** Move a point in the new position. If pointID isn't defined it use m_SelectedPoint */
	void MovePoint(double newPosition[3],int pointID=UNDEFINED_POINT_ID);

	/** Select the nearest point to position */
	void SelectPoint(double position[3]);

	/** Select Point with the ID */
	void SelectPoint(int pointID);

	/** Return VTK ID of the point selected */
	int GetVtkIdSelectedPoint(){return m_SelectedPointVTK;};
	
	/** Delete the nearest point to position */
	int DeletePoint(double position[3]);

	/** Delete pointID-th point */
	int DeletePoint(int pointID=UNDEFINED_POINT_ID);

	/** Select the nearest branch to position */
	void SelectBranch(double position[3]);

	vtkIdType GetVtkIdSelectBranch(){vtkIdType cellID;m_PolylineGraph->GetOutputCellCorrespondingToBranch(m_SelectedBranch,&cellID);return cellID;};

	/** Attach a new branch to the selected point and add a point (defined by position) to the new branch */
	int AddBranch(double position[3]);

	/** Insert a point defined by position between two points selected before */
	int InsertPoint(double position[3]);

  /** Enable Test Mode*/
	void SetTestModeOn(){m_TestMode=true;};
  /** Disable Test Mode*/
	void SetTestModeOff(){m_TestMode=false;};

  /** set radius dimension */
  void SetRadius(double radius){m_SphereRadius = radius; OnEvent(&albaEvent(this,ID_SPHERE_RADIUS));}

protected:
  /** create gui */
	void CreateGui();

	/** Create isa stuff */
	void CreateISA();

  /** create pipe*/
	void CreatePipe();

  /** update dataset inside vme */
	int UpdateVMEEditorData(vtkPolyData *polydata);

	/** Manage if a VME is picked in the view */
	void VmePicked(albaEvent *e);

	/** Update VME Editor behavior and VME Input behavior */
	void BehaviorUpdate();

  /** compute distance to polyline */
  double ComputeDistancePointLine(double lineP0[3],double lineP1[3],double point[3]);

	/**
	Register the event receiver object*/
	albaObserver *m_Listener;

	albaInteractorPicker *m_Picker;
	albaInteractor *m_OldBehavior;

	albaVMEPolylineEditor			*m_VMEPolylineEditor;
	albaVMEPolylineEditor			*m_VMEPolylineSelection;///<VME that show the selection
	albaPolylineGraph					*m_PolylineGraph;

	albaGUI	*m_Gui;

	albaVME	*m_InputVME;

	int m_CurrentBranch;///<Branch in witch we are working
	int m_SelectedPoint;
	int m_SelectedPointVTK;
	int m_SelectedBranch;

	int m_SelectedPoint1ForInserting;
	int m_SelectedPoint2ForInserting;

	int m_SelectedPoint1ForInsertingVTK;
	int m_SelectedPoint2ForInsertingVTK;
	//Gui Variables
	int m_PointTool;
	int m_BranchTool;
	int m_Action;
  double m_SphereRadius;

	//Pipe Variables
	vtkSphereSource		*m_Sphere;
	vtkGlyph3D				*m_Glyph;
	vtkTubeFilter			*m_Tube;
	vtkAppendPolyData	*m_AppendPolydata;

	bool m_TestMode;
};
#endif
