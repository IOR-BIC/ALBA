/*=========================================================================

 Program: MAF2Medical
 Module: mmiSelectPoint
 Authors: Matteo Giacomoni , Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mmiSelectPoint_h
#define __mmiSelectPoint_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafInteractorCameraMove.h"
#include "mafMTime.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkCellPicker;
class vtkCamera;

/** Perform continuous cell picking with or without the CTRL modifier or move the camera
in the scene (when not picking)

This class implements a mouse move of the renderer camera in the scene. The interaction
modality is a mouse manipulation, where movements of the mouse are mapped
into movements of the camera. 

A pick can be set to happen with LeftMouseButton + CTRL or LeftMouseButton only

When the pick is performed a:

mafEvent(this,VME_PICKED,pickedPoint,pickedCellId);

is issued.

To retrieve picking information client should use some code like this:

pointFromEvent = (vtkPoints *)e->GetVtkObj();
pointFromEvent->GetPoint(0,pointCoordinates);
int cellID = e->GetArg();

*/
class MAF_EXPORT mmiSelectPoint : public mafInteractorCameraMove
{
public:
	mafTypeMacro(mmiSelectPoint,mafInteractorCameraMove);

	/** redefined not to move the camera if something has been picked */
	virtual void OnMouseMove();

	/** redefined to pick cell if CTRL modifier is pressed */
	virtual void OnLeftButtonDown(mafEventInteraction *e);

	/** redefined to end pick modality */
	virtual void OnButtonUp(mafEventInteraction *e);

  /** Set the CTRL modifier to On => picking performed on LeftButtonDown + CTRL */
  void SetCtrlModifierOn(){m_UseCtrlModifier = true;};

  /** Set the CTRL modifier to Off => pick performed on Left button down*/
  void SetCtrlModifierOff(){m_UseCtrlModifier = false;};

  /** Set the CTRL modifier: 
  true => pick on Ctrl + LeftButtonDown
  false => pick on LeftButtonDown only */
  void SetCtrlModifier(bool useCtrlModifier) {m_UseCtrlModifier = useCtrlModifier;};
  
  /** Get the CTRL modifier */
  bool GetCtrlModifier() {return m_UseCtrlModifier;};

protected:
	mmiSelectPoint();
	virtual ~mmiSelectPoint();

	/** send the picking informations to the listener */
	void PickCell(mafDevice *device);

	bool m_IsPicking;

  bool m_UseCtrlModifier;

private:
	mmiSelectPoint(const mmiSelectPoint&);  // Not implemented.
	void operator=(const mmiSelectPoint&);  // Not implemented.
	void OnEvent(mafEventBase *event);

};
#endif 
