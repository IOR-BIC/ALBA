/*=========================================================================

 Program: ALBA
 Module: albaInteractorCompositorMouseFloatVME
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaInteractorCompositorMouseFloatVME_h
#define __albaInteractorCompositorMouseFloatVME_h

#include "albaInteractorCompositorMouse.h"

//----------------------------------------------------------------------------
//forward ref
//----------------------------------------------------------------------------
class albaInteractorCameraMove;
class albaInteractorGenericMouseFloatVME;

//----------------------------------------------------------------------------
/** Class name: albaInteractorCompositorMouseFloatVME
This class derived from albaInteractorCompositorMouse customize its OnEvent method: in some cases,
depending on current view and its camera position, rotational transformation axis are VME axis.
*/
class ALBA_EXPORT albaInteractorCompositorMouseFloatVME : public albaInteractorCompositorMouse
{
public:
  albaTypeMacro(albaInteractorCompositorMouseFloatVME, albaInteractorCompositorMouse);

  /**  Process events coming from the mouse */
  virtual void OnEvent(albaEventBase *event);

	void InitInteraction(int buttonPressed, albaEventInteraction *e);

	void OnButtonDown(albaEventInteraction *e);

	void OnButtonUp(albaEventInteraction *e);
	
	/**  Create a behavior given the activator*/
	albaInteractorGenericMouse *CreateBehavior(MMI_ACTIVATOR activator);
	  
protected:
	typedef std::map<int, albaAutoPointer<albaInteractorGenericMouseFloatVME> > mmuActivatorArbitraryMap;
	mmuActivatorArbitraryMap m_ActivatorInteractorMap;

private:

	int m_MousePose[2];
	int m_LastMousePose[2];
	int m_ButtonPressed;
};
#endif

