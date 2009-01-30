/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiSelectCell.h,v $
  Language:  C++
  Date:      $Date: 2009-01-30 10:08:04 $
  Version:   $Revision: 1.3.4.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmiSelectCell_h
#define __mmiSelectCell_h

#include "mmiCameraMove.h"
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
class mmiSelectCell : public mmiCameraMove
{
public:
  mafTypeMacro(mmiSelectCell,mmiCameraMove);

  /** redefined not to move the camera if something has been picked */
  virtual void OnMouseMove();

  /** redefined to pick cell if CTRL modifier is pressed */
  virtual void OnLeftButtonDown(mafEventInteraction *e);
	
  /** redefined to end pick modality */
  virtual void OnButtonUp(mafEventInteraction *e);

protected:
  mmiSelectCell();
  virtual ~mmiSelectCell();
  
  /** send the picking informations to the listener */
  void PickCell(mafDevice *device);

  bool m_IsPicking;

private:
  mmiSelectCell(const mmiSelectCell&);  // Not implemented.
  void operator=(const mmiSelectCell&);  // Not implemented.
	void OnEvent(mafEventBase *event);
  
};
#endif 
