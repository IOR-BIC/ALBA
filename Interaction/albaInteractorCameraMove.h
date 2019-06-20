/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorCameraMove
 Authors: Paolo Quadrani & Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaInteractorCameraMove_h
#define __albaInteractorCameraMove_h

enum MOUSE_CAMERA
{
	MOUSE_CAMERA_NONE,
	MOUSE_CAMERA_ROTATE,
	MOUSE_CAMERA_PAN,
	MOUSE_CAMERA_SPIN,
	MOUSE_CAMERA_DOLLY,
	MOUSE_CAMERA_ZOOM,
};



#include "albaInteractor.h"
#include "albaMTime.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaDeviceButtonsPadMouse;
class vtkCamera;

/** implements mouse move of camera in the scene.
  This class implements a mouse move of the renderer camera in the scene. The interaction
  modality is a mouse manipulation, where movements of the mouse are mapped
  into movements of the camera. More details to be written...*/
class ALBA_EXPORT albaInteractorCameraMove : public albaInteractor
{
public:
  albaTypeMacro(albaInteractorCameraMove,albaInteractor);

  /** Start the interaction with the selected object */
  virtual int StartInteraction(albaDeviceButtonsPadMouse *mouse);

  /**  Process events coming from tracker */
  virtual void OnEvent(albaEventBase *event);

  virtual void OnMouseMove();
  virtual void OnLeftButtonDown(albaEventInteraction *e);
  virtual void OnLeftButtonUp();
  virtual void OnMiddleButtonDown(albaEventInteraction *e);
  virtual void OnMiddleButtonUp();
  virtual void OnRightButtonDown(albaEventInteraction *e);
  virtual void OnRightButtonUp();

  virtual void Rotate();
  virtual void Spin();
  virtual void Pan();
  virtual void Dolly();

  virtual void StartRotate();
  virtual void EndRotate();
  virtual void StartZoom();
  virtual void EndZoom();
  virtual void StartPan();
  virtual void EndPan();
  virtual void StartSpin();
  virtual void EndSpin();
  virtual void StartDolly();
  virtual void EndDolly();

  virtual void StartState(int newstate);
  virtual void StopState();

  void ResetClippingRange(); // to work with layers

  virtual void AutoResetClippingRangeOn();
	virtual void AutoResetClippingRangeOff();

protected:
  albaInteractorCameraMove();
  virtual ~albaInteractorCameraMove();

  virtual void OnButtonDown(albaEventInteraction *e);
  
  virtual void OnButtonUp(albaEventInteraction *e);

	virtual void OnMouseWheel(albaEventInteraction *e);
	  
  float         m_MotionFactor;
  int           m_State;  

  int           m_MousePose[2];
  int           m_LastMousePose[2];
  int           m_ButtonPressed;
  bool          m_ActiveCameraToCurrentCameraFlag;
  vtkCamera*    m_CurrentCamera;  ///< Stores camera to which the interaction is currently assigned
  albaMTime      m_UpdateTime;     ///< Time stamp of the last update of the tracker to world transformation 
  bool			m_AutoResetClippingRange; // Turn on (default)-off the auto reset of clipping range

private:
  albaInteractorCameraMove(const albaInteractorCameraMove&);  // Not implemented.
  void operator=(const albaInteractorCameraMove&);  // Not implemented.
};
#endif 
