/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorDICOMImporter
 Authors: Paolo Quadrani , Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaInteractorDICOMImporter_h
#define __albaInteractorDICOMImporter_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaInteractorCameraMove.h"
#include "albaMTime.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkCamera;
class vtkPlaneSource;
class vtkActor;
class albaRWI;
class albaRWIBase;

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
vtkALBASmartPointer<vtkPoints> position_point; 
position_point->SetNumberOfPoints(1);
position_point->SetPoint(0,wp[0],wp[1],wp[2]);

albaEventMacro(albaEvent(this, event_id, position_point.GetPointer()));
-----------------

@sa albaOpImporterDicomOffis where this class is used in the Dicom importer wizard dialog

*/
class ALBA_EXPORT albaInteractorDICOMImporter : public albaInteractorCameraMove
{
public:
  albaTypeMacro(albaInteractorDICOMImporter,albaInteractorCameraMove);

  /** Start the interaction with the given device*/
  virtual int StartInteraction(albaDeviceButtonsPadMouse *mouse);

  /** redefined to send the picking world coordinates also */
  virtual void OnMouseMove();

  /** redefined to send the picking world coordinates also */
  virtual void OnLeftButtonDown(albaEventInteraction *e);

	virtual void SetRendererAndView(vtkRenderer *ren, albaView *view);

	void SetRWI(albaRWI *rwi);
	
	void CalculateSideDragged(double * pos);

	virtual void OnEvent(albaEventBase *event);

	/** redefined to send the picking world coordinates also */
  virtual void OnLeftButtonUp();

	void GetPlaneBounds(double *bounds);

	void SetSliceBounds(double *bounds);
	
	void PlaneVisibilityOn();

	void PlaneVisibilityOff();;

protected:
  albaInteractorDICOMImporter();
  virtual ~albaInteractorDICOMImporter();

	void SetPlaneFromSliceSize();


	vtkPlaneSource		*m_CropPlane;
	vtkActor					*m_CropActor;
	int	m_GizmoStatus;
	bool m_PlaneVisibility;
	int	m_SideToBeDragged;
	double m_SliceSize[2];
	double m_Delta;
	albaRWIBase *m_RWIbase;
	wxStockCursor m_CurrentArrow;
  
private:
  albaInteractorDICOMImporter(const albaInteractorDICOMImporter&);  // Not implemented.
  void operator=(const albaInteractorDICOMImporter&);  // Not implemented.
};
#endif 
