/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoAutoscaleHelper
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGizmoAutoscaleHelper_h
#define __albaGizmoAutoscaleHelper_h

#include "albaTransformBase.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkRenderer;
class vtkTransform;
class vtkCamera;
class albaOBB;
class vtkCallbackCommand;
class vtkObject;
class vtkActor;
class albaVME;

/** 

Helper object for gizmo autoscaling ie actor2D-like behavior

*/

class ALBA_EXPORT albaGizmoAutoscaleHelper:public albaTransformBase
{
public:
  albaGizmoAutoscaleHelper();
  virtual ~albaGizmoAutoscaleHelper();

  albaTypeMacro(albaGizmoAutoscaleHelper,albaTransformBase);

  /** Set the renderer used as input for this transform */
  void SetRenderer(vtkRenderer *ren);

  /** Return the renderer used as input for this transform */
  vtkRenderer *GetRenderer() {return m_Renderer;}

  /** Enable/Disable following of the camera scaling (on by default)*/
  void SetFollowScale(bool flag) {m_FollowScale=flag;Modified();}
  bool GetFollowScale() {return m_FollowScale;}
  void FollowScaleOn() {SetFollowScale(true);}
  void FollowScaleOff() {SetFollowScale(false);}

  /** Set the size of the gizmo when autoscale is enabled. The size is expressed as gizmoHeight / renderWindowHeight.
  Max size is 1 (gizmo height equal to the render window one). This setting has no effect when autoscale is off*/
  void SetRenderWindowHeightPercentage(double percentage) {m_RenderWindowHeightPercentage = percentage;};
  double GetRenderWindowHeightPercentage() {return m_RenderWindowHeightPercentage;};

  /** Set the prop to be autoscaled (to be used from the pipe) */
  void SetActor(vtkActor *actor) {m_Actor = actor;};
  vtkActor *GetActor() {return m_Actor;};

  /** Set the vme to be autoscaled (to be used from the pipe) */
  void SetVME(albaVME *vme);
  albaVME *GetVME() {return m_VME;};

  virtual int DeepCopy(albaGizmoAutoscaleHelper *trans);
  
  /** 
    Get the MTime: this is the bit of magic that makes everything work.
    This MTime takes in consideration also the camera's MTime */
  virtual vtkMTimeType GetMTime();

protected:

  /** 
    Set the bounds of the object that should follow the camera.
    This is necessary to allow the the right scaling. */
  void SetBounds(albaOBB *bounds) {m_Bounds=bounds;}
  albaOBB *GetBounds() {return m_Bounds;}

  /** Change the given matrix to follow camera movements */
  void UpdatePoseMatrix(albaMatrix *matrix,albaMatrix *old_view_matrix, albaMatrix *new_view_matrix);

  void RecomputeAll();

  /** Change matrix scale to create a transform making two boxes 
      to fit.Translation and Orientation are left unchanged.
      Fitting can occur according to different modalities.
      @sa ComputeScaling() */ 
  static void AutoFitting(albaMatrix *matrix,albaOBB *tracked_bounds,vtkRenderer *ren);  
  void AutoFitting() {AutoFitting(m_Matrix,m_Bounds,m_Renderer);}

  /** Compute scale between two boxes in order to fit viewport Y */
  static void ComputeScaling(albaOBB *inBox, albaOBB *outBox,double *scale);

  /** gizmoHeight / renderWindowHeight (1 at maximum) */
  double m_RenderWindowHeightPercentage;

  /**  Updates the internal matrix */
  virtual void InternalUpdate();

  static void InternalProcessEvents(vtkObject* sender, unsigned long channel, void* clientdata, void* calldata);

  vtkRenderer*  m_Renderer; ///< This stores the renderer used as input
  vtkCamera*    m_Camera; ///< this is to find if the render's active camera is changed
  bool           m_FollowScale; ///< Flag for enabling the scaling according to camera frustum
  
  albaOBB*       m_Bounds; ///< m_Bounds of the object that should follow the camera

  double m_VMEBounds[6]; ///< Input VME bounds  

  float         m_OldViewAngle;
  double        m_OldDistance;
  albaMatrix*    m_OldViewMatrix;

  vtkCallbackCommand* m_EventRouter;

  vtkActor *m_Actor;
  albaVME   *m_VME;

private:
  albaGizmoAutoscaleHelper(const albaGizmoAutoscaleHelper&); // Not implemented
  void operator=(const albaGizmoAutoscaleHelper&); // Not implemented
};

#endif /* __albaGizmoAutoscaleHelper_h */
