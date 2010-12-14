/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGizmoCrossTranslate.h,v $
  Language:  C++
  Date:      $Date: 2010-12-14 17:33:06 $
  Version:   $Revision: 1.1.2.7 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
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




#ifndef __medGizmoCrossTranslate_H__
#define __medGizmoCrossTranslate_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafGizmoInterface.h"
#include "mafGUIGizmoTranslate.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class medGizmoCrossTranslateAxis;
class medGizmoCrossTranslatePlane;
class mafMatrix;

/** 

 Gizmo used to perform constrained translation 

*/

class medGizmoCrossTranslate : public mafGizmoInterface 
{
public:


  enum NORMAL {X = 0, Y, Z};

           medGizmoCrossTranslate(mafVME *input, mafObserver* listener = NULL, bool BuildGUI = true, int normal = X);
  virtual ~medGizmoCrossTranslate(); 

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
  Set/Get the gizmo pose*/
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
  enum PLANE {XNORMAL = 0, YNORMAL, ZNORMAL};
  enum ACTIVE_COMPONENT {NONE = -1, AXIS_0 = 0 , AXIS_1 = 1,PLANE = 3};
 
  /**
  Highlight one component and dehighlight other components*/
  void Highlight(int component); 
  
  /** Array holding the three gizmo that performs translation on a vector*/  
  medGizmoCrossTranslateAxis *m_GTAxis0;
  medGizmoCrossTranslateAxis *m_GTAxis1;

  /** Array holding the three gizmo that performs translation on a plane*/  
  medGizmoCrossTranslatePlane *m_GTPlane;

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
