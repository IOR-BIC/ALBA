/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoScale.h,v $
  Language:  C++
  Date:      $Date: 2008-06-06 10:59:10 $
  Version:   $Revision: 1.4 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGizmoScale_H__
#define __mafGizmoScale_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafGizmoInterface.h"
#include "mafGuiGizmoScale.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGizmoScaleAxis;
class mafGizmoScaleIsotropic;
class mafMatrix;
class mafVME;
class mmgGui;

/** Gizmo used to perform constrained scaling along the x, yz axis
 or the xy, xz, yz plane.

 This class create a scaling gizmo composed of:
  
 3 mafGizmoScaleAxis

 
      z                                 
      ^                      ^              
      |                      | 
      |                      |               
      |                  =   |                
      |                      |               
      x--------> y            -------->    
         
  3 x mafGizmoScaleAxis   1 x mafGizmoScale 
  (single axis scaling       
   constrain)

 and forward pose matrixes to the listener operation. 
 This object works by creating smaller components that are parented to the
 vme tree root ie works in global coordinates.
 in order to use it in your client create an instance of it and concatenate
 in post multiply modality matrix that this object is sending to the the
 vme you want to move.

 @sa mafGizmoScaleAxis
*/

class mafGizmoScale: public mafGizmoInterface 
{
public:
           mafGizmoScale(mafVME *input, mafObserver* listener = NULL);
  virtual ~mafGizmoScale(); 

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
	void Show(bool showX, bool showY, bool showZ, bool showIso);

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

  //----------------------------------------------------------------------------
  // Gizmo Gui
  //----------------------------------------------------------------------------
  mmgGui *GetGui() {return (m_GuiGizmoScale->GetGui());};

protected:

  mafGuiGizmoScale *m_GuiGizmoScale;

  /** Gizmo components events handling */
  void OnEventGizmoGui(mafEventBase *maf_event);

  /** Gizmo components events handling */
  void OnEventGizmoComponents(mafEventBase *maf_event);

  /** 
  gizmo components enum*/
  enum AXIS {X = 0, Y, Z};
  enum ACTIVE_COMPONENT {NONE = -1, X_AXIS = 0, Y_AXIS, Z_AXIS, ISOTROPIC};
 
  /**
  Highlight one component and dehighlight other components*/
  void Highlight(int component); 
  
  /** Array holding the three gizmo that performs scaling on a vector*/  
  mafGizmoScaleAxis *m_GSAxis[3];

  mafGizmoScaleIsotropic *m_GSIsotropic;

  /** 
  Register the active gizmo component at MOUSE_DOWN*/
  int m_ActiveGizmoComponent;

  /** Send matrix to postmultiply to listener */
  void SendTransformMatrixFromGui(mafEventBase *maf_event);

private:
  
  /**
  Scaling gizmo initial pose; used to restore scale gizmo axis pose after a gizmo drag event*/
  mafMatrix *m_InitialGizmoPose;
  
  /**
  Vme matrix relative to RefSysVME at MOUSE_DOWN*/
  mafMatrix *m_VmeMatrixRelativeToRefSysVME;

  /**
  RefSys abs matrix at MOUSE_DOWN*/
  mafMatrix *m_RefSysVMEAbsMatrixAtMouseDown;

  /** 
  Return the scaling value to be applied to vme on current axis based on active gizmo position */
  double GetScalingValue() const;

  
  /** 
  Build vector with origin in p1 pointing to p2 */
  void BuildVector(double *p1, double *p2, double *vec) const
  {
    if (vec)
    {
        vec[0] = p2[0] - p1[0];
        vec[1] = p2[1] - p1[1];
        vec[2] = p2[2] - p1[2];
    }
  }
};
#endif

/*
  //----------------------------------------------------------------------------
  // Build the scale matrix from gizmo translation during mouse_move
  //----------------------------------------------------------------------------

  
  ^           ________          
  |          |        |
  |----------|        |
  |          |        |------>X
  |----------|        |
  |          |________|
  O          
  
  |-------------------|
        gl0

  
  ^                      ________          
  |                     |        |
  |           ----------|        |
  |                     |        |------>X
  |           ----------|        |
  |                     |________|
  O          
  
  |------------------------------|
        gl1


  S = gl1 / gl0

  S: scale to be postmultiplyed at mouse up
  

  //----------------------------------------------------------------------------
  // Build matrix to be sent to listener operation given the scale matrix S
  // to be applied relative to RS refsys to the vme
  //----------------------------------------------------------------------------


                      ^   
               ^      |
               |      |
               |       --->
  ^             --->  VME 
  |            RS
  |
   --->
  W

  1) Express VME matrix in RS refsys via mafTransform
      mflTr->SetInput(VME->GetAbsMatrix)
      mflTr->SetSourceRefSys(W)
      mflTr->SetTargetRefSys(RS)

      VME_RS = mflTr->GetTransform();

  2) Postmultiply scale matrix to VME_RS matrix
      vtkTr *tr;
      tr->PostMultiply()
      tr->SetMatrix(VME_RS)
      tr->Concatenate(S)
        
  3) Express S * VME_RS in W refsys: this is new vme abs pose
      
      mflTr->SetInput(S_x_VME_RS)
      mflTr->SetSourceRefSys(RS)
      mflTr->SetTargetRS(W)

      NEW_VME_ABS_pose = mflTr->GetTransform();
  
  - At this point i could set the abs pose to the vme 
  
  4) Build matrix to be sent to listening op if a matrix to be postmultiplied to the vme abs pose has to be created


*/