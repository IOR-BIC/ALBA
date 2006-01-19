/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoSlice.h,v $
  Language:  C++
  Date:      $Date: 2006-01-19 14:14:50 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani, Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGizmoSlice_h
#define __mafGizmoSlice_h

#include "mafObserver.h"
#include "mafMatrix.h"

//---------------------------------------------------------------------------
// forward refs:
//---------------------------------------------------------------------------
class mmiCompositorMouse;
class mmiGenericMouse;
class mafVME;
class mafVMESurface;
class vtkPoints;
class vtkDoubleArray;

/** Gizmo used to perform volume slicing.
    @sa mafViewRXCT, mafViewOrthoSlice
*/
class mafGizmoSlice : public mafObserver
{
public:
					 mafGizmoSlice(mafNode* vme, mafObserver *Listener = NULL);
  virtual	~mafGizmoSlice();
    
	/** Create the plane, set its id and its position */
	void SetSlice(int id, int axis, double pos);

	/** Set the slice color */
	void SetColor(double col[3]);
	
	void SetListener(mafObserver *listener);
  void OnEvent(mafEventBase *maf_event);

  /** return the mafVMEGizmo */
	mafVME *GetOutput(); 

  /** Set/Get the gizmo moving modality, default is Snap */
  void SetGizmoModalityToBound(); 
  void SetGizmoModalityToSnap(); 
  int  GetGizmoMovingModality() {return this->m_MovingModality;};

  enum GISMO_SLICE_AXIS_ID
  {
    GIZMO_SLICE_X = 0,
    GIZMO_SLICE_Y,
    GIZMO_SLICE_Z
  };	

protected:
	/** Initialize snap array */
	void InitSnapArray(mafVME *vol, int axis);

  long                m_id;
  mafVME             *m_VmeInput;
  mafVMESurface      *m_VmeGizmo;
  mmiCompositorMouse *m_GizmoBehavior;
	vtkDoubleArray     *m_SnapArray;

  mafObserver   *m_Listener;
  mafMatrix     *m_GizmoHandleCenterMatrix;
  vtkPoints		  *m_Point;

  /** Register the gizmo axis, default is z */
  int m_axis;

  /** 
  Register the gizmo behavior: BOUND means gizmo movement is free
  in an interval while SNAP snaps on a given array */
  int m_MovingModality;

  enum MOVING_MODALITY_ID
  {
    BOUND,
    SNAP,
  };
  mmiGenericMouse *m_MouseBH;
};
#endif
