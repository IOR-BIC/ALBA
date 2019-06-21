/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoTranslate
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGizmoTranslate_H__
#define __albaGizmoTranslate_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaGizmoInterface.h"
#include "albaGUIGizmoTranslate.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGizmoTranslateAxis;
class albaGizmoTranslatePlane;
class albaMatrix;

/** Gizmo used to perform constrained translation along the x, yz axis
 or the xy, xz, yz plane.

 This class create a translation gizmo composed of:
  
 3 albaGizmoTranslateAxis
 3 albaGizmoTranslatePlane

 
      z
      ^                                 S2
      |                                -----
      |                                     |         
      |                     +           SQ  |S1            =      
      |                                     |         
      x--------> y   
        
  3 x albaGizmoTranslateAxis          3 x albaGizmoTranslatePlane
  (single axis translation       (single plane translation constrain) 
   constrain)

  
      
      ^  
      |-----
      |     |         
  =   |     |          
      |     |         
       -------->    
  
   1 x albaGizmoTranslate
     

 and forward pose matrixes to the 
 listener operation. 

 This object works by creating smaller components that are parented to the
 vme tree root ie works in global coordinates.
 in order to use it in your client create an instance of it and concatenate
 in post multiply modality matrix that this object is sending to the the
 vme you want to move.


 @sa albaGizmoTranslateAxis, albaGizmoTranslatePlane
*/

class ALBA_EXPORT albaGizmoTranslate : public albaGizmoInterface 
{
public:
           albaGizmoTranslate(albaVME *input, albaObserver* listener = NULL, bool BuildGUI = true);
  virtual ~albaGizmoTranslate(); 

  /** 
  Set input vme for the gizmo*/
  void SetInput(albaVME *vme); 

  /** Superclass override */
  void SetRenderWindowHeightPercentage(double percentage);

  /** Superclass override */
  void SetAutoscale(bool autoscale);

  /** Superclass override */
  void SetAlwaysVisible(bool alwaysVisible);

  //----------------------------------------------------------------------------
  // events handling 
  //----------------------------------------------------------------------------

  /**
  Events handling*/        
  void OnEvent(albaEventBase *alba_event);
 
  //----------------------------------------------------------------------------
  // show 
  //----------------------------------------------------------------------------
  
  /**
  Show the gizmo*/
  void Show(bool show);

  /**
  Show the gizmo*/
  void Show(bool showX,bool showY,bool showZ);

  /**
  Set/Get the gizmo pose*/
  void SetAbsPose(albaMatrix *absPose, albaTimeStamp ts = -1);
  albaMatrix *GetAbsPose();
  
  //----------------------------------------------------------------------------
  // RefSys
  //----------------------------------------------------------------------------

  /** Set the vme to be used as reference system, the vme is referenced; default ref sys is vme abs matrix */
  void SetRefSys(albaVME *refSys);
  albaVME* GetRefSys();

  albaGUI *GetGui() {return (m_GuiGizmoTranslate->GetGui());};

  /**
  Set the constraint modality for the given axis; allowed constraint modality are:
  LOCK, FREE, BOUNDS, SNAP_STEP, SNAP_ARRAY defined in albaInteractorConstraint*/
  void SetConstraintModality(int axis, int constrainModality);
  int GetConstraintModality(int axis);

  /** Set the step value for snap step constraint type for the given axis*/
  void SetStep(int axis, double step);
  int GetStep(int axis);

protected:

  albaGUIGizmoTranslate *m_GuiGizmoTranslate;

  /** Gizmo components events handling */
  void OnEventGizmoGui(albaEventBase *alba_event);

  /** Gizmo components events handling */
  void OnEventGizmoComponents(albaEventBase *alba_event);

  /** 
  gizmo components enum*/
  enum AXIS {X = 0, Y, Z};
  enum PLANE {XNORMAL = 0, YNORMAL, ZNORMAL};
  enum ACTIVE_COMPONENT {NONE = -1, X_AXIS = 0, Y_AXIS, Z_AXIS, XN_PLANE, YN_PLANE, ZN_PLANE};
 
  /**
  Highlight one component and dehighlight other components*/
  void Highlight(int component); 
  
  /** Array holding the three gizmo that performs translation on a vector*/  
  albaGizmoTranslateAxis *m_GTAxis[3];

  /** Array holding the three gizmo that performs translation on a plane*/  
  albaGizmoTranslatePlane *m_GTPlane[3];

  /** 
  Register the active gizmo component at MOUSE_DOWN*/
  int m_ActiveGizmoComponent;

  /**
  Pivot point position; */
  albaMatrix *m_PivotPose;

  /** Send matrix to postmultiply to listener */
  void SendTransformMatrixFromGui(albaEventBase *alba_event);
  
  /** Build Gizmo GUI */
  bool m_BuildGUI;

  /** test friend */
  friend class albaGizmoTranslateTest;

  /** register axis constraint modality */
  int m_ConstraintModality[3];

  /** register step on axes */
  int m_Step[3];

};
#endif
