/*=========================================================================

 Program: MAF2
 Module: mafInteractorDICOMImporter
 Authors: Paolo Quadrani , Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafInteractorDICOMImporter_h
#define __mafInteractorDICOMImporter_h

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
class vtkPlaneSource;
class vtkActor;
class mafRWI;
class mafRWIBase;

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

@sa mafOpImporterDicomOffis where this class is used in the Dicom importer wizard dialog

*/
class MAF_EXPORT mafInteractorDICOMImporter : public mafInteractorCameraMove
{
public:
  mafTypeMacro(mafInteractorDICOMImporter,mafInteractorCameraMove);

  /** Start the interaction with the given device*/
  virtual int StartInteraction(mafDeviceButtonsPadMouse *mouse);

  /** redefined to send the picking world coordinates also */
  virtual void OnMouseMove();

  /** redefined to send the picking world coordinates also */
  virtual void OnLeftButtonDown(mafEventInteraction *e);

	virtual void SetRenderer(vtkRenderer *ren);

	void SetRWI(mafRWI *rwi);
	
	void CalculateSideDragged(double * pos);

	virtual void OnEvent(mafEventBase *event);

	/** redefined to send the picking world coordinates also */
  virtual void OnLeftButtonUp();

	void GetPlaneBounds(double *bounds);

	void SetSliceBounds(double *bounds);
	
	void PlaneVisibilityOn();

	void PlaneVisibilityOff();;

protected:
  mafInteractorDICOMImporter();
  virtual ~mafInteractorDICOMImporter();

	void SetPlaneFromSliceSize();


	vtkPlaneSource		*m_CropPlane;
	vtkActor					*m_CropActor;
	int	m_GizmoStatus;
	bool m_PlaneVisibility;
	int	m_SideToBeDragged;
	double m_SliceSize[2];
	double m_Delta;
	mafRWIBase *m_RWIbase;
	int m_CurrentArrow;
  
private:
  mafInteractorDICOMImporter(const mafInteractorDICOMImporter&);  // Not implemented.
  void operator=(const mafInteractorDICOMImporter&);  // Not implemented.
};
#endif 
