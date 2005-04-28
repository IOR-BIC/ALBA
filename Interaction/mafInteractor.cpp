/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafInteractor.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-28 16:10:12 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows
#ifdef __GNUG__
    #pragma implementation "mafInteractor.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "mafInteractor.h"
#include "mafInteractionDecl.h"
#include "vtkRenderer.h"
#include "mmdButtonsPad.h"
#include "mflEventInteraction.h"
#include "mflVME.h"
#include "vtkProp3D.h"
#include <assert.h>

#include "mflAssembly.h"
#include "mmdTracker.h"
#include "mafAvatar.h"
#include "mmdMouse.h"
#include "vtkAssemblyNode.h"
#include "vtkAssemblyPath.h"
#include "mafVmeData.h"
#include "vtkAbstractPropPicker.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MFL_EVT_IMP(mafInteractor::InteractionStartedEvent);
MFL_EVT_IMP(mafInteractor::InteractionStoppedEvent);
MFL_EVT_IMP(mafInteractor::ButtonDownEvent);
MFL_EVT_IMP(mafInteractor::ButtonUpEvent);

//------------------------------------------------------------------------------
vtkCxxSetObjectMacro(mafInteractor,Renderer,vtkRenderer);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafInteractor::mafInteractor()
//------------------------------------------------------------------------------
{
  Renderer            = NULL;
  Device              = NULL;
  VME                 = NULL;
  Prop                = NULL;
  
  LockDevice            = true;
  DeviceIsSet           = false;
  IgnoreTriggerEvents   = false;
  InteractionFlag       = false;
  StartInteractionEvent = mmdButtonsPad::ButtonDownEvent;
  StopInteractionEvent  = mmdButtonsPad::ButtonUpEvent;
  StartButton           = 0;// Button 0
  Modifiers             = 0;// no modifiers
  ButtonMode            = SINGLE_BUTTON_MODE;
  ButtonsCounter        = 0;

  CurrentButton         = 0;
  CurrentModifier       = 0;
  m_listener            = NULL;
}

//------------------------------------------------------------------------------
mafInteractor::~mafInteractor()
//------------------------------------------------------------------------------
{
  if (IsInteracting()&&Device&&LockDevice)
    Device->Unlock();
    
  vtkDEL(Renderer);
  Device = NULL;
  vtkDEL(Prop);
  vtkDEL(VME);
}

//------------------------------------------------------------------------------
void mafInteractor::SetDevice(mafDevice *device)
//------------------------------------------------------------------------------
{
  if (Device)
    UnPlugEventSource(Device);
  Device = device; // avoid cross reference counting
  if (Device)
    PlugEventSource(device,mafDevice::DeviceInputChannel);

  DeviceIsSet=(device!=NULL);
}

//------------------------------------------------------------------------------
void mafInteractor::SetVME(mflVME *vme)
//------------------------------------------------------------------------------
{
  vtkSetObjectBodyMacro(VME,mflVME,vme);
}

//------------------------------------------------------------------------------
void mafInteractor::SetProp(vtkProp3D *prop)
//------------------------------------------------------------------------------
{
  vtkSetObjectBodyMacro(Prop,vtkProp3D,prop);
}

//------------------------------------------------------------------------------
bool mafInteractor::IsInteracting() 
//------------------------------------------------------------------------------
{
  return InteractionFlag!=0;
}

//------------------------------------------------------------------------------
bool mafInteractor::IsInteracting(mafDevice *device)
//------------------------------------------------------------------------------
{
  return ((InteractionFlag!=0)&&(Device==device));
}

