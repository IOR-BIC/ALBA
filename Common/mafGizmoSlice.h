/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoSlice.h,v $
  Language:  C++
  Date:      $Date: 2008-06-12 10:02:55 $
  Version:   $Revision: 1.9 $
  Authors:   Paolo Quadrani, Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGizmoSlice_h
#define __mafGizmoSlice_h

#include "mafGizmoInterface.h"
#include "mafMatrix.h"

//---------------------------------------------------------------------------
// forward refs:
//---------------------------------------------------------------------------
class mmiCompositorMouse;
class mmiGenericMouse;
class mafVME;
class mafVMEGizmo;
class vtkPoints;
class vtkDoubleArray;

/** Gizmo typically used to move volume slices. It is reparented under the input vme ie it is working in
local coordinates.
@sa mafViewRXCT, mafViewOrthoSlice for usage examples: these views act as mediators between gizmo slices
*/
class mafGizmoSlice : public mafGizmoInterface
{
public:

	mafGizmoSlice(mafNode* inputVme, mafObserver *Listener = NULL, const char *name = "GizmoSlice");

  virtual	~mafGizmoSlice();

  /** Show/Hide the gizmos using actor visibility instead of pipe creation/destruction: this is used for faster 
  rendering*/
  void Show(bool show);

  /** This method is used to change the input vme */
  void SetInput(mafVME *vme);

  /** Set the gizmo color */
  void SetColor(double col[3]);

  /** return the gizmo slice object */
  mafVME *GetOutput(); 

	/** Create the gizmo slice with its id on choosed position on choosed axis*/
	void CreateGizmoSliceInLocalPositionOnAxis(int gizmoSliceId, int axis, double positionOnAxis);
  
  enum GISMO_SLICE_AXIS_ID
  {
    GIZMO_SLICE_X = 0,
    GIZMO_SLICE_Y,
    GIZMO_SLICE_Z
  };	

  /** Set/Get the gizmo moving modality, default is Snap */
  void SetGizmoMovingModalityToBound(); 
  void SetGizmoMovingModalityToSnap(); 
  int  GetGizmoMovingModality() {return this->m_MovingModality;};
  
  /** Events handling method */
  void OnEvent(mafEventBase *maf_event);
 
  /** Gizmo is reparented under the vme tree root, this modality is currently not supported */
  void SetModalityToGlobal() {mafLogMessage("Global modality is currently not supported for this item");};

  void  SetPosition(mafMatrix *matrix);
  mafMatrix *GetPosition(){return m_GizmoHandleCenterMatrix;};
  

protected:

  void CreateGizmoSlice(mafNode *imputVme, mafObserver *listener, const char *name);

  void DestroyGizmoSlice();

	/** Initialize snap array */
	void InitSnapArray(mafVME *vol, int axis);

  /*Set Gizmo On correct slices when activate snap array modality*/
  void SetOnSnapArray(int axis);

  mafString           m_Name;
  long                m_Id;
  mafVMEGizmo        *m_VmeGizmo;
  mmiCompositorMouse *m_GizmoBehavior;
	vtkDoubleArray     *m_SnapArray;

  mafMatrix     *m_GizmoHandleCenterMatrix;
  vtkPoints		  *m_Point;

  /** Register the gizmo axis, default is z */
  int m_Axis;

  /** 
  Register the gizmo behavior: BOUND means gizmo movement is free
  in an interval while SNAP snaps on a given array */
  int m_MovingModality;

  enum MOVING_MODALITY_ID
  {
    BOUND = 0,
    SNAP,
    MOVING_MODALITY_NUMBERS,
  };
  mmiGenericMouse *m_MouseBH;
};
#endif
