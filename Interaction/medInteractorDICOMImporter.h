/*=========================================================================

 Program: MAF2
 Module: medInteractorDICOMImporter
 Authors: Paolo Quadrani , Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medInteractorDICOMImporter_h
#define __medInteractorDICOMImporter_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafInteractorCameraMove.h"
#include "mafMTime.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkCamera;

/** Implements mouse move of the camera in the scene and notify the listener with world coordinates
on left mouse pick , left mouse move and left mouse up.

This class implements a mouse move of the renderer camera in the scene. The interaction
modality is a mouse manipulation, where movements of the mouse are mapped
into movements of the camera. 

On left button down: Send pick position in world coordinates to the listener
On mouse move: Send cursor position in world coordinates to the listener
On left button up: Send cursor position in world coordinates to the listener

Picked World Coordinates position is sent as single vtkPoint to the listener
in the following way:
-----------------
vtkMAFSmartPointer<vtkPoints> position_point; 
position_point->SetNumberOfPoints(1);
position_point->SetPoint(0,wp[0],wp[1],wp[2]);

mafEventMacro(mafEvent(this, event_id, position_point.GetPointer()));
-----------------

@sa medOpImporterDicomOffis where this class is used in the Dicom importer wizard dialog

*/
class MAF_EXPORT medInteractorDICOMImporter : public mafInteractorCameraMove
{
public:
  mafTypeMacro(medInteractorDICOMImporter,mafInteractorCameraMove);

  /** Start the interaction with the given device*/
  virtual int StartInteraction(mafDeviceButtonsPadMouse *mouse);

  /** redefined to send the picking world coordinates also */
  virtual void OnMouseMove();

  /** redefined to send the picking world coordinates also */
  virtual void OnLeftButtonDown(mafEventInteraction *e);

  /** redefined to send the picking world coordinates also */
  virtual void OnLeftButtonUp();

protected:
  medInteractorDICOMImporter();
  virtual ~medInteractorDICOMImporter();

  /** Compute the world point corresponding to the mouse position and send it to the listener */
  void SendCropPosition(int event_id);
  
private:
  medInteractorDICOMImporter(const medInteractorDICOMImporter&);  // Not implemented.
  void operator=(const medInteractorDICOMImporter&);  // Not implemented.
};
#endif 
