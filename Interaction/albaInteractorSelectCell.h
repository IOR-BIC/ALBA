/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorSelectCell
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaInteractorSelectCell_h
#define __albaInteractorSelectCell_h

#include "albaInteractorCameraMove.h"
#include "albaMTime.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkCellPicker;
class vtkCamera;

/** Implements mouse move of camera in the scene or perform continuous cell picking
  when using CTRL modifier.
  This class implements a mouse move of the renderer camera in the scene. The interaction
  modality is a mouse manipulation, where movements of the mouse are mapped
  into movements of the camera. 
  If CTRL modifier is pressed a pick is performed and a 
  albaEvent(this,VME_PICKED,pickedPoint,pickedCellId);
  is issued.
  
  to retrieve picking information client should use some code like this:

  pointFromEvent = (vtkPoints *)e->GetVtkObj();
  pointFromEvent->GetPoint(0,pointCoordinates);
  int cellID = e->GetArg();

  @sa albaOpRemoveCells for a usage example 


*/
class ALBA_EXPORT albaInteractorSelectCell : public albaInteractorCameraMove
{
public:
  albaTypeMacro(albaInteractorSelectCell,albaInteractorCameraMove);

  /** redefined not to move the camera if something has been picked */
  virtual void OnMouseMove();

  /** redefined to pick cell if CTRL modifier is pressed */
  virtual void OnLeftButtonDown(albaEventInteraction *e);
	
  /** redefined to end pick modality */
  virtual void OnButtonUp(albaEventInteraction *e);

protected:
  albaInteractorSelectCell();
  virtual ~albaInteractorSelectCell();
  
  /** send the picking informations to the listener */
  void PickCell(albaDevice *device);

  bool m_IsPicking;

private:
  albaInteractorSelectCell(const albaInteractorSelectCell&);  // Not implemented.
  void operator=(const albaInteractorSelectCell&);  // Not implemented.
	void OnEvent(albaEventBase *event);
  
};
#endif 
