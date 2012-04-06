/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmiSelectPoint.h,v $
Language:  C++
Date:      $Date: 2012-04-06 08:19:22 $
Version:   $Revision: 1.1.2.5 $
Authors:   Matteo Giacomoni , Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmiSelectPoint_h
#define __mmiSelectPoint_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medCommonDefines.h"
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
class MED_COMMON_EXPORT mmiSelectPoint : public mafInteractorCameraMove
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
