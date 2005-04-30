/*========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafCameraTransform.h,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:34:55 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafCameraTransform_h
#define __mafCameraTransform_h

#include "mafTransform.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkRenderer;
class vtkTransform;
class vtkCamera;
class mafOBB;
class vtkCallbackCommand;

/** Implement transform between view coordinates to world coordinates

  mafCameraTransform is a class implementing transform between view coordinate system
  to world coordinate system. The renderer storing the coordinate system must be
  provided as input. This transforms maps points from the view coordinate system
  to a specified coordinate system. By default (TargetFrame=I) to the World coordinate
  system. In general the view coordinate system has the following spawning: 
  X [-1,1] , where -1 is left frustum plane
  Y [-1,1] , where -1 is the bottom frustum plane
  Z [-1,1] , for Z there can be different mapping strategies:
  
    -# Z = 0 corresponds to the Camera position. Z = 1, depends on Y scale
    -# Z = 0  corresponds to the Focal point, Z = -1 and Z = 1 depends on Y Scale
    -# Z = -1 corresponds to the Near plane, Z = 1 to the Far plane

  You can enable single features:
  - camera orientation
  - camera position
  - camera scaling

  With orientation enabled the transform will always try to take the input matrix and 
  rotate it such that it points to the camera (supposed 0,0,0 means orientated
  toward the camera). The original orientation is cumulated.
  With position enabled the transform will try to translate the matrix to be in the
  same position when the camera moves. 0,0,0 means in the focus point.


  @sa mafTransform 
  */
class mafCameraTransform:public mafTransform
{
public:
  mafCameraTransform();
  virtual ~mafCameraTransform();

  mafTypeMacro(mafCameraTransform,mafTransform);
  
  enum AutoPositionModalities
  {
    ATTACH_TO_FOCAL_POINT, ///< Follow the focal point
    ATTACH_TO_CAMERA, ///< Follow the camera position
    ATTACH_TO_CLIPPING_PLANE ///< Follow the neat clipping plane
  };

  enum AutoFittingModalities
  {
    MAX_SCALE, ///< Select the maximum scale among X/Y (i.e. fit the minimum size)
    MIN_SCALE, ///< Select the minimum scale X/Y (i.e. fit the maximum size)
    FIT_X, ///< Always fit the X axis
    FIT_Y, ///< Always fit the Y axis
    ANISOTROPIC, ///< set XYZ scales to fit exactly
    NUM_OF_MAPPINGS
  };

  enum AutoScalingModalities
  {
    AUTO_FITTING, ///< scale the bounds to fit the camera frustum
    PRESERVE_SIZE ///< scale the bounds to appear at constant size (use with ATTACH_TO_CAMERA)
  };

  /** Set the renderer used as input for this transform */
  void SetRenderer(vtkRenderer *ren);

  /** Return the renderer used as input for this transform */
  vtkRenderer *GetRenderer() {return m_Renderer;}

  /** 
    Set the bounds of the object that should follow the camera.
    This is necessary to allow the the right scaling. */
  void SetBounds(mafOBB *bounds) {m_Bounds=bounds;}
  mafOBB *GetBounds() {return m_Bounds;}

  /** Enable/Disable following of the camera position */
  //vtkBooleanMacro(FollowPosition,int);
  void SetFollowPosition(int flag) {m_FollowPosition=flag;Modified();}
  int GetFollowPosition() {return m_FollowPosition;}

  /** Enable/Disable following of the camera orientation */
  //vtkBooleanMacro(FollowOrientation,int) ;
  void SetFollowOrientation(int flag) {m_FollowOrientation=flag;Modified();}
  int GetFollowOrientation () {return m_FollowOrientation;}

  /** Enable/Disable following of the camera scaling */
  //vtkBooleanMacro(FollowScale,int);
  void SetFollowScale(int flag) {m_FollowScale=flag;Modified();}
  vtkGetMacro(FollowScale,int) {return m_FollowScale;}

  /** Set the modality to fit the Z coordinate. See class comment. */
  void SetPositionMode(int type) {m_PositionMode=type;Modified();}
  int GetPositionMode() {return m_PositionMode;}
  void SetPositionModeToAttachToFocalPoint() {SetPositionMode(ATTACH_TO_FOCAL_POINT);}
  void SetPositionModeToAttachToCamera() {SetPositionMode(ATTACH_TO_CAMERA);}
  void SetPositionModeToAttachToClippingPlane() {SetPositionMode(ATTACH_TO_CLIPPING_PLANE);}

  /** Set which kind of scaling should be applied to the transform */
  void SetScalingMode(int mode) {m_ScalingMode=mode;}
  int GetScalingMode() {return m_ScalingMode;}