//------------------------------------------------------------------------------
int mafInteractor::StartInteraction(mafDevice *device)
//------------------------------------------------------------------------------
{
  // if not already interacting start the interaction
  if (!IsInteracting())
  {
    // if device already in use return
    if (device&&LockDevice&&device->IsLocked())
      return false;

    // if device != from device set from outside
    if (DeviceIsSet&&device!=Device)
      return false;

    InteractionFlag=true;// lock the interactor

    // Set the device being interacting
    if (device)         
    { 
      Device=device;// Beware there could be more then one attached as input, set which is interacting
      if (LockDevice)
        device->Lock();
    }
                       
    ButtonsCounter=1;

    return true;
  }
  else if (ButtonMode==MULTI_BUTTON_MODE)
  {
    // interaction already started! return true to allow multi-button interaction
    if (IsInteracting(device))
    {
      ButtonsCounter++;
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
    ButtonsCounter--;

    // if no more active button pressed, stop the interaction
    if (ButtonsCounter==0)
    {
      InteractionFlag=false;

      if (device)
      {
        if (LockDevice)
          device->Unlock();

        if (!DeviceIsSet) // if device set from outside, do not reset
          Device=NULL;
      }
      CurrentButton=-1;
      CurrentModifier=0;
    }
    
    return true;
  }
  else
  {
    vtkGenericWarningMacro("StopInteraction without a StartInteraction or stopped by wrong device.");
  }
  return false;
}

//------------------------------------------------------------------------------
int mafInteractor::OnStartInteraction(mflEventInteraction *e)
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
int mafInteractor::OnStopInteraction(mflEventInteraction *e)
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
void mafInteractor::OnButtonDown(mflEventInteraction *e)
//------------------------------------------------------------------------------
{ 
  mflSmartPointer<mflEventInteraction> event;
  event->DeepCopy(e);
  e->SetID(ButtonDownEvent);
  e->SetSender(this);
  ForwardEvent(e);
}
//------------------------------------------------------------------------------
void mafInteractor::OnButtonUp(mflEventInteraction *e)
//------------------------------------------------------------------------------
{
  mflSmartPointer<mflEventInteraction> event;
  event->DeepCopy(e);
  e->SetID(ButtonUpEvent);
  e->SetSender(this);
  
  ForwardEvent(e);
}
//------------------------------------------------------------------------------
void mafInteractor::ProcessEvent(mflEvent *event,mafID ch)
//------------------------------------------------------------------------------
{
  assert(event);
  
  mafID id=event->GetID();

  if (ch==mafDevice::DeviceInputChannel)
  {
    // Start the interaction if not disabled
    if (id==StartInteractionEvent && !IgnoreTriggerEvents)
    {
      mflEventInteraction *e=mflEventInteraction::SafeDownCast(event);
      assert(e);

      // check if the right button has been pressed
      if ((e->GetButton()==StartButton || (StartButton<0)) && ((e->GetModifiers()&Modifiers) == Modifiers ))
      {
        // the start button has been already pressed once
        if (IsInteracting((mafDevice *)e->GetSender()))
        {
          if (ButtonMode==MULTI_BUTTON_MODE && ButtonsCounter > 0)
          {
            OnStartInteraction(e);
          }
          return;
        }

        CurrentButton   = e->GetButton();
        CurrentModifier = e->GetModifiers();
        if (OnStartInteraction(e))
        {
          if (IsInteracting()) ForwardEvent(InteractionStartedEvent);
        }
      }
      else if (ButtonMode==MULTI_BUTTON_MODE && ButtonsCounter > 0)
      {
        // in case of MULTI BUTTON and the start button has already started 
        // the interaction we call anyway the OnStartInteraction function
        OnStartInteraction(e);
      }
    }
    
    // Stop the interaction if not disabled
    else if (id==StopInteractionEvent && !IgnoreTriggerEvents)
    {
      mflEventInteraction *e=mflEventInteraction::SafeDownCast(event);
      assert(e);

      // check if the right button has been released
      if (e->GetButton()==StartButton || (StartButton<0))
      {
        if (OnStopInteraction(e))
          if (!IsInteracting()) ForwardEvent(InteractionStoppedEvent);
      }
      else if (ButtonMode==MULTI_BUTTON_MODE && ButtonsCounter > 0)
      {
        // in case of MULTI BUTTON and the start button has already started 
        // the interaction we call anyway the OnStopInteraction function
        if (OnStopInteraction(e))
          if (!IsInteracting()) ForwardEvent(InteractionStoppedEvent);
      }

    }
  }
  else
  {
    Superclass::ProcessEvent(event,ch);
  }
}

//----------------------------------------------------------------------------
void mafInteractor::ComputeDisplayToWorld(double x, double y, double z, double worldPt[4])
//----------------------------------------------------------------------------
{
  if ( !Renderer ) 
    return;
  
  Renderer->SetDisplayPoint(x, y, z);
  Renderer->DisplayToWorld();
  Renderer->GetWorldPoint(worldPt);
  if (worldPt[3])
  {
    worldPt[0] /= worldPt[3];
    worldPt[1] /= worldPt[3];
    worldPt[2] /= worldPt[3];
    worldPt[3] = 1.0;
  }
}

//----------------------------------------------------------------------------
void mafInteractor::ComputeDisplayToWorld(double x, double y, double z, float worldPt[4])
//----------------------------------------------------------------------------
{
  if ( !Renderer ) 
    return;
  
  Renderer->SetDisplayPoint(x, y, z);
  Renderer->DisplayToWorld();
  Renderer->GetWorldPoint(worldPt);
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
  if ( !Renderer ) 
    return;
  
  Renderer->SetWorldPoint(x, y, z, 1.0);
  Renderer->WorldToDisplay();
  Renderer->GetDisplayPoint(displayPt);
}

// Description:
// transform from world to display coordinates.
// displayPt has to be allocated as 3 vector
//----------------------------------------------------------------------------
void mafInteractor::ComputeWorldToDisplay(double x, double y, double z, float displayPt[3])
//----------------------------------------------------------------------------
{
  if ( !Renderer ) 
    return;
  
  Renderer->SetWorldPoint(x, y, z, 1.0);
  Renderer->WorldToDisplay();
  Renderer->GetDisplayPoint(displayPt);
}

//----------------------------------------------------------------------------
bool mafInteractor::FindPokedVme(mafDevice *device,mflMatrix *point_pose,vtkProp3D *&picked_prop,mflVME *&picked_vme,mafInteractor *&picked_behavior)
//----------------------------------------------------------------------------
{
  bool                foundVme = false;
  mflVME              *vme = NULL;
  mflAssembly         *as = NULL;
  vtkAssemblyPath     *ap = NULL;
  mafInteractor       *bh = NULL;

  if (mmdTracker *tracker=mmdTracker::SafeDownCast(device))
  { // is it a tracker?
    
    mflMatrix *tracker_pose = point_pose;

    // extract device avatar's renderer, no avatar == no picking
    mafAvatar *avatar = tracker->GetAvatar();
    if (avatar)
    {
      int flag=avatar->Pick(tracker_pose);
      if (flag)
      {
        // for debugging purposes
        //avatar->ShowPickLine();
        //avatar->Hide();

        // find picked VME
        vtkAbstractPropPicker *picker = avatar->GetPicker();        
        ap = picker->GetPath(); // extract the assembly path
      }
    }
  }
  else if (mmdMouse *mouse=mmdMouse::SafeDownCast(device))
  { 
    // Pick with mouse
    int mouse_pos[2];
    mouse_pos[0] = (int)point_pose->GetElement(0,3);
    mouse_pos[1] = (int)point_pose->GetElement(1,3);
    ap = mouse->Pick(mouse_pos);
  }

  if(ap)
  {
    //scan the path from the leaf finding an assembly
    //which know the related vme.
    int pathlen = ap->GetNumberOfItems();
    for (int i=pathlen-1; i>=0; i--)
    {
      vtkAssemblyNode *an = (vtkAssemblyNode*)ap->GetItemAsObject(i);
      if (an)
      {
        vtkProp *p = an->GetProp();
        if(p && p->IsA("mflAssembly"))
        {
          as  = (mflAssembly*)p;
          vme  = as->GetVme();
          foundVme = true;

          if(vme && vme->GetClientData() )
          {
            mafVmeData *vd = (mafVmeData*) vme->GetClientData();
            bh = vd->m_behavior; //can be NULL
          }
          break;
        }
      }
    }

    if(foundVme)
    {
      picked_prop     = as;
      picked_behavior = bh;
      picked_vme      = vme;
    }
    else
    {
      picked_prop     = NULL;
      picked_behavior = NULL;
      picked_vme      = NULL;
    }
  }

  return foundVme;
}

//------------------------------------------------------------------------------
void mafInteractor::ForwardEvent(int id, mafID channel,vtkObjectBase *data)
//------------------------------------------------------------------------------
{
  ForwardEvent(mflSmartEvent(id,this,data),channel);
}

//------------------------------------------------------------------------------
void mafInteractor::ForwardEvent(mflEvent *event, mafID channel)
//------------------------------------------------------------------------------
{
  Superclass::ForwardEvent(event,channel);

  if (channel==mflAgent::DefaultChannel)
  {    
    // send event through m_listenter too...
    mafEventMacro(mafEvent(this,MFL_EVENT_ID,event));
  }
}