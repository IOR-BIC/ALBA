/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiExtractIsosurface.h,v $
  Language:  C++
  Date:      $Date: 2005-11-16 15:16:20 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani & Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmiExtractIsosurface_h
#define __mmiExtractIsosurface_h

#include "mmiCameraMove.h"
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
class mmiExtractIsosurface : public mmiCameraMove
{
public:
  mafTypeMacro(mmiExtractIsosurface,mmiCameraMove);

  /** Start the interaction with the selected object */
  virtual int StartInteraction(mmdMouse *mouse);

  /** redefined to set the picking iso-value flag */
  virtual void OnMouseMove();

  /** redefined to set the picking iso-value flag */
  virtual void OnLeftButtonDown(mafEventInteraction *e);

  /** redefined to set the picking iso-value flag and if it is true, call PickIsoValue() */
  virtual void OnButtonUp(mafEventInteraction *e);

protected:
  mmiExtractIsosurface();
  virtual ~mmiExtractIsosurface();
  
  /** send the picked point to the listener */
  void PickIsoValue(mafDevice *device);

  bool m_PickValue;

private:
  mmiExtractIsosurface(const mmiExtractIsosurface&);  // Not implemented.
  void operator=(const mmiExtractIsosurface&);  // Not implemented.
};
#endif 
