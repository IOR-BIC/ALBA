/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolCameraTracker
 Authors: Gianluigi Crimi, from albaInteractor6DOFCameraMove by Marco Petrone

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaToolCameraTracker_h
#define __albaToolCameraTracker_h

#include "albaInteractionTool.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaDevice;
class albaDeviceButtonsPadTracker;
class albaAvatar3D;
class albaMatrix;
class albaTransform;
class vtkCamera;

/** Tool moving the camera with a tracker: while the left button is pressed
  the movements of the tracker are mapped into movements of the camera.
  It works only with trackers having an albaAvatar3D. */
class ALBA_EXPORT albaToolCameraTracker : public albaInteractionTool
{
public:
  albaTypeMacro(albaToolCameraTracker, albaInteractionTool);

  /** The left button starts the motion */
  virtual bool OnTrackerButtonDown(albaDeviceButtonsPadTracker *tracker, int button, albaMatrix *pose);
  virtual bool OnTrackerButtonUp(albaDeviceButtonsPadTracker *tracker, int button);
  virtual bool OnTrackerDrag(albaDeviceButtonsPadTracker *tracker, albaMatrix *pose);

  /** Return true if the camera is moving because of the device */
  bool IsInteracting(albaDevice *device);

protected:
  albaToolCameraTracker();
  virtual ~albaToolCameraTracker();

  /** Store the tracker pose at the start of the motion or after the last update */
  void TrackerSnapshot(albaMatrix *pose);

  /** Compute the transform from the last snapshot to the current tracker pose */
  void UpdateDeltaTransform();

  /** Move the camera according to the last tracker pose */
  void UpdateCamera();

  albaDeviceButtonsPadTracker *m_Tracker; ///< the tracker moving the camera, NULL if not interacting
  albaAvatar3D *m_Avatar;          ///< the avatar of the tracker
  vtkCamera    *m_CurrentCamera;   ///< the camera being moved

  albaMatrix    *m_TrackerPoseMatrix;
  albaMatrix    *m_StartTrackerPoseMatrix;
  albaMatrix    *m_InverseTrackerPoseMatrix;
  albaTransform *m_DeltaTransform;
  albaTransform *m_TmpTransform;

  double m_StartCameraPosition[4]; ///< position of the camera at the last update
  double m_StartFocalPoint[4];     ///< focal point of the camera at the last update
  double m_StartViewUp[4];         ///< view up of the camera at the last update

private:
  albaToolCameraTracker(const albaToolCameraTracker&);  // Not implemented.
  void operator=(const albaToolCameraTracker&);  // Not implemented.
};

#endif
