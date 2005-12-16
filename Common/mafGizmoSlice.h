/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoSlice.h,v $
  Language:  C++
  Date:      $Date: 2005-12-16 18:54:17 $
  Version:   $Revision: 1.1 $
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
class mafVMEGizmo;
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

  /** return the mflVMEGizmo */
	mafVME *GetOutput(); 

  /** Set/Get the gizmo moving modality, default is Snap */
  void SetGizmoModalityToBound(); 
  void SetGizmoModalityToSnap(); 
  int  GetGizmoMovingModality() {return this->MovingModality; };

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
  mafVME             *m_vme_input;
	mafVMEGizmo        *m_vme_gizmo;
  mmiCompositorMouse *m_behavior;
	vtkDoubleArray     *m_snapArray;

  mafObserver   *m_Listener;
  mafMatrix      GizmoHandleCenterMatrix;
  vtkPoints		  *m_point;

  /** Register the gizmo axis, default is z */
  int m_axis;

  /** 
  Register the gizmo behavior: BOUND means gizmo movement is free
  in an interval while SNAP snaps on a given array */
  int MovingModality;

  enum MOVING_MODALITY_ID
  {
    BOUND,
    SNAP,
  };
  mmiGenericMouse *MouseBH;
};
#endif
