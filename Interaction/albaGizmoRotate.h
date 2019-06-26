/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoRotate
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
 
#ifndef __albaGizmoRotate_H__
#define __albaGizmoRotate_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaGizmoInterface.h"
#include "albaGUIGizmoRotate.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGizmoRotateCircle;
class albaGizmoRotateFan;
class albaInteractorGenericInterface;
class albaMatrix;

//----------------------------------------------------------------------------
/** Gizmo used to perform constrained rotation around an axis.
 This class create a rotation gizmo composed of:
  
 3 albaGizmoRotateCircle
 3 albaGizmoRotateFan
 
      y                             y
      ^                             ^  
      |                             |  
      | ___                         |     /|
      |     \              +        |   /  |
      |      |                      | /    | 
      z--------> x                  z--------->x
        
  3 x albaGizmoRotateCircle          3 x albaGizmoRotateFan
  (single axis rotation       (visual feedback for rotation angle on one axis) 
   constrain)

      
      ^  
      |-----
      |     /\
  =   |   /    \         
      | /       |         
       ---------->    
  
   1 x albaGizmoRotate
     

  and forward pose matrixes to the listener operation. 
  This object works by creating smaller components that are parented to the
  vme tree root ie works in global coordinates.
  in order to use it in your client create an instance of it and concatenate
  in post multiply modality matrix that this object is sending to the the
  vme you want to move.

*/

class ALBA_EXPORT albaGizmoRotate : public albaGizmoInterface 
{
public:
           albaGizmoRotate(albaVME *input, albaObserver* listener = NULL, bool buildGUI = true);
  virtual ~albaGizmoRotate(); 

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
  void Show(bool showX, bool showY, bool showZ);

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
  void Highlight(int component); 
  
  /** Array holding the three gizmo that performs rotation*/  
  albaGizmoRotateCircle *m_GRCircle[3];

  /** The rotating fan gizmo*/
  albaGizmoRotateFan *m_GRFan[3];

  /** Build Gizmo GUI */
  bool m_BuildGUI;

  /** test friend */
  friend class albaGizmoRotateTest;
  
  double m_CircleFanRadius;

};
#endif
