/*=========================================================================

 Program: ALBA
 Module: appInteractorCompositorMouse
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaInteractorCompositorMouseFloatVME.h"
#include "albaInteractorGenericMouseFloatVME.h"

#include "albaDeviceButtonsPadMouse.h"
#include "albaEventInteraction.h"
#include "albaInteractorCameraMove.h"
#include "albaView.h"

#include "vtkCamera.h"
#include "vtkRenderer.h"


albaCxxTypeMacro(albaInteractorCompositorMouseFloatVME);

//----------------------------------------------------------------------------
albaInteractorGenericMouse *albaInteractorCompositorMouseFloatVME::CreateBehavior(MMI_ACTIVATOR activator)
{
  // create the generic behavior and populate the map
  albaInteractorGenericMouseFloatVME *pISAG = albaInteractorGenericMouseFloatVME::New();
  
	m_ActivatorInteractorMap[activator] = pISAG;
  switch (activator)
  {    
    case MOUSE_LEFT:
      pISAG->SetStartButton(ALBA_LEFT_BUTTON);
      break;
    case MOUSE_LEFT_SHIFT:
      pISAG->SetStartButton(ALBA_LEFT_BUTTON);
      pISAG->SetModifiers(ALBA_SHIFT_KEY);
      break;
    case MOUSE_LEFT_CONTROL:
      pISAG->SetStartButton(ALBA_LEFT_BUTTON);
      pISAG->SetModifiers(ALBA_CTRL_KEY);
      break;
    case MOUSE_MIDDLE:
      pISAG->SetStartButton(ALBA_MIDDLE_BUTTON);
      break;
    case MOUSE_MIDDLE_SHIFT:
      pISAG->SetStartButton(ALBA_MIDDLE_BUTTON);
      pISAG->SetModifiers(ALBA_SHIFT_KEY);
      break;
    case MOUSE_MIDDLE_CONTROL:
      pISAG->SetStartButton(ALBA_MIDDLE_BUTTON);
      pISAG->SetModifiers(ALBA_CTRL_KEY);
      break;
    case MOUSE_RIGHT:
      pISAG->SetStartButton(ALBA_RIGHT_BUTTON);
      break;
    case MOUSE_RIGHT_SHIFT:
      pISAG->SetStartButton(ALBA_RIGHT_BUTTON);
      pISAG->SetModifiers(ALBA_SHIFT_KEY);
      break;
    case MOUSE_RIGHT_CONTROL:
      pISAG->SetStartButton(ALBA_RIGHT_BUTTON);
      pISAG->SetModifiers(ALBA_CTRL_KEY);
      break;
  }
  
  return pISAG;
}


//------------------------------------------------------------------------------
void albaInteractorCompositorMouseFloatVME::OnEvent(albaEventBase *event)
{  
	assert(event);
	assert(event->GetSender());

	albaID id = event->GetId();
	albaID channel = event->GetChannel();

	if (channel == MCH_INPUT && m_InteractionFlag)
	{
		albaEventInteraction *e = (albaEventInteraction *)event;
		albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::SafeDownCast(GetDevice());

		// if the event comes from tracker which started the interaction continue...
		// Move2DEvent handling
		if (id == albaDeviceButtonsPadMouse::GetMouse2DMoveId() && mouse)
		{

			double pos2d[2];
			e->Get2DPosition(pos2d);

			// get the mouse pose and forward it to the active interactor 
			m_MousePose[0] = (int)pos2d[0];
			m_MousePose[1] = (int)pos2d[1];

			// if the position has not changed discard OnMouseMoveAction
			if (!(m_MousePose[0] == m_LastMousePose[0] && m_MousePose[1] == m_LastMousePose[1]))
			{
				if (m_ActiveMMIGeneric)
				{
					m_ActiveMMIGeneric->OnMouseMoveAction(m_MousePose[0], m_MousePose[1]);
					m_LastMousePose[0] = m_MousePose[0];
					m_LastMousePose[1] = m_MousePose[1];
				}
			}
			return;
		}
	}

	//Superclass::OnEvent(event);
	Superclass::OnEvent(event);
}

//------------------------------------------------------------------------------
void albaInteractorCompositorMouseFloatVME::InitInteraction(int buttonPressed, albaEventInteraction *e)
{
	double pos[2];
	e->Get2DPosition(pos);

	// fill this object MousePos ivar
	m_LastMousePose[0] = m_MousePose[0] = (int)pos[0];
	m_LastMousePose[1] = m_MousePose[1] = (int)pos[1];

	albaDeviceButtonsPadMouse *mouse = (albaDeviceButtonsPadMouse *)e->GetSender();

	// get info from picked vme
	albaVME        *picked_vme = NULL;
	vtkProp3D     *picked_prop = NULL;
	albaInteractor *picked_bh = NULL;
	albaMatrix     point_pose;

	point_pose.SetElement(0, 3, m_MousePose[0]);
	point_pose.SetElement(1, 3, m_MousePose[1]);

	albaView *v = mouse->GetView();
	assert(v);
	v->FindPokedVme(mouse, point_pose, picked_prop, picked_vme, picked_bh);

	// Set the m_Renderer and m_CurrentCamera ivar
	StartInteraction(mouse);

	// check for ctrl and shift pressure
	int shiftPressed = e->GetModifier(ALBA_SHIFT_KEY) ? 1 : 0;
	int ctrlPressed = e->GetModifier(ALBA_CTRL_KEY) ? 1 : 0;

	// get the activator from key mouse combination:
	int activator = MouseToActivator(buttonPressed, shiftPressed, ctrlPressed);

	// get the behavior from the m_ActivatorMap
	if (m_ActivatorInteractorMap.count(activator))
	{
		m_ActiveMMIGeneric = m_ActivatorInteractorMap[activator];
		if (m_ActiveMMIGeneric != NULL)
		{
			// m_ActiveMMIGeneric is the current active mouse behavior

			m_ActiveMMIGeneric->SetRendererAndView(m_Renderer, v);
			m_ActiveMMIGeneric->SetProp(picked_prop);
			m_ActiveMMIGeneric->SetCurrentCamera(m_Renderer->GetActiveCamera());
			if (m_ActiveMMIGeneric->StartInteraction(mouse)) // force 
			{
				m_ActiveMMIGeneric->OnButtonDownAction(m_MousePose[0], m_MousePose[1]);
			}
		}
	}
	else
		// no behavior is associated with current key combo
	{
		m_ActiveMMIGeneric = NULL;
	}
}

//----------------------------------------------------------------------------
void albaInteractorCompositorMouseFloatVME::OnButtonDown(albaEventInteraction *e)
{
	// if in multi button mode and already interacting simply forward the event...
	if (m_ActiveMMIGeneric && m_ButtonMode == MULTI_BUTTON_MODE)
	{
		m_ActiveMMIGeneric->OnEvent(e);
		return;
	}

	// get the button pressed
	m_ButtonPressed = e->GetButton();

	double pos[2];
	e->Get2DPosition(pos);
	m_LastMousePose[0] = m_MousePose[0] = (int)pos[0];
	m_LastMousePose[1] = m_MousePose[1] = (int)pos[1];

	switch (m_ButtonPressed)
	{
	case ALBA_LEFT_BUTTON:
		InitInteraction(MOUSE_LEFT_PRESSED, e);
		break;
	case ALBA_MIDDLE_BUTTON:
		InitInteraction(MOUSE_MIDDLE_PRESSED, e);
		break;
	case ALBA_RIGHT_BUTTON:
		InitInteraction(MOUSE_RIGHT_PRESSED, e);
		break;
	}
}

//----------------------------------------------------------------------------
void albaInteractorCompositorMouseFloatVME::OnButtonUp(albaEventInteraction *e)
{
	m_ButtonPressed = e->GetButton();
	if (m_CurrentButton >= 0 && m_ButtonPressed != m_CurrentButton)
	{
		if (m_ActiveMMIGeneric && m_ButtonMode == MULTI_BUTTON_MODE)
			m_ActiveMMIGeneric->OnEvent(e);

		return;
	}

	switch (m_ButtonPressed)
	{
		case ALBA_LEFT_BUTTON:
		{
			if (m_ActiveMMIGeneric)
			{
				albaDeviceButtonsPadMouse *mouse = (albaDeviceButtonsPadMouse *)e->GetSender();
				if (m_ActiveMMIGeneric->StopInteraction(mouse)) // stop interaction
				{
					m_ActiveMMIGeneric->OnButtonUpAction();
					m_ActiveMMIGeneric = NULL;
				}
			}
		}
		break;
		case ALBA_MIDDLE_BUTTON:
			OnMiddleButtonUp(e);
			break;
		case ALBA_RIGHT_BUTTON:
			OnRightButtonUp(e);
			break;
	}
}
