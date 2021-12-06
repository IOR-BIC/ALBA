/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMeshDeformation
 Authors: Josef Kohout
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpMeshDeformation_H__
#define __albaOpMeshDeformation_H__

#include "albaDefines.h"
#include "albaOp.h"
#include "mmiVTKPicker.h"

#define DEBUG_albaOpMeshDeformation
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaGUIButton;
class albaGUIDialog;
class albaRWI;
class albaPolylineGraph;
class vtkPolyDataMapper;
class vtkActor;
class vtkAppendPolyData;
class vtkPolyData;
class vtkIdList;
class vtkGlyph3D;
class vtkTubeFilter;
class vtkSphereSource;
class vtkCellPicker;

//----------------------------------------------------------------------------
// albaOpMeshDeformation :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpMeshDeformation: public albaOp
{
protected:
  //----------------------------------------------------------------------------
  // Constants:
  //----------------------------------------------------------------------------
  enum GUI_IDS
  {
    ID_RESET = MINID,
    ID_UNDO,
    ID_PREVIEW,
    ID_CREATE_CURVES,
    ID_SELECT_OC,
    ID_SELECT_DC,
    ID_CORRESPONDENCE,
    ID_RESETCURVE,
    ID_ADDCURVE,
    ID_EDITMODE,
    ID_EDITMODE_HELP,
    ID_EDITMODE_INFO,
    ID_ENABLE_OC_EDIT,
    ID_SHOW_OC,
    ID_SHOW_DC,
    ID_SHOW_CC,
    ID_SHOW_OM,
    ID_SHOW_DM,
    ID_SHOW_XX_CHANGE,
    ID_SAVEOC,
    ID_SAVEDC,
    ID_DEFORMATIONMODE, 
    ID_DEFORMATIONMODE_HELP,
    ID_OK,
    ID_CANCEL,
  };

  //This must match combobox values sets in CreateOpDialog
  enum EDIT_MODES 
  {
    EDM_ADD_CURVE = -1, //special
    EDM_SELECT = 0,
    EDM_MOVE_POINTS,
    EDM_ADD_POINT,
    EDM_DELETE_POINT,
    EDM_ADD_CORRESPONDENCE,
    EDM_DELETE_CORRESPONDENCE,
    EDM_DELETE_CURVE,  
#ifdef DEBUG_albaOpMeshDeformation
    EDM_SELECT_MESH_VERTEX,    
#endif
  };

  //this must match combobox values sets in CreateOpDialog
  enum DEFORMATION_MODES
  {
    DEM_BLANCO,             //Implementation of Blanco's method
    DEM_SEPPLANES,          //Has precise separating planes between edges
    DEM_WARPING,            //Simple warping
  };

#pragma region //Help structures
  typedef struct UNDO_ITEM 
  {
    int nCurveIndex;          //<which curve was affected
    int nAction;              //<LOWORD(action) is the action that was performed + HIWORD(action) denotes what was affected
                              //LOWORD(action) corresponds to EDIT_MODES,
                              //HIWORD(action) is bit mask, bit 0 corresponds to pOC_DC[0], 
                              //bit 1 - pOC_DC[1] and bit 2 - pCC

    albaPolylineGraph* pOC_DC[2];  //<original and deformed curve (in unmodified version)    
    vtkIdList* pCC;               //<correspondences (in unmodified version)
  } UNDO_ITEM;
  
  typedef struct CONTROL_CURVE
  {  
    albaPolylineGraph* pPolyLines[2];    //<polyline curves (0 = original, 1 = deformed)
    vtkIdList* pCCList;                 //<list of correspondences between curves

    vtkPolyData* pPolys[3];             //<polydata constructed from m_pPolyLine and m_pCCList
    vtkGlyph3D* pGlyphs[2];             //<input is m_pPoly, output is set of glyphs for every vertex
    vtkTubeFilter* pTubes[3];           //<input it m_pPoly, output is data with thick lines
    vtkActor* pActors[3];               //<m_pGlyph + m_pTube goes into vtkAppendPolyData
                                        //which goes into vtkPolyDataMapper and into m_pActor        
  } CONTROL_CURVE;

  typedef struct MESH
  {
    vtkPolyData* pPoly;                 //<polydata of the mesh
    vtkPolyDataMapper* pMapper;         //<mapper
    vtkActor* pActor;                   //<and its actor
  } MESH;

#pragma endregion //Help structures

public:
	albaOpMeshDeformation(const wxString &label = "Mesh deformation");
	~albaOpMeshDeformation(); 

	/*virtual*/ void OnEvent(albaEventBase *alba_event);

	albaTypeMacro(albaOpMeshDeformation, albaOp);

	/*virtual*/ albaOp* Copy();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Makes the undo for the operation. */
	/*virtual*/ void OpUndo();

#pragma region //Input Control Curves
  /** Get the number of control curves. */
  inline virtual int GetNumberOfControlCurves() {
    return m_NumberOfCurves;
  }

  /** Sets the number of control curves.
  Sets the number of control VMEs (polylines or polylinegraphs).
  Old VMEs are copied (and preserved) */
  virtual void SetNumberOfControlCurves(int nCount);
  
  /** Specifies the n-th control curve. 
  See: SetNthOriginalControlCurve, SetNthDeformedControlCurve and
  SetNthControlCurveCorrespondence for detailed information */
  inline virtual void SetNthControlCurve(int num,  albaVME* original,
    albaVME* modified, vtkIdList* correspondence);

  /** Specifies the n-th control curve in its original (undeformed) state. */
  virtual void SetNthOriginalControlCurve(int num, albaVME* input);

  /** Specifies the n-th control curve in its deformed state. */
  virtual void SetNthDeformedControlCurve(int num, albaVME* input);

  /** Sets the correspondence between the n-th original and deformed curve.
  The list contains pairs of indices of vertices of original-deformed 
  curve that correspond to each other. 
  N.B. if not specified, it is assumed that first vertices correspond. */
  virtual void SetNthControlCurveCorrespondence(int num, vtkIdList* matchlist);

  /** Returns an array with all control curves in their original state. */
  inline virtual albaVME** GetOriginalControlCurves() {
    return m_OriginalCurves;
  }

  /** Returns an array with all control curves in their deformed state. */
  inline virtual albaVME** GetDeformedControlCurves() {
    return m_DeformedCurves;
  }

  /** Returns an array with all correspondences for control curves. */
  inline virtual vtkIdList** GetControlCurvesCorrespondence() {
    return m_CurvesCorrespondence;
  }
#pragma endregion //Input Control Curves    

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);

  /** Creates internal data structures used in the editor.
  Returns false, if an error occurs (e.g. unsupported input) */
  virtual bool CreateInternalStructures();

  /** Destroys internal data structures created by CreateInternalStructures */
  virtual void DeleteInternalStructures();

  /** Creates GUI including renderer window */
	void CreateOpDialog();

  /** Destroys GUI */
	void DeleteOpDialog();

  /** This method creates the internal mesh structure.
  It constructs also the VTK pipeline. */
  MESH* CreateMesh(vtkPolyData* pMesh);

  /** Creates the control curve structure.
  It constructs also the VTK pipeline.*/
  CONTROL_CURVE* CreateControlCurve(albaPolylineGraph* pOC, 
    albaPolylineGraph* pDC, vtkIdList* pCC);

  /** Updates the VTK pipeline for the given curve.
  N.B. It does not connects actors into the renderer. */
  void UpdateMesh(MESH* pMesh);

  /** Updates the VTK pipeline for the given curve.
  Flags define what needs to be updated, bit 0 corresponds to the
  original curve, 1 to deformed and 2 to correspondence 
  N.B. It does not connects actors into the renderer.*/
  void UpdateControlCurve(CONTROL_CURVE* pCurve, int flags = -1);

  /** Updates the visibility of the given control curve. 
  It adds/removes actors from the renderer according to the status of curves
  and the visual options specified by the user in the GUI.
  N.B. This is typically called when curve was updated. */
  void UpdateControlCurveVisibility(CONTROL_CURVE* pCurve);  

  /** Updates the visibility of meshes and control curves.
  It adds/removes actors from the renderer according to the
  status of their associated data and the visual options
  specified by the user in the GUI.
  This method is typically calls after UpdateMesh or 
  UpdateControlCurve is finished */
  void UpdateVisibility();

  /** Updates the visibility of the selected items.
  Called automatically from UpdateVisibility and UpdateControlCurveVisibility*/
  void UpdateSelectionVisibility();  
  	
  /** Removes all actors from the renderer. */
  void RemoveAllActors();  

  /** Deforms the input mesh producing output mesh */
  void DeformMesh();

  /** Template for various methods */
  template < class T >
  void DeformMeshT();

