/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiCameraMove.h,v $
  Language:  C++
  Date:      $Date: 2005-05-04 16:27:46 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani & Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmiCameraMove_h
#define __mmiCameraMove_h

#define MOUSE_CAMERA_START  0
#define MOUSE_CAMERA_NONE   0

#define MOUSE_CAMERA_ROTATE 1
#define MOUSE_CAMERA_PAN    2
#define MOUSE_CAMERA_SPIN   3
#define MOUSE_CAMERA_DOLLY  4
#define MOUSE_CAMERA_ZOOM   5

#include "mafInteractor.h"
#include "mafMTime.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mmdMouse;
class vtkCamera;

/** implements mouse move of camera in the scene.
  This class implements a mouse move of the renderer camera in the scene. The interaction
  modality is a mouse manipulation, where movements of the mouse are mapped
  into movements of the camera. More details to be written...*/
class mmiCameraMove : public mafInteractor
{
public:
  mafTypeMacro(mmiCameraMove,mafInteractor);

  /** Start the interaction with the selected object */
  virtual int StartInteraction(mmdMouse *mouse);

  /**  Process events coming from tracker */
  virtual void OnEvent(mafEventBase *event);

  virtual void OnMouseMove();
  virtual void OnLeftButtonDown(mafEventInteraction *e);
  virtual void OnLeftButtonUp();
  virtual void OnMiddleButtonDown();
  virtual void OnMiddleButtonUp();
  virtual void OnRightButtonDown();
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

protected:
  mmiCameraMove();
  virtual ~mmiCameraMove();

  virtual void OnButtonDown(mafEventInteraction *e);
  
  virtual void OnButtonUp(mafEventInteraction *e);

  float         m_MotionFactor;
  int           m_State;  

  int           m_MousePose[2];
  int           m_LastMousePose[2];
  int           m_ButtonPressed;
  bool          m_ActiveCameraToCurrentCameraFlag;
  vtkCamera*    m_CurrentCamera;  ///< Stores camera to which the interaction is currently assigned
  mafMTime      m_UpdateTime;     ///< Time stamp of the last update of the tracker to world transformation 

private:
  mmiCameraMove(const mmiCameraMove&);  // Not implemented.
  void operator=(const mmiCameraMove&);  // Not implemented.
};
#endif 
