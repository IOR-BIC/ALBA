/*=========================================================================

 Program: MAF2
 Module: mafInteractorSelectCell
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafInteractorSelectCell_h
#define __mafInteractorSelectCell_h

#include "mafInteractorCameraMove.h"
#include "mafMTime.h"

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
  mafEvent(this,VME_PICKED,pickedPoint,pickedCellId);
  is issued.
  
  to retrieve picking information client should use some code like this:

  pointFromEvent = (vtkPoints *)e->GetVtkObj();
  pointFromEvent->GetPoint(0,pointCoordinates);
  int cellID = e->GetArg();

  @sa mafOpRemoveCells for a usage example 


*/
class MAF_EXPORT mafInteractorSelectCell : public mafInteractorCameraMove
{
public:
  mafTypeMacro(mafInteractorSelectCell,mafInteractorCameraMove);

  /** redefined not to move the camera if something has been picked */
  virtual void OnMouseMove();

  /** redefined to pick cell if CTRL modifier is pressed */
  virtual void OnLeftButtonDown(mafEventInteraction *e);
	
  /** redefined to end pick modality */
  virtual void OnButtonUp(mafEventInteraction *e);

protected:
  mafInteractorSelectCell();
  virtual ~mafInteractorSelectCell();
  
  /** send the picking informations to the listener */
  void PickCell(mafDevice *device);

  bool m_IsPicking;

private:
  mafInteractorSelectCell(const mafInteractorSelectCell&);  // Not implemented.
  void operator=(const mafInteractorSelectCell&);  // Not implemented.
	void OnEvent(mafEventBase *event);
  
};
#endif 