#pragma region //Edit operations
  /** Adds a new control curve.
  N.B. this does everything. It creates VTK pipeline, internal structures,
  undo item for this operation and also updates the visibility. */
  void AddControlCurve(albaPolylineGraph* pOC, albaPolylineGraph* pDC, vtkIdList* pCC);

  /** Deletes the control curve at the given index.
  N.B. this does everything. It destroys VTK pipeline and
  creates undo item for this operation. */
  void DeleteControlCurve(int index);

  /** Finds the point on iType curve (0 for OC, 1 for DC) closest to the given position.
  Returns -1, if there is no point within the glyph tolerance */
  int FindPoint(CONTROL_CURVE* pCurve, int iType, double pos[3]);

  /** Selects an original curve, a deformed curve, a vertex or a correspondence.  
  The selection is done for the given control curve and iType is 0 for OC,
  1 for DC and 2 for CC; pos is the position where the pick event happened*/
  void SelectControlCurve(CONTROL_CURVE* pCurve, int iType, double pos[3]);

  /** Initializes the movement of the point denoted by pos. 
  The curve is defined by its index and iType is 0 for OC, 1 for DC
  After the calling of this routine, any number of MovePoint routines
  is called to move the point. The process ends when EndMovePoint is called */
  void BeginMovePoint(int index, int iType, double pos[3]);

  /** Finalizes the movement of the point
  N.B. see BeginMovePoint */
  void EndMovePoint();

  /** Moves the current point into the new position
  N.B. see BeginMovePoint  */
  void MovePoint(double pos[3]);

  /** Adds a new point with the specified coordinates on a curve. 
  The curve is defined by its index and iType is 0 for OC, 1 for DC
  The point is inserted into the closest segment */
  void AddPoint(int index, int iType, double pos[3]);

  /** Removes the existing point with the specified coordinates from a curve
  The curve is defined by its index and iType is 0 for OC, 1 for DC */
  void DeletePoint(int index, int iType, double pos[3]);

  /** Creates an undo item for the given curve. */
  void CreateUndo(int index, int nAction, int nFlags);

  /** Initializes the correspondence construction */
  void BeginAddCorrespondence(int index, int iType, double pos[3]);

  /** Finalizes the correspondence construction */
  void EndAddCorrespondence(int index, int iType, double pos[3]);  

  /** Finds a correspondence for the given point from the specified curve
  It returns -1, if there is no such correspondence; index to CC list otherwise */  
  int FindCorrespondence(vtkIdList* pCL, int ptIndex, int iCurve);

  /** Deletes the correspondence at the given index */
  void DeleteCorrespondence(vtkIdList*& pCL, int index);

  /** Deletes the correspondence close to the given point coordinates
  The control curve to be modified is denoted by the given index */
  void DeleteCorrespondence(int index, double pos[3]);
