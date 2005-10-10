/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiDICOMImporterInteractor.h,v $
  Language:  C++
  Date:      $Date: 2005-10-10 13:06:06 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmiDICOMImporterInteractor_h
#define __mmiDICOMImporterInteractor_h

#include "mmiCameraMove.h"
#include "mafMTime.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkCamera;

/** implements mouse move of camera in the scene.
  This class implements a mouse move of the renderer camera in the scene. The interaction
  modality is a mouse manipulation, where movements of the mouse are mapped
  into movements of the camera. More details to be written...*/
class mmiDICOMImporterInteractor : public mmiCameraMove
{
public:
  mafTypeMacro(mmiDICOMImporterInteractor,mmiCameraMove);

  /** Start the interaction with the selected object */
  virtual int StartInteraction(mmdMouse *mouse);

  /** redefined to set the picking iso-value flag */
  virtual void OnMouseMove();

  /** redefined to set the picking iso-value flag */
  virtual void OnLeftButtonDown(mafEventInteraction *e);

  /** redefined to set the picking iso-value flag and if it is true, call PickIsoValue() */
  virtual void OnLeftButtonUp();

protected:
  mmiDICOMImporterInteractor();
  virtual ~mmiDICOMImporterInteractor();

  /** Compute the world point corresponding to the mouse position and send it to the listener to allow to draw the crop box. */
  void SendCropPosition(int event_id);
  
private:
  mmiDICOMImporterInteractor(const mmiDICOMImporterInteractor&);  // Not implemented.
  void operator=(const mmiDICOMImporterInteractor&);  // Not implemented.
};
#endif 
