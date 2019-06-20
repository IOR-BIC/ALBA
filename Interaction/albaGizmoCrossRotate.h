/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoCrossRotate
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
 

//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 


#ifndef __albaGizmoCrossRotate_H__
#define __albaGizmoCrossRotate_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaGizmoInterface.h"
#include "albaGUIGizmoRotate.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGizmoCrossRotateAxis;
class albaGizmoCrossRotateFan;
class albaInteractorGenericInterface;
class albaMatrix;

//----------------------------------------------------------------------------
/** 

  Gizmo used to perform constrained rotation around an axis.

*/
class albaGizmoCrossRotate : public albaGizmoInterface 
{
public:
           albaGizmoCrossRotate(albaVME *input, albaObserver* listener = NULL, bool buildGUI = true, int axis = X);
  virtual ~albaGizmoCrossRotate(); 

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

  //----------------------------------------------------------------------------

  /**
  Set the gizmo pose*/
  void SetAbsPose(albaMatrix *absPose, bool applyPoseToFans = true);
  albaMatrix *GetAbsPose();

  /**
  Get the Interactor asscociated to each axis*/
  albaInteractorGenericInterface *GetInteractor(int axis);

  //----------------------------------------------------------------------------
  // RefSys
  //----------------------------------------------------------------------------

  /** Set the vme to be used as reference system, the vme is referenced; default ref sys is vme abs matrix */
  void SetRefSys(albaVME *refSys);
  albaVME* GetRefSys();;

  albaGUI *GetGui() {return m_GuiGizmoRotate->GetGui();};

  /** Modify radius of circles*/
  void SetCircleFanRadius(double radius);
  double GetCircleFanRadius();

  albaGizmoCrossRotateAxis * GetGizmoCrossRotateAxisNS() { return m_GizmoCrossRotateAxisNS; }
  albaGizmoCrossRotateAxis * GetGizmoCrossRotateAxisEW() { return m_GizmoCrossRotateAxisEW; }
  
protected:
  albaGUIGizmoRotate *m_GuiGizmoRotate;

  /** Send matrix to postmultiply to listener */
  void SendTransformMatrixFromGui(albaEventBase *alba_event);

  /** Gizmo components events handling */
  void OnEventGizmoGui(albaEventBase *alba_event);

  /** Gizmo components events handling */
  void OnEventGizmoComponents(albaEventBase *alba_event);

  /** 
  gizmo components enum*/
  enum AXIS {X = 0, Y, Z};

  enum ACTIVE_COMPONENT {NONE = -1, X_AXIS = 0, Y_AXIS, Z_AXIS, NUM_COMPONENTS};

  /**
  Highlight one component and dehighlight other components*/
  void Highlight(bool highlight); 
  
  /** Array holding the three gizmo that performs rotation*/  
  albaGizmoCrossRotateAxis *m_GizmoCrossRotateAxisNS;
  albaGizmoCrossRotateAxis *m_GizmoCrossRotateAxisEW;

  /** The rotating fan gizmo*/
  albaGizmoCrossRotateFan *m_GizmoCrossRotateFan;

  /** Build Gizmo GUI */
  bool m_BuildGUI;


  double m_CircleFanRadius;

};
#endif
