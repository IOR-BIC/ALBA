/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolCamera
 Authors: Gianluigi Crimi, from albaInteractorCameraMove by Paolo Quadrani & Marco Petrone

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaToolCamera_h
#define __albaToolCamera_h

#include "albaInteractionTool.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkCamera;

/** Tool moving the camera with the mouse.
  - left button: rotate; with SHIFT pan, with SHIFT+CTRL dolly, with CTRL spin
  - middle button: pan
  - right button: dolly
  - wheel: zoom
  The motion started by a button lasts until a button is released. */
class ALBA_EXPORT albaToolCamera : public albaInteractionTool
{
public:
  albaTypeMacro(albaToolCamera, albaInteractionTool);

  enum CAMERA_MOTIONS {MOTION_NONE = 0, MOTION_ROTATE, MOTION_PAN, MOTION_DOLLY, MOTION_SPIN};

  virtual bool OnLeftButtonDownImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);
  virtual bool OnLeftButtonUpImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) {return Stop();}
  virtual bool OnMiddleButtonDownImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) {return Start(rwi, ren, pos, MOTION_PAN);}
  virtual bool OnMiddleButtonUpImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) {return Stop();}
  virtual bool OnRightButtonDownImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) {return Start(rwi, ren, pos, MOTION_DOLLY);}
  virtual bool OnRightButtonUpImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]) {return Stop();}
  virtual bool OnDragImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2]);
  virtual bool OnWheelImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2], double rotation);

  /** Return true if the camera is moving because of a mouse button */
  bool IsInteracting() {return m_Interacting;}

  /** Return the current camera motion */
  int GetMotion() {return m_Motion;}

  /** Enable/disable the reset of the clipping range after each camera change (default enabled) */
  void SetAutoResetClippingRange(bool enable) {m_AutoResetClippingRange = enable;}

protected:
  albaToolCamera();
  virtual ~albaToolCamera() {}

  /** Start the motion if the camera is not already moving */
  bool Start(albaRWIBase *rwi, vtkRenderer *ren, double pos[2], int motion);

  /** Stop the motion, if the camera is moving */
  bool Stop();

  void Rotate();
  void Spin();
  void Pan();
  void Dolly();

  /** Reset the clipping range, if enabled, and render */
  void CameraChanged();

  /** Reset the clipping range of the front renderer considering the other layers */
  void ResetClippingRange();

  bool m_Interacting; ///< true while the camera is moving because of a mouse button
  vtkCamera *m_CurrentCamera; ///< the camera being moved
  int m_Motion;
  int m_MousePose[2];
  int m_LastMousePose[2];
  double m_MotionFactor;
  bool m_AutoResetClippingRange;

private:
  albaToolCamera(const albaToolCamera&);  // Not implemented.
  void operator=(const albaToolCamera&);  // Not implemented.
};

#endif
