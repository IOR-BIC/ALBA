/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoTranslate.h,v $
  Language:  C++
  Date:      $Date: 2005-12-07 11:21:32 $
  Version:   $Revision: 1.2 $
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
#include "mafGuiGizmoTranslate.h"

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

 @sa mafGizmoTranslateAxis, mafGizmoTranslatePlane
*/

class mafGizmoTranslate : public mafGizmoInterface 
{
public:
           mafGizmoTranslate(mafVME *input, mafObserver* listener = NULL);
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
  Set the gizmo pose*/
  void SetAbsPose(mafMatrix *absPose, mafTimeStamp ts = -1);

  mafMatrix *GetAbsPose();
  
  //----------------------------------------------------------------------------
  // RefSys
  //----------------------------------------------------------------------------

  /** Set the vme to be used as reference system, the vme is referenced; default ref sys is vme abs matrix */
  void SetRefSys(mafVME *refSys);
  mafVME* GetRefSys();

  mmgGui *GetGui() {return (GuiGizmoTranslate->GetGui());};

  /**
  Set the constraint modality for the given axis; allowed constraint modality are:
  LOCK, FREE, BOUNDS, SNAP_STEP, SNAP_ARRAY.*/
  void SetConstraintModality(int axis, int constrainModality);

  /** Set the step value for snap step constraint type for the given axis*/
  void SetStep(int axis, double step);

protected:

  mafGuiGizmoTranslate *GuiGizmoTranslate;

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
  mafGizmoTranslateAxis *GTAxis[3];

  /** Array holding the three gizmo that performs translation on a plane*/  
  mafGizmoTranslatePlane *GTPlane[3];

  /** 
  Register the active gizmo component at MOUSE_DOWN*/
  int ActiveGizmoComponent;

  /**
  Pivot point position; */
  mafMatrix *PivotPose;

  /** Send matrix to postmultiply to listener */
  void SendTransformMatrixFromGui(mafEventBase *maf_event);
};
#endif