#pragma endregion  //Edit operations
  //GUI handlers
protected:
  /** Generates control curves for the mesh.
  All existing control curves are removed */
  virtual void OnCreateCurves();

  /** Opens VMEChoose dialog and selects new original curve. */
  virtual void OnSelectOC();

  /** Opens VMEChoose dialog and selects new deformed curve. */
  virtual void OnSelectDC();  

  /** Clears everything form OC, DC and CC edits. */
  virtual void OnResetCurve();

  /** Adds new control curve. */
  virtual void OnAddCurve();  

  /** Called when the edit mode changes.
  Updates the EditModeHelp text */
  virtual void OnEditMode();

  /** Called when the deformation mode changes.
  Updates the DeformationModeHelp text */
  virtual void OnDeformationMode();

  /** Called when the user picks something. */
  virtual void OnPick(vtkCellPicker* cellPicker);

  /** Undoes every action performed after the last adding/deletion of control curve 
  It undoes movement of points, adding/deletion of vertices and correspondences
  N.B. The default implementation calls OnUndo */
  virtual void OnReset();  

  /** Undoes the last action */
  virtual void OnUndo();

  /** Performs the deformation using the current settings. */
  virtual void OnPreview();

  /** Performs the deformation and creates outputs */
  virtual void OnOk();

  ////TODO: to be removed
  //void TestCode();

