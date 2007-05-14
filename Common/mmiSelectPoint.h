/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmiSelectPoint.h,v $
Language:  C++
Date:      $Date: 2007-05-14 09:51:09 $
Version:   $Revision: 1.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmiSelectPoint_h
#define __mmiSelectPoint_h

#include "mmiCameraMove.h"
#include "mafMTime.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkCellPicker;
class vtkCamera;

/** implements mouse move of camera in the scene or perform continuous cell picking
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

*/
class mmiSelectPoint : public mmiCameraMove
{
public:
	mafTypeMacro(mmiSelectPoint,mmiCameraMove);

	/** redefined not to move the camera if something has been picked */
	virtual void OnMouseMove();

	/** redefined to pick cell if CTRL modifier is pressed */
	virtual void OnLeftButtonDown(mafEventInteraction *e);

	/** redefined to end pick modality */
	virtual void OnButtonUp(mafEventInteraction *e);

protected:
	mmiSelectPoint();
	virtual ~mmiSelectPoint();

	/** send the picking informations to the listener */
	void PickCell(mafDevice *device);

	bool m_IsPicking;

private:
	mmiSelectPoint(const mmiSelectPoint&);  // Not implemented.
	void operator=(const mmiSelectPoint&);  // Not implemented.
	void OnEvent(mafEventBase *event);

};
#endif 
