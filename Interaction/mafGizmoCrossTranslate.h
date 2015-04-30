/*=========================================================================

 Program: MAF2
 Module: mafGizmoCrossTranslate
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




#ifndef __mafGizmoCrossTranslate_H__
#define __mafGizmoCrossTranslate_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafGizmoInterface.h"
#include "mafGUIGizmoTranslate.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGizmoCrossTranslateAxis;
class mafGizmoCrossTranslatePlane;
class mafMatrix;

/** 

 Gizmo used to perform constrained translation 

*/

class mafGizmoCrossTranslate : public mafGizmoInterface 
{
public:


  enum NORMAL {X = 0, Y, Z};

           mafGizmoCrossTranslate(mafVME *input, mafObserver* listener = NULL, bool BuildGUI = true, int normal = X);
  virtual ~mafGizmoCrossTranslate(); 

  /** 
  Set input vme for the gizmo*/
  void SetInput(mafVME *vme); 

  /** Superclass override */
  void SetRenderWindowHeightPercentage(double percentage);

  /** Superclass override */
  void SetAutoscale(bool autoscale);

  /** Superclass override */
  void SetAlwaysVisible(bool alwaysVisible);

  /** Constrain translation to Input VME bounds (default to true): the gizmo will move inside the input volume only  */
  void ConstrainTranslationToInputVMEBoundsOn() {m_ConstrainTranslationToInputVMEBounds = true;};
  void ConstrainTranslationToInputVMEBoundsOff() {m_ConstrainTranslationToInputVMEBounds = false;};

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

  /**
  Set/Get the gizmo pose: works by setting the pose to all gizmo subcomponents */
  void SetAbsPose(mafMatrix *absPose, mafTimeStamp ts = -1);
  mafMatrix *GetAbsPose();
  
  //----------------------------------------------------------------------------
  // RefSys
  //----------------------------------------------------------------------------

  /** Set the vme to be used as reference system, the vme is referenced; default ref sys is vme abs matrix */
  void SetRefSys(mafVME *refSys);
  mafVME* GetRefSys();

  mafGUI *GetGui() {return (m_GuiGizmoTranslate->GetGui());};

  /**
  Allowed constraint modality are:
  LOCK, FREE, BOUNDS, SNAP_STEP, SNAP_ARRAY defined in mafInteractorConstraint*/
  void SetConstraintModality(int constrainModality);
  int GetConstraintModality();

  /** Set the step value for snap step constraint type for the given axis*/
  void SetStep(double step);
  int GetStep();

  mafGizmoCrossTranslateAxis * GetGTUpDown() { return m_GTUpDown; }
  mafGizmoCrossTranslateAxis * GetGTLeftRight() { return m_GTLeftRight; }
  mafGizmoCrossTranslatePlane * GetGTPlane() const{ return m_GTPlane; }

protected:

  /** Normal to the cross gizmo */
  int m_Normal;

  /** Register if translation is constrained to input vme bounds */
  bool m_ConstrainTranslationToInputVMEBounds;

  /** Gizmo gui */
  mafGUIGizmoTranslate *m_GuiGizmoTranslate;

  /** Gizmo components events handling */
  void OnEventGizmoGui(mafEventBase *maf_event);

  /** Gizmo components events handling */
  void OnEventGizmoComponents(mafEventBase *maf_event);

  /** 
  gizmo components enum*/
  enum TRANSLATE_ON_PLANE {XNORMAL = 0, YNORMAL, ZNORMAL};

  /** 
  gizmo components enum
  
       up
       
       o o
        O
       \_/
  
  left     right
      
      down

  World Coordinates

   Z
   .
  / \
   |
   .---> X
   Y

  */
  
  enum ACTIVE_COMPONENT {NONE = -1, TRANSLATE_UP_DOWN = 0 , TRANSLATE_LEFT_RIGHT = 1,TRANSLATE_ON_PLANE = 3};
 
  /**
  Highlight one component and dehighlight other components*/
  void Highlight(int component); 
  
  /** Array holding the three gizmo that performs translation on a vector*/  
  mafGizmoCrossTranslateAxis *m_GTUpDown;  
  mafGizmoCrossTranslateAxis *m_GTLeftRight;
  /** Array holding the three gizmo that performs translation on a plane*/  
  mafGizmoCrossTranslatePlane *m_GTPlane;
  
  /** 
  Register the active gizmo component at MOUSE_DOWN*/
  int m_ActiveGizmoComponent;

  /**
  Pivot point position; */
  mafMatrix *m_PivotPose;

  /** Send matrix to postmultiply to listener */
  void SendTransformMatrixFromGui(mafEventBase *maf_event);
  
  /** Build Gizmo GUI */
  bool m_BuildGUI;

  /** register axis constraint modality */
  int m_ConstraintModality;

  /** register step on axes */
  int m_Step;

};
#endif
