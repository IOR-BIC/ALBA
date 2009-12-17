/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafInteractorExtractIsosurface.h,v $
  Language:  C++
  Date:      $Date: 2009-12-17 11:46:39 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Paolo Quadrani & Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafInteractorExtractIsosurface_h
#define __mafInteractorExtractIsosurface_h

#include "mafInteractorCameraMove.h"
#include "mafMTime.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkCellPicker;
class vtkCamera;

/** implements mouse move of camera in the scene.
  This class implements a mouse move of the renderer camera in the scene. The interaction
  modality is a mouse manipulation, where movements of the mouse are mapped
  into movements of the camera. More details to be written...*/
class mafInteractorExtractIsosurface : public mafInteractorCameraMove
{
public:
  mafTypeMacro(mafInteractorExtractIsosurface,mafInteractorCameraMove);

  /** Start the interaction with the selected object */
  virtual int StartInteraction(mafDeviceButtonsPadMouse *mouse);

  /** redefined to set the picking iso-value flag */
  virtual void OnMouseMove();

  /** redefined to set the picking iso-value flag */
  virtual void OnLeftButtonDown(mafEventInteraction *e);

  /** redefined to set the picking iso-value flag and if it is true, call PickIsoValue() */
  virtual void OnButtonUp(mafEventInteraction *e);

protected:
  mafInteractorExtractIsosurface();
  virtual ~mafInteractorExtractIsosurface();
  
  /** send the picked point to the listener */
  void PickIsoValue(mafDevice *device);

  bool m_PickValue;

private:
  mafInteractorExtractIsosurface(const mafInteractorExtractIsosurface&);  // Not implemented.
  void operator=(const mafInteractorExtractIsosurface&);  // Not implemented.
};
#endif 
