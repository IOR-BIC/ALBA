/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorExtractIsosurface
 Authors: Paolo Quadrani & Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaInteractorExtractIsosurface_h
#define __albaInteractorExtractIsosurface_h

#include "albaInteractorCameraMove.h"
#include "albaMTime.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkCellPicker;
class vtkCamera;

/** implements mouse move of camera in the scene.
  This class implements a mouse move of the renderer camera in the scene. The interaction
  modality is a mouse manipulation, where movements of the mouse are mapped
  into movements of the camera. More details to be written...*/
class ALBA_EXPORT albaInteractorExtractIsosurface : public albaInteractorCameraMove
{
public:
  albaTypeMacro(albaInteractorExtractIsosurface,albaInteractorCameraMove);

  /** Start the interaction with the selected object */
  virtual int StartInteraction(albaDeviceButtonsPadMouse *mouse);

  /** redefined to set the picking iso-value flag */
  virtual void OnMouseMove();

  /** redefined to set the picking iso-value flag */
  virtual void OnLeftButtonDown(albaEventInteraction *e);

  /** redefined to set the picking iso-value flag and if it is true, call PickIsoValue() */
  virtual void OnButtonUp(albaEventInteraction *e);

protected:
  albaInteractorExtractIsosurface();
  virtual ~albaInteractorExtractIsosurface();
  
  /** send the picked point to the listener */
  void PickIsoValue(albaDevice *device);

  bool m_PickValue;

private:
  albaInteractorExtractIsosurface(const albaInteractorExtractIsosurface&);  // Not implemented.
  void operator=(const albaInteractorExtractIsosurface&);  // Not implemented.

  /** test friend */
  friend class albaInteractorExtractIsosurfaceTest;
};
#endif 