protected:
  /** Selects VME of supported type for control curve 
  Returns NULL, if the user cancels the selection. */
  virtual albaVME* SelectCurveVME();

  /** Callback for VME_CHOOSE that accepts polylines only */
  static bool SelectCurveVMECallback(albaVME *vme);

  /** Creates albaPolyLineGraph for the given vme */
  albaPolylineGraph* CreatePolylineGraph(albaVME* vme);

  /** Creates a deep copy of polylinegraph  */
  albaPolylineGraph* CreateCopyOfPolylineGraph(albaPolylineGraph* input);

  
protected:  
#pragma region //Input Control Curves
  int m_NumberOfCurves;                   //<the number of control curves
  albaVME** m_OriginalCurves;             //<original curves that correspond to the input mesh (m_Input)
  albaVME** m_DeformedCurves;             //<deformed curves that correspond to the output mesh
  vtkIdList** m_CurvesCorrespondence;    //<correspondence between vertices of original and deformed curves
#pragma endregion

#pragma region //Visualization stuff
  albaGUIDialog						*m_Dialog;     //<dialog - GUI
  albaRWI									*m_Rwi;	       //<rendering window
  mmiVTKPicker					  *m_Picker;     //<picks VTK objects

  vtkSphereSource* m_Spheres[2];        //<this is the source for glyph of vertices
  double m_SphereRadius[2];             //<this is its radius

  MESH* m_Meshes[2];                       //<original and deformed mesh    
  std::vector< CONTROL_CURVE* > m_Curves;  //< control curves
  std::vector< UNDO_ITEM* > m_UndoStack;   //< undo stack
  
  CONTROL_CURVE* m_SelectedCurve;          //<the currently selected curve
  int m_SelectedObjType;                   //<the selected object type: -1 nothing, 0 OC, 1 DC, 2 OC vertex, 3 DC vertex, 4 CC tube
  int m_SelectedObjId;                     //<index of the selected item

  vtkSphereSource* m_SelPointGlyph;       //<red sphere to announce the position of selected vertex
  vtkActor* m_SelPointActor;              //<actor for the selected vertex

  albaVME* m_OCToAdd;    //<this is the VME of the original curve selected to be added
  albaVME* m_DCToAdd;    //<this is the VME of the deformed curve selected to be added

  int m_EditMode;               //<the current edit mode
  int m_MeshesVisibility[2];    //<original and deformed mesh visibility
  int m_CurvesVisibility[3];    //<original, deformed curve + correspondences visibility

  int m_DeformationMode;        //<method that should be used for the deformation
  int m_SaveODC[2];             //<non-zero, if the original (0) or deformed (1) curves should be saved

  bool m_BPointMoveActive;          //<true, if the point is currently in the move 
  double m_PointMoveCorrection[3];  //<correction of the movement

  bool m_BCorrespondenceActive;     //<true, if the correspondence construction is active

  bool m_BDoNotCreateUndo;          //<true, if new items should not be created

#pragma region //GUI Controls  
  wxButton* m_BttnReset;
  wxButton* m_BttnUndo;
  wxButton* m_BttnPreview;    
  wxStaticText* m_OCNameCtrl;    
  wxStaticText* m_DCNameCtrl;
  wxButton* m_BttnSelDC;  
  wxTextCtrl* m_CCCtrl;  
  wxButton* m_BttnGenCurves;
  wxButton* m_BttnResetCurve;
  wxButton* m_BttnAddCurve;    
  wxTextCtrl* m_EditModeHelp;
  wxStaticText* m_EditModeInfo;
  wxCheckBox* m_ChckEditOC;
  wxTextCtrl* m_DeformationModeHelp;

  wxButton* m_BttnOK;  
#pragma endregion //GUI Controls
#pragma endregion //Visualization stuff
};

#pragma region //Input Control Curves
//------------------------------------------------------------------------
//Specifies the n-th control curve. 
//See: SetNthOriginalControlCurve, SetNthDeformedControlCurve and
//SetNthControlCurveCorrespondence for detailed information
inline /*virtual*/ void albaOpMeshDeformation::SetNthControlCurve(int num,  albaVME* original,
                                       albaVME* modified, vtkIdList* correspondence)
//------------------------------------------------------------------------
{
  SetNthOriginalControlCurve(num, original);
  SetNthDeformedControlCurve(num, modified);
  SetNthControlCurveCorrespondence(num, correspondence);
}
#pragma endregion //Input Control Curves

#endif
