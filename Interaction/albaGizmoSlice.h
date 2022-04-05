/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoSlice
 Authors: Paolo Quadrani, Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGizmoSlice_h
#define __albaGizmoSlice_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaGizmoInterface.h"
#include "albaMatrix.h"

//---------------------------------------------------------------------------
// forward refs:
//---------------------------------------------------------------------------
class albaInteractorCompositorMouse;
class albaInteractorGenericMouse;
class albaVME;
class albaVMEGizmo;
class vtkPoints;
class vtkDoubleArray;

/** Gizmo typically used to move volume slices. It is reparented under the input vme ie it is working in
local coordinates.
@sa albaViewRXCT, albaViewOrthoSlice for usage examples: these views act as mediators between gizmo slices
*/
class ALBA_EXPORT albaGizmoSlice : public albaGizmoInterface
{
public:

	albaGizmoSlice(albaVME* inputVme, albaObserver *Listener = NULL, const char *name = "GizmoSlice", bool inverseHandle = false, double centralClipfactor = 0);

  virtual	~albaGizmoSlice();

  /** Show/Hide the gizmos using actor visibility instead of pipe creation/destruction: this is used for faster 
  rendering*/
  void Show(bool show);

  /** This method is used to change the input vme */
  void SetInput(albaVME *vme);

  /** Set the gizmo color */
  void SetColor(double col[3]);

  /** Set the central clip length in order to create a less invasive gizmo*/
  void SetCentralClipFactor(double factor){m_CentralClipFactor = factor;};

  /** return the gizmo slice object */
  albaVME *GetOutput(); 

	/** Create the gizmo slice with its id on choosed position on choosed axis*/
	void UpdateGizmoSliceInLocalPositionOnAxis(int gizmoSliceId, int axis, double positionOnAxis, bool visibleCubeHandler = true);

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
  
  void SetGizmoEnableMoving(bool enable);

  /** Events handling method */
  void OnEvent(albaEventBase *alba_event);
 
  /** Gizmo is reparented under the vme tree root, this modality is currently not supported */
  void SetModalityToGlobal() {albaLogMessage("Global modality is currently not supported for this item");};

  void  SetPosition(albaMatrix *matrix);
  albaMatrix *GetPosition(){return m_GizmoHandleCenterMatrix;};

  /** Set matrix of the handle */
  void SetHandleCenterMatrix(albaMatrix *m);

	void SetEnableCustomizedSnapArray(bool enable){m_CustomizedSnapArrayFlag = enable;};
	void SetCustomizedSnapArrayStep(double step){m_CustomizedArrayStep = step;};

	double *GetBounds() { return m_Bounds; }
	void SetBounds(double *val);
protected:

  void CreateGizmoSlice(albaVME *imputVme, albaObserver *listener, const char *name, bool inverseHandle, double centralClipfactor);

  void DestroyGizmoSlice();

	double GetBorderCube();

	/** Initialize snap array */
	void InitSnapArray(albaVME *vol, int axis);

  /*Set Gizmo On correct slices when activate snap array modality*/
  void SetOnSnapArray(int axis);

  albaString           m_Name;
  long                m_Id;
  albaVMEGizmo        *m_VmeGizmo;
  albaInteractorCompositorMouse *m_GizmoBehavior;
	vtkDoubleArray     *m_SnapArray;
	double m_Bounds[6];

  albaMatrix     *m_GizmoHandleCenterMatrix;
  vtkPoints		  *m_Point;

	bool m_CustomizedSnapArrayFlag;
	double m_CustomizedArrayStep;

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
  albaInteractorGenericMouse *m_MouseBH;

  bool m_InverseHandle;

  double m_CentralClipFactor;
};
#endif
