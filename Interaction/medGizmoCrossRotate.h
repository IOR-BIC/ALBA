/*=========================================================================

 Program: MAF2Medical
 Module: medGizmoCrossRotate
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

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


#ifndef __medGizmoCrossRotate_H__
#define __medGizmoCrossRotate_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafGizmoInterface.h"
#include "mafGUIGizmoRotate.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class medGizmoCrossRotateAxis;
class medGizmoCrossRotateFan;
class mafInteractorGenericInterface;
class mafMatrix;

//----------------------------------------------------------------------------
/** 

  Gizmo used to perform constrained rotation around an axis.

*/
class medGizmoCrossRotate : public mafGizmoInterface 
{
public:
           medGizmoCrossRotate(mafVME *input, mafObserver* listener = NULL, bool buildGUI = true, int axis = X);
  virtual ~medGizmoCrossRotate(); 

  /** 
  Set input vme for the gizmo*/
  void SetInput(mafVME *vme);

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
  void OnEvent(mafEventBase *maf_event);
 
  //----------------------------------------------------------------------------
  // show 
  //----------------------------------------------------------------------------
  
  /**
  Show the gizmo*/
  void Show(bool show);

  //----------------------------------------------------------------------------

  /**
  Set the gizmo pose*/
  void SetAbsPose(mafMatrix *absPose, bool applyPoseToFans = true);
  mafMatrix *GetAbsPose();

  /**
  Get the Interactor asscociated to each axis*/
  mafInteractorGenericInterface *GetInteractor(int axis);

  //----------------------------------------------------------------------------
  // RefSys
  //----------------------------------------------------------------------------

  /** Set the vme to be used as reference system, the vme is referenced; default ref sys is vme abs matrix */
  void SetRefSys(mafVME *refSys);
  mafVME* GetRefSys();;

  mafGUI *GetGui() {return m_GuiGizmoRotate->GetGui();};

  /** Modify radius of circles*/
  void SetCircleFanRadius(double radius);
  double GetCircleFanRadius();

  medGizmoCrossRotateAxis * GetGizmoCrossRotateAxisNS() { return m_GizmoCrossRotateAxisNS; }
  medGizmoCrossRotateAxis * GetGizmoCrossRotateAxisEW() { return m_GizmoCrossRotateAxisEW; }
  
protected:
  mafGUIGizmoRotate *m_GuiGizmoRotate;

  /** Send matrix to postmultiply to listener */
  void SendTransformMatrixFromGui(mafEventBase *maf_event);

  /** Gizmo components events handling */
  void OnEventGizmoGui(mafEventBase *maf_event);

  /** Gizmo components events handling */
  void OnEventGizmoComponents(mafEventBase *maf_event);

  /** 
  gizmo components enum*/
  enum AXIS {X = 0, Y, Z};

  enum ACTIVE_COMPONENT {NONE = -1, X_AXIS = 0, Y_AXIS, Z_AXIS, NUM_COMPONENTS};

  /**
  Highlight one component and dehighlight other components*/
  void Highlight(bool highlight); 
  
  /** Array holding the three gizmo that performs rotation*/  
  medGizmoCrossRotateAxis *m_GizmoCrossRotateAxisNS;
  medGizmoCrossRotateAxis *m_GizmoCrossRotateAxisEW;

  /** The rotating fan gizmo*/
  medGizmoCrossRotateFan *m_GizmoCrossRotateFan;

  /** Build Gizmo GUI */
  bool m_BuildGUI;


  double m_CircleFanRadius;

};
#endif
