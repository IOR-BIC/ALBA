/*========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoAutoscaleHelper.h,v $
  Language:  C++
  Date:      $Date: 2010-11-10 18:52:35 $
  Version:   $Revision: 1.1.2.2 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGizmoAutoscaleHelper_h
#define __mafGizmoAutoscaleHelper_h

#include "mafTransformBase.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkRenderer;
class vtkTransform;
class vtkCamera;
class mafOBB;
class vtkCallbackCommand;
class vtkObject;
class vtkActor;
class mafVME;

/** 

Helper object for gizmo autoscaling ie actor2D-like behavior

*/

class mafGizmoAutoscaleHelper:public mafTransformBase
{
public:
  mafGizmoAutoscaleHelper();
  virtual ~mafGizmoAutoscaleHelper();

  mafTypeMacro(mafGizmoAutoscaleHelper,mafTransformBase);

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
  void SetVME(mafVME *vme) {m_VME = vme;};
  mafVME *GetVME() {return m_VME;};

  virtual int DeepCopy(mafGizmoAutoscaleHelper *trans);
  
  /** 
    Get the MTime: this is the bit of magic that makes everything work.
    This MTime takes in consideration also the camera's MTime */
  virtual unsigned long GetMTime();

protected:

  /** 
    Set the bounds of the object that should follow the camera.
    This is necessary to allow the the right scaling. */
  void SetBounds(mafOBB *bounds) {m_Bounds=bounds;}
  mafOBB *GetBounds() {return m_Bounds;}

  /** Change the given matrix to follow camera movements */
  void UpdatePoseMatrix(mafMatrix *matrix,mafMatrix *old_view_matrix, mafMatrix *new_view_matrix);

  void RecomputeAll();

  /** Change matrix scale to create a transform making two boxes 
      to fit.Translation and Orientation are left unchanged.
      Fitting can occur according to different modalities.
      @sa ComputeScaling() */ 
  static void AutoFitting(mafMatrix *matrix,mafOBB *tracked_bounds,vtkRenderer *ren);  
  void AutoFitting() {AutoFitting(m_Matrix,m_Bounds,m_Renderer);}

  /** Compute scale between two boxes in order to fit viewport Y */
  static void ComputeScaling(mafOBB *inBox, mafOBB *outBox,double *scale);

  /** gizmoHeight / renderWindowHeight (1 at maximum) */
  double m_RenderWindowHeightPercentage;

  /**  Updates the internal matrix */
  virtual void InternalUpdate();

  static void InternalProcessEvents(vtkObject* sender, unsigned long channel, void* clientdata, void* calldata);

  vtkRenderer*  m_Renderer; ///< This stores the renderer used as input
  vtkCamera*    m_Camera; ///< this is to find if the render's active camera is changed
  bool           m_FollowScale; ///< Flag for enabling the scaling according to camera frustum
  
  mafOBB*       m_Bounds; ///< m_Bounds of the object that should follow the camera

  float         m_OldViewAngle;
  double        m_OldDistance;
  mafMatrix*    m_OldViewMatrix;

  vtkCallbackCommand* m_EventRouter;

  vtkActor *m_Actor;
  mafVME   *m_VME;

private:
  mafGizmoAutoscaleHelper(const mafGizmoAutoscaleHelper&); // Not implemented
  void operator=(const mafGizmoAutoscaleHelper&); // Not implemented
};

#endif /* __mafGizmoAutoscaleHelper_h */
 
s