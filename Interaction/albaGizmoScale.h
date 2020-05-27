/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoScale
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGizmoScale_H__
#define __albaGizmoScale_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"
#include "albaGizmoInterface.h"
#include "albaGUIGizmoScale.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGizmoScaleAxis;
class albaGizmoScaleIsotropic;
class albaMatrix;
class albaVME;
class albaGUI;

/** Gizmo used to perform constrained scaling along the x, yz axis
 or the xy, xz, yz plane.

 This class create a scaling gizmo composed of:
  
 3 albaGizmoScaleAxis

 
      z                                 
      ^                      ^              
      |                      | 
      |                      |               
      |                  =   |                
      |                      |               
      x--------> y            -------->    
         
  3 x albaGizmoScaleAxis   1 x albaGizmoScale 
  (single axis scaling       
   constrain)

 and forward pose matrixes to the listener operation. 
 This object works by creating smaller components that are parented to the
 vme tree root ie works in global coordinates.
 in order to use it in your client create an instance of it and concatenate
 in post multiply modality matrix that this object is sending to the the
 vme you want to move.

 @sa albaGizmoScaleAxis
*/

class ALBA_EXPORT albaGizmoScale: public albaGizmoInterface 
{
public:
  albaGizmoScale(albaVME *input, albaObserver* listener = NULL , bool buildGUI = true);
  virtual ~albaGizmoScale(); 

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
	void Show(bool showX, bool showY, bool showZ, bool showIso);

  /**
  Set the gizmo pose*/
  void SetAbsPose(albaMatrix *absPose, albaTimeStamp ts = -1);

  albaMatrix *GetAbsPose();
  
  //----------------------------------------------------------------------------
  // RefSys
  //----------------------------------------------------------------------------

  /** Set the vme to be used as reference system, the vme is referenced; default ref sys is vme abs matrix */
  void SetRefSys(albaVME *refSys);
  albaVME* GetRefSys();

  //----------------------------------------------------------------------------
  // Gizmo Gui
  //----------------------------------------------------------------------------
  albaGUI *GetGui() {return (m_GuiGizmoScale->GetGui());};

	bool GetApplyScaleToVME() const { return m_ApplyScaleToVME; }
	void SetApplyScaleToVME(bool val) { m_ApplyScaleToVME = val; }

protected:

  albaGUIGizmoScale *m_GuiGizmoScale;

  /** Gizmo components events handling */
  void OnEventGizmoGui(albaEventBase *alba_event);

  /** Gizmo components events handling */
  void OnEventGizmoComponents(albaEventBase *alba_event);

  /** 
  gizmo components enum*/
  enum AXIS {X = 0, Y, Z};
  enum ACTIVE_COMPONENT {NONE = -1, X_AXIS = 0, Y_AXIS, Z_AXIS, ISOTROPIC};
 
  /**
  Highlight one component and dehighlight other components*/
  void Highlight(int component); 
  
  /** Array holding the three gizmo that performs scaling on a vector*/  
  albaGizmoScaleAxis *m_GSAxis[3];

  albaGizmoScaleIsotropic *m_GSIsotropic;

  /** 
  Register the active gizmo component at MOUSE_DOWN*/
  int m_ActiveGizmoComponent;

  /** Send matrix to postmultiply to listener */
  void SendTransformMatrixFromGui(albaEventBase *alba_event);

private:
  
  /**
  Register if the gizmo gui has to be built at startup*/
  bool m_BuildGUI;

	bool m_ApplyScaleToVME;

	double m_CurrentDist;
	double m_ScaleDiff;

  /**
  Scaling gizmo initial pose; used to restore scale gizmo axis pose after a gizmo drag event*/
  albaMatrix *m_InitialGizmoPose;
  
  /**
  Vme matrix relative to RefSysVME at MOUSE_DOWN*/
  albaMatrix *m_VmeMatrixRelativeToRefSysVME;

  /**
  RefSys abs matrix at MOUSE_DOWN*/
  albaMatrix *m_RefSysVMEAbsMatrixAtMouseDown;

  /** 
  Return the scaling value to be applied to vme on current axis based on active gizmo position */
  double GetScalingValue();

  
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
 
  /**
  test friend*/
  friend class albaGizmoScaleTest;
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

  1) Express VME matrix in RS refsys via albaTransform
      mflTr->SetInput(VME->GetAbsMatrix)
      mflTr->SetSourceRefSys(W)
      mflTr->SetTargetRefSys(RS)

      VME_RS = mflTr->GetTransform();

  2) Postmultiply scale matrix to VME_RS matrix
      transform->PostMultiply()
      transform->SetMatrix(VME_RS)
      transform->Concatenate(S)
        
  3) Express S * VME_RS in W refsys: this is new vme abs pose
      
      mflTr->SetInput(S_x_VME_RS)
      mflTr->SetSourceRefSys(RS)
      mflTr->SetTargetRS(W)

      NEW_VME_ABS_pose = mflTr->GetTransform();
  
  - At this point i could set the abs pose to the vme 
  
  4) Build matrix to be sent to listening op if a matrix to be postmultiplied to the vme abs pose has to be created


*/
