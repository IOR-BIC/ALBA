/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionBehavior
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


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


#include "albaInteractionBehavior.h"

//#include "albaDeviceButtonsPad.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaAvatar.h"
#include "albaAvatar3D.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaEventInteraction.h"
#include "albaVME.h"
#include "mmuIdFactory.h"
#include "albaView.h"
//#include "albaViewCompound.h"

#include "albaEvent.h"

#include "vtkRenderer.h"
#include "vtkProp3D.h"
#include "vtkPoints.h"

#include <assert.h>

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
ALBA_ID_IMP(albaInteractionBehavior::INTERACTION_STARTED);
ALBA_ID_IMP(albaInteractionBehavior::INTERACTION_STOPPED);
ALBA_ID_IMP(albaInteractionBehavior::BUTTON_DOWN);
ALBA_ID_IMP(albaInteractionBehavior::BUTTON_UP);

//------------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaInteractionBehavior);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaInteractionBehavior::albaInteractionBehavior()
//------------------------------------------------------------------------------
{
  m_Device              = NULL;
  m_VME                 = NULL;
  m_Prop                = NULL;
  m_TestMode = false;
  
  m_DeviceIsSet           = false;
  m_IgnoreTriggerEvents   = false;
  m_InteractionFlag       = false;
  m_StartInteractionEvent = albaDeviceButtonsPad::GetButtonDownId();
  m_StopInteractionEvent  = albaDeviceButtonsPad::GetButtonUpId();
  m_StartButton           = 0;// Button 0
  m_Modifiers             = 0;// no modifiers
  m_ButtonMode            = SINGLE_BUTTON_MODE;
  m_ButtonsCounter        = 0;

  m_CurrentButton         = 0;
  m_CurrentModifier       = 0;
}

//------------------------------------------------------------------------------
albaInteractionBehavior::~albaInteractionBehavior()
//------------------------------------------------------------------------------
{
	m_Device = NULL;
}

//------------------------------------------------------------------------------
void albaInteractionBehavior::SetDevice(albaDevice *device)
//------------------------------------------------------------------------------
{
  if (m_Device)
    m_Device->RemoveObserver(this);

  m_Device = device; // avoid cross reference counting

  if (m_Device)
    device->AddObserver(this,MCH_INPUT);

  m_DeviceIsSet=(device!=NULL);
}

//------------------------------------------------------------------------------
void albaInteractionBehavior::SetVME(albaVME *vme)
//------------------------------------------------------------------------------
{
  m_VME=vme;
}

//------------------------------------------------------------------------------
void albaInteractionBehavior::SetProp(vtkProp3D *prop)
//------------------------------------------------------------------------------
{
  m_Prop=prop;
}

//------------------------------------------------------------------------------
bool albaInteractionBehavior::IsInteracting() 
//------------------------------------------------------------------------------
{
  return m_InteractionFlag!=0;
}

//------------------------------------------------------------------------------
bool albaInteractionBehavior::IsInteracting(albaDevice *device)
//------------------------------------------------------------------------------
{
  return ((m_InteractionFlag!=0)&&(m_Device==device));
}

//------------------------------------------------------------------------------
int albaInteractionBehavior::StartInteraction(albaDevice *device)
//------------------------------------------------------------------------------
{
  // if not already interacting start the interaction
  if (!IsInteracting())
  {
    // if device != from device set from outside
    if (m_DeviceIsSet&&device!=m_Device)
      return false;

    m_InteractionFlag=true;// lock the interactor

    // Set the device being interacting
    if (device)         
    { 
      m_Device=device;// Beware there could be more then one attached as input, set which is interacting
    }
                       
    m_ButtonsCounter=1;

    return true;
  }
  else if (m_ButtonMode==MULTI_BUTTON_MODE)
  {
    // interaction already started! return true to allow multi-button interaction
    if (IsInteracting(device))
    {
      m_ButtonsCounter++;
      return true;
    }
  }
  
  return false;
}

//------------------------------------------------------------------------------
int albaInteractionBehavior::StopInteraction(albaDevice *device)
//------------------------------------------------------------------------------
{
  // check if the interaction was ongoing and if
  // its the right device which is sending the event
  if (IsInteracting(device))
  {
    m_ButtonsCounter--;

    // if no more active button pressed, stop the interaction
    if (m_ButtonsCounter==0)
    {
      m_InteractionFlag=false;

      if (device)
      {
        if (!m_DeviceIsSet) // if device set from outside, do not reset
          m_Device=NULL;
      }
      m_CurrentButton=-1;
      m_CurrentModifier=0;
    }
    
    return true;
  }
  else
  {
    albaWarningMacro("StopInteraction without a StartInteraction or stopped by wrong device.");
  }
  return false;
}

//------------------------------------------------------------------------------
int albaInteractionBehavior::OnStartInteraction(albaEventInteraction *e)
//------------------------------------------------------------------------------
{
  albaDevice *device=albaDevice::SafeDownCast((albaDevice *)e->GetSender());
  assert(device);

  if (!StartInteraction(device))
    return false;
    
  OnButtonDown(e);
  return true;  
}
//------------------------------------------------------------------------------
int albaInteractionBehavior::OnStopInteraction(albaEventInteraction *e)
//------------------------------------------------------------------------------
{
  albaDevice *device=albaDevice::SafeDownCast((albaDevice *)e->GetSender());
  assert(device);

  if (!StopInteraction(device))
    return false;

  OnButtonUp(e);
  return true;
}

