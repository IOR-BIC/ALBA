/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafInteractor.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-02 10:41:04 $
  Version:   $Revision: 1.7 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafInteractor.h"

//#include "mmdButtonsPad.h"
#include "mmdTracker.h"
#include "mafAvatar.h"
#include "mafAvatar3D.h"
#include "mmdMouse.h"
#include "mafEventInteraction.h"
#include "mafVME.h"
#include "mmuIdFactory.h"
#include "mafView.h"
#include "mafViewCompound.h"

#include "vtkRenderer.h"
#include "vtkProp3D.h"

#include <assert.h>

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafInteractor::INTERACTION_STARTED);
MAF_ID_IMP(mafInteractor::INTERACTION_STOPPED);
MAF_ID_IMP(mafInteractor::BUTTON_DOWN);
MAF_ID_IMP(mafInteractor::BUTTON_UP);

//------------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafInteractor);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafInteractor::mafInteractor()
//------------------------------------------------------------------------------
{
  m_Renderer            = NULL;
  m_Device              = NULL;
  m_VME                 = NULL;
  m_Prop                = NULL;
  
  m_LockDevice            = true;
  m_DeviceIsSet           = false;
  m_IgnoreTriggerEvents   = false;
  m_InteractionFlag       = false;
  m_StartInteractionEvent = mmdButtonsPad::BUTTON_DOWN;
  m_StopInteractionEvent  = mmdButtonsPad::BUTTON_UP;
  m_StartButton           = 0;// Button 0
  m_Modifiers             = 0;// no modifiers
  m_ButtonMode            = SINGLE_BUTTON_MODE;
  m_ButtonsCounter        = 0;

  m_CurrentButton         = 0;
  m_CurrentModifier       = 0;
}

//------------------------------------------------------------------------------
mafInteractor::~mafInteractor()
//------------------------------------------------------------------------------
{
  if (IsInteracting()&&m_Device&&m_LockDevice)
    m_Device->Unlock();
  
  m_Device = NULL;
}

//------------------------------------------------------------------------------
void mafInteractor::SetDevice(mafDevice *device)
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
void mafInteractor::SetRenderer(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  m_Renderer = ren;
}
//------------------------------------------------------------------------------
void mafInteractor::SetVME(mafVME *vme)
//------------------------------------------------------------------------------
{
  m_VME=vme;
}

//------------------------------------------------------------------------------
void mafInteractor::SetProp(vtkProp3D *prop)
//------------------------------------------------------------------------------
{
  m_Prop=prop;
}

//------------------------------------------------------------------------------
bool mafInteractor::IsInteracting() 
//------------------------------------------------------------------------------
{
  return m_InteractionFlag!=0;
}

//------------------------------------------------------------------------------
bool mafInteractor::IsInteracting(mafDevice *device)
//------------------------------------------------------------------------------
{
  return ((m_InteractionFlag!=0)&&(m_Device==device));
}

