/*=========================================================================

 Program: MAF2
 Module: mafInteractorCameraMove
 Authors: Paolo Quadrani & Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafInteractorCameraMove_h
#define __mafInteractorCameraMove_h

enum MOUSE_CAMERA
{
	MOUSE_CAMERA_NONE,
	MOUSE_CAMERA_ROTATE,
	MOUSE_CAMERA_PAN,
	MOUSE_CAMERA_SPIN,
	MOUSE_CAMERA_DOLLY,
	MOUSE_CAMERA_ZOOM,
};



#include "mafInteractor.h"
#include "mafMTime.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafDeviceButtonsPadMouse;
class vtkCamera;

/** implements mouse move of camera in the scene.
  This class implements a mouse move of the renderer camera in the scene. The interaction
  modality is a mouse manipulation, where movements of the mouse are mapped
  into movements of the camera. More details to be written...*/
class MAF_EXPORT mafInteractorCameraMove : public mafInteractor
{
public:
  mafTypeMacro(mafInteractorCameraMove,mafInteractor);

  /** Start the interaction with the selected object */
  virtual int StartInteraction(mafDeviceButtonsPadMouse *mouse);

  /**  Process events coming from tracker */
  virtual void OnEvent(mafEventBase *event);

  virtual void OnMouseMove();
  virtual void OnLeftButtonDown(mafEventInteraction *e);
  virtual void OnLeftButtonUp();
  virtual void OnMiddleButtonDown(mafEventInteraction *e);
  virtual void OnMiddleButtonUp();
  virtual void OnRightButtonDown(mafEventInteraction *e);
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
  mafInteractorCameraMove();
  virtual ~mafInteractorCameraMove();

  virtual void OnButtonDown(mafEventInteraction *e);
  
  virtual void OnButtonUp(mafEventInteraction *e);

	virtual void OnMouseWheel(mafEventInteraction *e);
	  
  float         m_MotionFactor;
  int           m_State;  

  int           m_MousePose[2];
  int           m_LastMousePose[2];
  int           m_ButtonPressed;
  bool          m_ActiveCameraToCurrentCameraFlag;
  vtkCamera*    m_CurrentCamera;  ///< Stores camera to which the interaction is currently assigned
  mafMTime      m_UpdateTime;     ///< Time stamp of the last update of the tracker to world transformation 
  bool			m_AutoResetClippingRange; // Turn on (default)-off the auto reset of clipping range

private:
  mafInteractorCameraMove(const mafInteractorCameraMove&);  // Not implemented.
  void operator=(const mafInteractorCameraMove&);  // Not implemented.
};
#endif 