//------------------------------------------------------------------------------
void albaInteractionBehavior::OnButtonDown(albaEventInteraction *e)
//------------------------------------------------------------------------------
{
  // send a copy of the incoming event: the incoming one is still being dispatched by the device
  albaEventInteraction event;
  event.DeepCopy(e);
  event.SetId(BUTTON_DOWN);
  event.SetSender(this);
  event.SetChannel(MCH_UP);

  InvokeEvent(&event);
}
//------------------------------------------------------------------------------
void albaInteractionBehavior::OnButtonUp(albaEventInteraction *e)
//------------------------------------------------------------------------------
{
  // send a copy of the incoming event: the incoming one is still being dispatched by the device
  albaEventInteraction event;
  event.DeepCopy(e);
  event.SetId(BUTTON_UP);
  event.SetSender(this);
  event.SetChannel(MCH_UP);

  InvokeEvent(&event);
}
//------------------------------------------------------------------------------
void albaInteractionBehavior::OnEvent(albaEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);
  
  albaID id=event->GetId();
  albaID ch=event->GetChannel();
  if (ch==MCH_INPUT)
  {
    // Start the interaction if not disabled
    if ((id == m_StartInteractionEvent || id == albaDeviceButtonsPadMouse::GetMouseDClickId()) && !m_IgnoreTriggerEvents)
    {
      albaEventInteraction *e = albaEventInteraction::SafeDownCast(event);
      assert(e);

      // check if the right button has been pressed
      if ((e->GetButton() == m_StartButton || (m_StartButton<0)) && ((e->GetModifiers()&m_Modifiers) == m_Modifiers ))
      {
        // the start button has been already pressed once
        if (IsInteracting((albaDevice *)e->GetSender()))
        {
          if (m_ButtonMode == MULTI_BUTTON_MODE && m_ButtonsCounter > 0)
          {
            OnStartInteraction(e);
          }
          return;
        }

        m_CurrentButton   = e->GetButton();
        m_CurrentModifier = e->GetModifiers();
        if (OnStartInteraction(e))
        {
          if (IsInteracting()) 
            InvokeEvent(this,INTERACTION_STARTED);
        }
      }
      else if (m_ButtonMode == MULTI_BUTTON_MODE && m_ButtonsCounter > 0)
      {
        // in case of MULTI BUTTON and the start button has already started 
        // the interaction we call anyway the OnStartInteraction function
        OnStartInteraction(e);
      }
    }
    
    // Stop the interaction if not disabled
    else if (id==m_StopInteractionEvent && !m_IgnoreTriggerEvents)
    {
      albaEventInteraction *e = albaEventInteraction::SafeDownCast(event);
      assert(e);

      // check if the right button has been released
      if (e->GetButton() == m_StartButton || (m_StartButton<0))
      {
        if (OnStopInteraction(e))
          if (!IsInteracting()) 
            InvokeEvent(this,INTERACTION_STOPPED);

      }
      else if (m_ButtonMode == MULTI_BUTTON_MODE && m_ButtonsCounter > 0)
      {
        // in case of MULTI BUTTON and the start button has already started 
        // the interaction we call anyway the OnStopInteraction function
        if (OnStopInteraction(e))
          if (!IsInteracting())
            InvokeEvent(this,INTERACTION_STOPPED);
      }
    }
  }
  else
  {
    Superclass::OnEvent(event);
  }
}

//----------------------------------------------------------------------------
/*bool albaInteractionBehavior::FindPokedVme(albaDevice *device,albaMatrix &point_pose,vtkProp3D *&picked_prop,albaVME *&picked_vme,albaInteractionBehavior *&picked_behavior)
//----------------------------------------------------------------------------
{
  albaView *v = NULL;
  albaDeviceButtonsPadTracker *tracker = NULL;
  albaDeviceButtonsPadMouse   *mouse   = NULL;
  albaMatrix world_pose;
  int mouse_pos[2];

  if (tracker = albaDeviceButtonsPadTracker::SafeDownCast(device))
  {
    albaMatrix &tracker_pose = point_pose;
    albaAvatar *avatar = tracker->GetAvatar();
    if (avatar)
    {
      albaAvatar3D *avatar3D = albaAvatar3D::SafeDownCast(avatar);
      if (avatar3D)
        avatar3D->TrackerToWorld(tracker_pose,world_pose,albaAvatar3D::CANONICAL_TO_WORLD_SCALE);
      else
        world_pose = tracker_pose;
      v = avatar->GetView();
    }
  }
  else if (mouse = albaDeviceButtonsPadMouse::SafeDownCast(device))
  { 
    mouse_pos[1] = (int)point_pose.GetElement(1,3);
    mouse_pos[0] = (int)point_pose.GetElement(0,3);
    v = mouse->GetView();
  }
  if (v)
  {
    albaViewCompound *vc = albaViewCompound::SafeDownCast(v);
    if (vc)
      v = vc->GetSubView();
    bool picked_something = false;
    if (tracker)
      picked_something = v->Pick(world_pose);
    else
      picked_something = v->Pick(mouse_pos[0], mouse_pos[1]);
    if(picked_something)
    {
      picked_vme = v->GetPickedVme();
      picked_prop = v->GetPickedProp();
      picked_behavior = picked_vme->GetBehavior();
      return true;
    }
  }
  return false;
}
*/
