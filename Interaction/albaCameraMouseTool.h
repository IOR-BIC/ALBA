/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCameraMouseTool
 Authors: Gianluigi Crimi, from albaInteractorCameraMove by Paolo Quadrani & Marco Petrone

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaCameraMouseTool_h
#define __albaCameraMouseTool_h

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
class ALBA_EXPORT albaCameraMouseTool : public albaInteractionTool
{
public:
  albaTypeMacro(albaCameraMouseTool, albaInteractionTool);

  enum CAMERA_MOTIONS {MOTION_NONE = 0, MOTION_ROTATE, MOTION_PAN, MOTION_DOLLY, MOTION_SPIN};

  virtual bool OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2]);
  virtual bool OnLeftButtonUp(albaView *view, vtkRenderer *ren, double pos[2]) {return Stop();}
  virtual bool OnMiddleButtonDown(albaView *view, vtkRenderer *ren, double pos[2]) {return Start(view, ren, pos, MOTION_PAN);}
  virtual bool OnMiddleButtonUp(albaView *view, vtkRenderer *ren, double pos[2]) {return Stop();}
  virtual bool OnRightButtonDown(albaView *view, vtkRenderer *ren, double pos[2]) {return Start(view, ren, pos, MOTION_DOLLY);}
  virtual bool OnRightButtonUp(albaView *view, vtkRenderer *ren, double pos[2]) {return Stop();}
  virtual bool OnDrag(albaView *view, vtkRenderer *ren, double pos[2]);
  virtual bool OnWheel(albaView *view, vtkRenderer *ren, double pos[2], double rotation);

  /** Return true if the camera is moving because of a mouse button */
  bool IsInteracting() {return m_Interacting;}

  /** Return the current camera motion */
  int GetMotion() {return m_Motion;}

  /** Enable/disable the reset of the clipping range after each camera change (default enabled) */
  void SetAutoResetClippingRange(bool enable) {m_AutoResetClippingRange = enable;}

protected:
  albaCameraMouseTool();
  virtual ~albaCameraMouseTool() {}

  /** Start the motion if the camera is not already moving */
  bool Start(albaView *view, vtkRenderer *ren, double pos[2], int motion);

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
  albaCameraMouseTool(const albaCameraMouseTool&);  // Not implemented.
  void operator=(const albaCameraMouseTool&);  // Not implemented.
};

#endif
