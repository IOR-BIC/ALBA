/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoTranslate.h,v $
  Language:  C++
  Date:      $Date: 2008-10-21 15:11:45 $
  Version:   $Revision: 1.5.2.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGizmoTranslate_H__
#define __mafGizmoTranslate_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafGizmoInterface.h"
#include "mafGUIGizmoTranslate.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGizmoTranslateAxis;
class mafGizmoTranslatePlane;
class mafMatrix;

/** Gizmo used to perform constrained translation along the x, yz axis
 or the xy, xz, yz plane.

 This class create a translation gizmo composed of:
  
 3 mafGizmoTranslateAxis
 3 mafGizmoTranslatePlane

 
      z
      ^                                 S2
      |                                -----
      |                                     |         
      |                     +           SQ  |S1            =      
      |                                     |         
      x--------> y   
        
  3 x mafGizmoTranslateAxis          3 x mafGizmoTranslatePlane
  (single axis translation       (single plane translation constrain) 
   constrain)

  
      
      ^  
      |-----
      |     |         
  =   |     |          
      |     |         
       -------->    
  
   1 x mafGizmoTranslate
     

 and forward pose matrixes to the 
 listener operation. 

 This object works by creating smaller components that are parented to the
 vme tree root ie works in global coordinates.
 in order to use it in your client create an instance of it and concatenate
 in post multiply modality matrix that this object is sending to the the
 vme you want to move.


 @sa mafGizmoTranslateAxis, mafGizmoTranslatePlane
*/

class mafGizmoTranslate : public mafGizmoInterface 
{
public:
           mafGizmoTranslate(mafVME *input, mafObserver* listener = NULL, bool BuildGUI = true);
  virtual ~mafGizmoTranslate(); 

  /** 
  Set input vme for the gizmo*/
  void SetInput(mafVME *vme); 


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
  Set the constraint modality for the given axis; allowed constraint modality are:
  LOCK, FREE, BOUNDS, SNAP_STEP, SNAP_ARRAY defined in mmiConstraint*/
  void SetConstraintModality(int axis, int constrainModality);
  int GetConstraintModality(int axis);

  /** Set the step value for snap step constraint type for the given axis*/
  void SetStep(int axis, double step);
  int GetStep(int axis);

protected:

  mafGUIGizmoTranslate *m_GuiGizmoTranslate;

  /** Gizmo components events handling */
  void OnEventGizmoGui(mafEventBase *maf_event);

  /** Gizmo components events handling */
  void OnEventGizmoComponents(mafEventBase *maf_event);

  /** 
  gizmo components enum*/
  enum AXIS {X = 0, Y, Z};
  enum PLANE {XNORMAL = 0, YNORMAL, ZNORMAL};
  enum ACTIVE_COMPONENT {NONE = -1, X_AXIS = 0, Y_AXIS, Z_AXIS, XN_PLANE, YN_PLANE, ZN_PLANE};
 
  /**
  Highlight one component and dehighlight other components*/
  void Highlight(int component); 
  
  /** Array holding the three gizmo that performs translation on a vector*/  
  mafGizmoTranslateAxis *m_GTAxis[3];

  /** Array holding the three gizmo that performs translation on a plane*/  
  mafGizmoTranslatePlane *m_GTPlane[3];

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

  /** test friend */
  friend class mafGizmoTranslateTest;

  /** register axis constraint modality */
  int m_ConstraintModality[3];

  /** register step on axes */
  int m_Step[3];

};
#endif