  /** 
    Set scaling mode to AUTO_FITTING: the transform try to fit the
    given m_Bounds (see SetBounds() ) to the view frustum. Fitting can 
    occur in different ways (see SetFittingMode() ). Notice the m_Bounds
    can be any size, the transform will provide the right scaling. */
  void SetScalingModeToAutoFitting();

  /** (not yet implemented)
    Set scaling mode to PRESERVE_SIZE. This should scale the 3D object
    to keep display size fixed with respect to the view port. In this
    case the m_Bounds (see SetBounds() ) should be expressed in normalized
    coordinates, where X and Y represent view port coords, and Z... yet
    to be defined :-) .*/
  void SetScalingModeToPreserveSize();

  /** set which kind of AutoFitting strategy should be used */
  void SetFittingMode(int mode) {m_FittingMode=mode;Modified();}
  void GetFittingMode() {return m_FittingMode;}
  
  /** 
    Get the MTime: this is the bit of magic that makes everything work.
    This MTime takes in consideration also the camera's MTime */
  virtual unsigned long GetMTime();

//  virtual int DeepCopy(mafCameraTransform *trans);
  
  /** Change the given matrix to follow camera movements */
  void UpdatePoseMatrix(vtkMatrix4x4 *matrix,vtkMatrix4x4 *old_view_matrix, vtkMatrix4x4 *new_view_matrix);

  /** change matrix orientation to create a transform making points to be
      oriented according to camera orientation. Translation is left unchanged
      while scale is reset to 1. The input matrix posiiton is used to compute
      the direction, in case call AutoPosition() before of this. */
  static void AutoOrientation(vtkMatrix4x4 *matrix,vtkRenderer *ren);
  void AutoOrientation() {AutoOrientation(m_Matrix,m_Renderer);}

  /** change matrix translation to create a transform making points to be
      centered into the camera's focal point. Orientation and scale are 
      left unchanged */
  static void AutoPosition(vtkMatrix4x4 *matrix,vtkRenderer *ren, int mode=ATTACH_TO_FOCAL_POINT);
  void AutoPosition() {AutoPosition(m_Matrix,m_Renderer,m_PositionMode);}

  /** Change matrix scale to create a transform making two boxes 
      to fit.Translation and Orientation are left unchanged.
      Fitting can occur according to different modalities.
      @sa ComputeScaling() */ 
  static void AutoFitting(vtkMatrix4x4 *matrix,mafOBB *tracked_bounds,vtkRenderer *ren,int mode=MIN_SCALE);  
  void AutoFitting() {AutoFitting(m_Matrix,m_Bounds,m_Renderer,m_FittingMode);}

  /** Change matrix scale to create a transform making two boxes 
      to fit. Translation and Orientation are left unchanged. This specific algrothim
      is thought to be used with AutoPosition() and ATTACH_TO_FOCAL_POINT mode.
      Fitting can occur according to different modalities.
      @sa ComputeScaling() */
  static void AutoFitting2(vtkMatrix4x4 *matrix,mafOBB *tracked_bounds,vtkRenderer *ren,int mode=ANISOTROPIC);
  void AutoFitting2() {AutoFitting2(m_Matrix,m_Bounds,m_Renderer,m_FittingMode);}

  /** Compute scale between two boxes according to given modality:
    - 0 select isotropic min scale
    - 1 select isotropic max scale
    - 2 select anisotropic scale (do not keep aspect ratio)
    - 3 fit Y
    - 4 fit X */
  static void ComputeScaling(mafOBB *inBox, mafOBB *outBox,double *scale,int mode=MIN_SCALE);

  void RecomputeAll();

protected:

  /**  Updates the internal matrix */
  virtual void InternalUpdate();

  static void InternalProcessEvents(vtkObject* sender, unsigned long channel, void* clientdata, void* calldata);

  vtkRenderer*  m_Renderer; ///< This stores the renderer used as input
  vtkCamera*    m_Camera; ///< this is to find if the render's active camera is changed
  int           m_FollowPosition; ///< Flag for enabling the translation according to camera position
  int           m_FollowOrientation; ///< Flag for enabling the re-orientation according to camera position
  int           m_FollowScale; ///< Flag for enabling the scaling according to camera frustum
  int           m_PositionMode; ///< Modality defining the rule to set the position with respect to the camera
  int           m_ScalingMode; ///< Kind of scaling policy (@sa AutoScalingModalities)
  int           m_FittingMode; ///< Kind of scaling used to fit the view frustum

  mafOBB*       m_Bounds; ///< m_Bounds of the object that should follow the camera

  vtkTransform* m_CameraFrame;
  float         m_OldViewAngle;
  double        m_OldDistance;
  vtkMatrix4x4* m_OldViewMatrix;

  vtkCallbackCommand* m_EventRouter;

private:
  mafCameraTransform(const mafCameraTransform&); // Not implemented
  void operator=(const mafCameraTransform&); // Not implemented
};

#endif /* __mafCameraTransform_h */
 