//------------------------------------------------------------------------------
int mafInteractor::StartInteraction(mafDevice *device)
//------------------------------------------------------------------------------
{
  // if not already interacting start the interaction
  if (!IsInteracting())
  {
    // if device already in use return
    if (device&&m_LockDevice&&device->IsLocked())
      return false;

    // if device != from device set from outside
    if (m_DeviceIsSet&&device!=m_Device)
      return false;

    m_InteractionFlag=true;// lock the interactor

    // Set the device being interacting
    if (device)         
    { 
      m_Device=device;// Beware there could be more then one attached as input, set which is interacting
      if (m_LockDevice)
        device->Lock();
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
int mafInteractor::StopInteraction(mafDevice *device)
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
        if (m_LockDevice)
          device->Unlock();

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
    mafWarningMacro("StopInteraction without a StartInteraction or stopped by wrong device.");
  }
  return false;
}

//------------------------------------------------------------------------------
int mafInteractor::OnStartInteraction(mafEventInteraction *e)
//------------------------------------------------------------------------------
{
  mafDevice *device=mafDevice::SafeDownCast((mafDevice *)e->GetSender());
  assert(device);

  if (!StartInteraction(device))
    return false;
    
  OnButtonDown(e);
  return true;  
}
//------------------------------------------------------------------------------
int mafInteractor::OnStopInteraction(mafEventInteraction *e)
//------------------------------------------------------------------------------
{
  mafDevice *device=mafDevice::SafeDownCast((mafDevice *)e->GetSender());
  assert(device);

  if (!StopInteraction(device))
    return false;

  OnButtonUp(e);
  return true;
}

//------------------------------------------------------------------------------
void mafInteractor::OnButtonDown(mafEventInteraction *e)
//------------------------------------------------------------------------------
{ 
  // create a new event copying data from the incoming one
  mafEventBase *event=e->NewInstance();
  e->DeepCopy(e);
  e->SetId(BUTTON_DOWN);
  e->SetSender(this);
  e->SetChannel(MCH_UP);
  
  InvokeEvent(e);
  
}
//------------------------------------------------------------------------------
void mafInteractor::OnButtonUp(mafEventInteraction *e)
//------------------------------------------------------------------------------
{
  // create a new event copying data from the incoming one
  mafEventBase *event=e->NewInstance();
  e->DeepCopy(e);
  e->SetId(BUTTON_UP);
  e->SetSender(this);
  e->SetChannel(MCH_UP);

  InvokeEvent(e);
}
//------------------------------------------------------------------------------
void mafInteractor::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);
  
  mafID id=event->GetId();
  mafID ch=event->GetChannel();
  if (ch==MCH_INPUT)
  {
    // Start the interaction if not disabled
    if (id==m_StartInteractionEvent && !m_IgnoreTriggerEvents)
    {
      mafEventInteraction *e=mafEventInteraction::SafeDownCast(event);
      assert(e);

      // check if the right button has been pressed
      if ((e->GetButton()==m_StartButton || (m_StartButton<0)) && ((e->GetModifiers()&m_Modifiers) == m_Modifiers ))
      {
        // the start button has been already pressed once
        if (IsInteracting((mafDevice *)e->GetSender()))
        {
          if (m_ButtonMode==MULTI_BUTTON_MODE && m_ButtonsCounter > 0)
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
            InvokeEvent(INTERACTION_STARTED);
        }
      }
      else if (m_ButtonMode==MULTI_BUTTON_MODE && m_ButtonsCounter > 0)
      {
        // in case of MULTI BUTTON and the start button has already started 
        // the interaction we call anyway the OnStartInteraction function
        OnStartInteraction(e);
      }
    }
    
    // Stop the interaction if not disabled
    else if (id==m_StopInteractionEvent && !m_IgnoreTriggerEvents)
    {
      mafEventInteraction *e=mafEventInteraction::SafeDownCast(event);
      assert(e);

      // check if the right button has been released
      if (e->GetButton()==m_StartButton || (m_StartButton<0))
      {
        if (OnStopInteraction(e))
          if (!IsInteracting()) 
            InvokeEvent(INTERACTION_STOPPED);

      }
      else if (m_ButtonMode==MULTI_BUTTON_MODE && m_ButtonsCounter > 0)
      {
        // in case of MULTI BUTTON and the start button has already started 
        // the interaction we call anyway the OnStopInteraction function
        if (OnStopInteraction(e))
          if (!IsInteracting())
            InvokeEvent(INTERACTION_STOPPED);
      }
    }
  }
  else
  {
    Superclass::OnEvent(event);
  }
}

//----------------------------------------------------------------------------
void mafInteractor::ComputeDisplayToWorld(double x, double y, double z, double worldPt[4])
//----------------------------------------------------------------------------
{
  if ( !m_Renderer ) 
    return;
  
  m_Renderer->SetDisplayPoint(x, y, z);
  m_Renderer->DisplayToWorld();
  m_Renderer->GetWorldPoint(worldPt);
  if (worldPt[3])
  {
    worldPt[0] /= worldPt[3];
    worldPt[1] /= worldPt[3];
    worldPt[2] /= worldPt[3];
    worldPt[3] = 1.0;
  }
}

//----------------------------------------------------------------------------
void mafInteractor::ComputeWorldToDisplay(double x, double y, double z, double displayPt[3])
//----------------------------------------------------------------------------
{
  if ( !m_Renderer ) 
    return;
  
  m_Renderer->SetWorldPoint(x, y, z, 1.0);
  m_Renderer->WorldToDisplay();
  m_Renderer->GetDisplayPoint(displayPt);
}
//----------------------------------------------------------------------------
bool mafInteractor::FindPokedVme(mafDevice *device,mafMatrix &point_pose,vtkProp3D *&picked_prop,mafVME *&picked_vme,mafInteractor *&picked_behavior)
//----------------------------------------------------------------------------
{
  if (mmdTracker *tracker = mmdTracker::SafeDownCast(device))
  {
    mafMatrix &tracker_pose = point_pose;
    mafAvatar *avatar = tracker->GetAvatar();
    if (avatar)
    {
      mafMatrix world_pose;
      mafAvatar3D *avatar3D = mafAvatar3D::SafeDownCast(avatar);
      if (avatar3D)
        avatar3D->TrackerToWorld(tracker_pose,world_pose,mafAvatar3D::CANONICAL_TO_WORLD_SCALE);
      else
        world_pose = tracker_pose;
      mafView *v = avatar->GetView();
      if (v)
      {
        if(v->Pick(world_pose))
        {
          picked_vme = v->GetPickedVme();
          picked_behavior = picked_vme->GetBehavior();
          return true;
        }
      }
    }
  }
  else if (mmdMouse *mouse = mmdMouse::SafeDownCast(device))
  { 
    int mouse_pos[2];
    mouse_pos[1] = (int)point_pose.GetElement(1,3);
    mouse_pos[0] = (int)point_pose.GetElement(0,3);
    mafView *v = mouse->GetView();
    if (v)
    {
      mafViewCompound *vc = mafViewCompound::SafeDownCast(v);
      if (vc)
      {
        v = vc->GetSubView(mouse->GetRWI());
      }
      if(v->Pick(mouse_pos[0], mouse_pos[1]))
      {
        picked_vme = v->GetPickedVme();
        picked_behavior = picked_vme->GetBehavior();
        return true;
      }
    }
  }
  return false;
}
