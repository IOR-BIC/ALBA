/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorPER
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


#include "albaInteractorPER.h"

#include "albaView.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaAvatar3D.h"
#include "albaInteractor6DOFCameraMove.h"
#include "albaInteractorCameraMove.h"

#include "albaEventBase.h"
#include "albaEventInteraction.h"

#include "albaVME.h"
#include "albaVMEGizmo.h"
#include "albaTransform.h"

#include "vtkALBAAssembly.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkMath.h"
#include <map>
#include <assert.h>
#include "albaVMELandmarkCloud.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractorPER)
//------------------------------------------------------------------------------
albaInteractorPER::albaInteractorPER()
//------------------------------------------------------------------------------
{
  m_DraggingLeft    = false; 
  m_CanSelect       = true; 
  m_FlyToFlag       = false;
  m_StartButton     = -1;

  m_CameraBehavior  = albaInteractor6DOFCameraMove::New(); // allocate camera behavior
  m_CameraBehavior->SetStartButton(ALBA_LEFT_BUTTON); // any button
  SetModeToSingleButton();
  m_FirstTime = 0;

  m_CameraMouseBehavior = albaInteractorCameraMove::New(); // allocate mouse camera behavior
  m_CameraMouseBehavior->SetStartButton(-1);
}

//------------------------------------------------------------------------------
albaInteractorPER::~albaInteractorPER()
//------------------------------------------------------------------------------
{
  vtkDEL(m_CameraBehavior);
  vtkDEL(m_CameraMouseBehavior);
}

//----------------------------------------------------------------------------
bool albaInteractorPER::IsInteracting(albaDevice *device, int button)
//----------------------------------------------------------------------------
{
  std::map<albaID,DeviceItem>::iterator iter = m_Devices.find(device->GetID());
  if (iter!=m_Devices.end()&&(*iter).first==device->GetID())
  {
    if (m_Mode==SINGLE_BUTTON)
    {
      // if optional button parameter is positive report if interaction
      // has been started by a specific button.
      return (button<0)?true:(*iter).second.m_Button==button;
    }
    else
    {
      // simply check if one or more button has been pressed.
      return (*iter).second.m_Button>0;
    }
  }
  return false;
}

//----------------------------------------------------------------------------
void albaInteractorPER::InsertDevice(albaDevice *device,int button)
//----------------------------------------------------------------------------
{
  if (m_Mode == SINGLE_BUTTON)
  {
    m_Devices[device->GetID()].m_Button = button;
    m_Devices[device->GetID()].m_VME = NULL; // initially set picked VME to NULL
  }
  else
  {
    std::map<albaID,DeviceItem>::iterator iter = m_Devices.find(device->GetID());
    if (iter == m_Devices.end())
    {
//      if ((*iter).first==device->GetID())
      m_Devices[device->GetID()].m_VME = NULL; // initially set picked VME to NULL
      m_Devices[device->GetID()].m_Button = 1;

    }
    else
    {
      //int prebuttonDevice = m_Devices[device->GetID()].Button;
      m_Devices[device->GetID()].m_Button++;
      //int postbuttonDevice = m_Devices[device->GetID()].Button;//only for debug
    }
  }
}

//----------------------------------------------------------------------------
void albaInteractorPER::RemoveDevice(albaDevice *device)
//----------------------------------------------------------------------------
{
  std::map<albaID,DeviceItem>::iterator iter = m_Devices.find(device->GetID());

  if (iter == m_Devices.end() || (*iter).first != device->GetID())
  {
    return;
  }
  
  m_Devices[device->GetID()].m_VME = NULL; // initially set picked VME to NULL
  
  if(m_Mode == SINGLE_BUTTON)
  {
    m_Devices.erase(iter);
  }
  else
  {
    // decrease counter, if counter gets to zero delete the item
    int cnt = (--m_Devices[device->GetID()].m_Button);
    if (cnt <= 0)
    {
      m_Devices.erase(iter);
    }
  }
}

//----------------------------------------------------------------------------
void albaInteractorPER::SetPickedVME(albaDevice *device,albaVME *vme)
//----------------------------------------------------------------------------
{
  if (device)
  {
    std::map<albaID,DeviceItem>::iterator iter = m_Devices.find(device->GetID());
    //assert(iter != m_Devices.end()); // check the device has started an interaction
    if (iter != m_Devices.end())
      (*iter).second.m_VME = vme;
  }
  else
  {
    SetVME(vme);
  }
}

//----------------------------------------------------------------------------
albaVME *albaInteractorPER::GetPickedVME(albaDevice *device)
//----------------------------------------------------------------------------
{
  if (device)
  {
    std::map<albaID,DeviceItem>::iterator iter = m_Devices.find(device->GetID());
    //assert(iter != m_Devices.end()); // check the device has started an interaction
    return (iter != m_Devices.end()) ? (*iter).second.m_VME : NULL;
  }
  else
  {
    return m_VME;
  }
}

//----------------------------------------------------------------------------
void albaInteractorPER::OnChar(albaEventInteraction *e) 
//----------------------------------------------------------------------------
{
  switch (e->GetKey()) 
  {
    case 'r' :      
    case 'R' :
    {
      albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::SafeDownCast((albaDevice *)e->GetSender());
      if (mouse)
      {
        albaView *view = mouse->GetView();
        InvokeEvent(this,CAMERA_FIT,MCH_UP,view);
      }
    }
    break; 
    case 'f' :      
    case 'F' :
			FlyTo(e,20,1.4);
    break;
    case 'g' :      
    case 'G' :
			FlyTo(e,20,-0.6);
    break;
    default:
        //modified by Marco. 30-6-2004
        // TODO: what to do here???
		    // vtkInteractorStyle::OnChar();
    break;
	}
}

//----------------------------------------------------------------------------
void albaInteractorPER::OnLeftButtonDown(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
  if(m_FlyToFlag)
    FlyTo(e); 

  OnButtonDown(e);

	if (!e->GetModifier(ALBA_SHIFT_KEY))
		m_DraggingLeft = true;
}

//----------------------------------------------------------------------------
void albaInteractorPER::OnMiddleButtonDown(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnButtonDown(e);
}
//----------------------------------------------------------------------------
void albaInteractorPER::OnRightButtonDown(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
	if(m_FlyToFlag)
     FlyTo(e,20,-0.6); 

  OnButtonDown(e);

	m_ShowContextMenu = true;
}

//----------------------------------------------------------------------------
void albaInteractorPER::OnLeftButtonUp(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
  //if(m_FlyToFlag)
  //  m_FlyToFlag = false; //SIL. 2-7-2004: - moved in FlyTo because if activated throught the voice label
                        //we receive a Mouseup right after 
  OnButtonUp(e);
}
//----------------------------------------------------------------------------
void albaInteractorPER::OnMiddleButtonUp(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnButtonUp(e);
}
//----------------------------------------------------------------------------
void albaInteractorPER::OnRightButtonUp(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
	//if(m_FlyToFlag)
  //  m_FlyToFlag = false; //SIL. 2-7-2004: - moved in FlyTo because if activated throught the voice label
                        //we receive a Mouseup right after

  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::SafeDownCast((albaDevice*)e->GetSender());

	if(m_ShowContextMenu && mouse)
	{
    albaVME *vme = GetPickedVME(mouse); 
    InvokeEvent(this, SHOW_VIEW_CONTEXTUAL_MENU,MCH_UP,vme);
	}

  OnButtonUp(e);
}

//----------------------------------------------------------------------------
void albaInteractorPER::OnMove(albaEventInteraction *e) 
//----------------------------------------------------------------------------
{
	m_ShowContextMenu = false;
}

//----------------------------------------------------------------------------
void albaInteractorPER::OnButtonDown(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
  albaDevice *device = albaDevice::SafeDownCast((albaDevice*)e->GetSender());

  albaView       *v = NULL;
  albaVME        *picked_vme  = NULL;
  vtkProp3D     *picked_prop = NULL;
  albaInteractor *picked_bh   = NULL;
  albaMatrix     point_pose;
  double        pos_2d[2];

  albaDeviceButtonsPadMouse   *mouse   = albaDeviceButtonsPadMouse::SafeDownCast(device);
  albaDeviceButtonsPadTracker *tracker = albaDeviceButtonsPadTracker::SafeDownCast(device);

  if (tracker)
  {
    point_pose = *e->GetMatrix();
    albaAvatar *avatar = tracker->GetAvatar();
    if (avatar)
    {
      v = avatar->GetView();
    }
  }
  else if (mouse)
  {
    e->Get2DPosition(pos_2d);
    point_pose.Identity();
    point_pose.SetElement(0,3,pos_2d[0]);
    point_pose.SetElement(1,3,pos_2d[1]);
    v = mouse->GetView();
  }

  if(v && v->FindPokedVme(device, point_pose, picked_prop, picked_vme, picked_bh))
  {
    SetPickedVME(device, picked_vme);
    // if a VME is picked its pointer is written in PickedVME
    int but_down_id = e->GetId();
    if (but_down_id == albaDeviceButtonsPadMouse::GetMouseDClickId() && !picked_vme->IsA("albaVMEGizmo"))
    {
      // Send event to inform Logic that a double click event is rised on a VME
      InvokeEvent(this,VME_DCLICKED,MCH_UP,picked_vme);
    }
    else if(m_CanSelect && !picked_vme->IsA("albaVMEGizmo"))
    {
			if (picked_vme->IsA("albaVMELandmarkCloud"))
			{
				double pos[3];
				v->GetPickedPosition(pos);
				picked_vme = (albaVME *)((albaVMELandmarkCloud *)picked_vme)->GetNearestLandmark(pos, true);

			}
      // Send a VME select event to Logic
      InvokeEvent(this,VME_SELECT,MCH_UP,picked_vme);
    }
  }
  // Forward the start event to the right behavior
  if(picked_bh)
  {
    // if a vme with a behavior has been picked... 
    picked_bh->SetVME(picked_vme);   // set the VME (Marco: to be removed, the operation should set the VME to the interactor!) 
    picked_bh->SetProp(picked_prop); // set the prop (Marco: to be removed, no access to the vtkProp!!!) 
    picked_bh->OnEvent(e); // forward the start event to picked behavior
  }
  // if I don't picked a VME or I picked but I cannot select, move the camera.
  else if (tracker && m_CameraBehavior&&(!m_CameraBehavior->IsInteracting()))
  {
    // if the camera behavior is free...
    m_CameraBehavior->OnEvent(e); // forward to the camera behavior
  }
  // if I don't picked a VME or I picked but I cannot select, move the camera.
  else if (mouse && m_CameraMouseBehavior&&(!m_CameraMouseBehavior->IsInteracting()))
  {
    // if the camera behavior is free...
    m_CameraMouseBehavior->OnEvent(e); // forward to the camera behavior
  }
}

//----------------------------------------------------------------------------
void albaInteractorPER::OnButtonUp(albaEventInteraction *e) 
//----------------------------------------------------------------------------
{
  m_DraggingLeft = false;
}

//------------------------------------------------------------------------------
int albaInteractorPER::StartInteraction(albaDevice *device, int button)
//------------------------------------------------------------------------------
{
  // Check if already interacting with the same device: it could happen the user
  // contemporary pressed another button...
  if (m_Mode == SINGLE_BUTTON)
  {
    if (!IsInteracting(device,button))
    {
      InsertDevice(device,button); // add the device to the list of interacting devices
      //device->Lock(); // it's the specific behavior which is locking the device!!!
      return true;
    }
    return false;
  }
  else
  {
    InsertDevice(device,button);
    return true;
  }
}

//------------------------------------------------------------------------------
int albaInteractorPER::StopInteraction(albaDevice *device, int button)
//------------------------------------------------------------------------------
{
  if (m_Mode == SINGLE_BUTTON)
  {
    if (IsInteracting(device, button))
    {   
      RemoveDevice(device); // remove from the list of interacting devices
      //device->Unlock(); // it's the specific behavior which is locking the device!!!
      return true;
    }
    return false;
  }
  else
  {
    RemoveDevice(device);
    return true;
  }

  albaWarningMacro("StopInteraction without a Start for device "<< device->GetName());
  return false;
}

//------------------------------------------------------------------------------
int albaInteractorPER::OnStartInteraction(albaEventInteraction *e)
//------------------------------------------------------------------------------
{
  albaDevice *device = albaDevice::SafeDownCast((albaDevice *)e->GetSender());
  
  assert(device);
  int button = e->GetButton();
  if (StartInteraction(device, button))
  {
    // standard action keys
    switch (button)
    {
		case ALBA_LEFT_BUTTON:
			OnLeftButtonDown(e);
			break;
		case ALBA_MIDDLE_BUTTON:
			OnMiddleButtonDown(e);
			break;
		case ALBA_RIGHT_BUTTON:
			OnRightButtonDown(e);
			break;
		default:
			StopInteraction(device, button); // stop immediately and return false
			return 0;
    }
    return 1;
  }
  return 0;
}
//------------------------------------------------------------------------------
int albaInteractorPER::OnStopInteraction(albaEventInteraction *e)
//------------------------------------------------------------------------------
{
  albaDevice *device = albaDevice::SafeDownCast((albaDevice *)e->GetSender());
  assert(device);

  int button = e->GetButton();
  if (m_Mode == MULTI_BUTTON || IsInteracting(device, button))
  {
    // standard action keys
    switch (button)
    {
		case ALBA_LEFT_BUTTON:
			OnLeftButtonUp(e);
			break;
		case ALBA_MIDDLE_BUTTON:
			OnMiddleButtonUp(e);
			break;
		case ALBA_RIGHT_BUTTON:
			OnRightButtonUp(e);
			break;
		default:
			return -1;
    }
    StopInteraction(device, button);
    return 1;
  }
  return 0;
}

//------------------------------------------------------------------------------
void albaInteractorPER::OnEvent(albaEventBase *event)
//------------------------------------------------------------------------------
{
  albaID ch = event->GetChannel();
  
  if (ch == MCH_INPUT)
  {
    albaID id = event->GetId();
    albaDevice *device = (albaDevice *)event->GetSender();
    assert(device);

		if (m_CameraBehavior && id == albaDeviceButtonsPadMouse::GetWheelId() && !IsInteracting(device))
		{
			m_ShowContextMenu = false;
			m_CameraMouseBehavior->OnEvent(event);
			return;
		}

    if (id == albaDeviceButtonsPadMouse::GetMouseCharEventId() && !IsInteracting(device))
    {
      albaEventInteraction *e = albaEventInteraction::SafeDownCast(event);
      OnChar(e);
    }
    // find if this device is one of those currently interacting
    if (IsInteracting(device))
    {
      // process the Move event
      if (id == albaDeviceButtonsPadTracker::GetTracker3DMoveId() || id == albaDeviceButtonsPadMouse::GetMouse2DMoveId())
      {
        albaEventInteraction *e = albaEventInteraction::SafeDownCast(event);
        OnMove(e);
      }
      // In any case, forward the event to the right behavior
      albaVME *vme = GetPickedVME(device);
      if(vme)
      {
        albaInteractor *bh = vme->GetBehavior(); //can be NULL
        if (bh)
        {
          bh->OnEvent(event); // forward to VME behavior
        }
      }
      // if no picked behavior...
      if (m_CameraBehavior && m_CameraBehavior->IsInteracting(device))
      {
        m_CameraBehavior->OnEvent(event); // forward to Camera behavior
      }
      else if (m_CameraMouseBehavior && m_CameraMouseBehavior->IsInteracting(device))
      {
        m_CameraMouseBehavior->OnEvent(event); // forward to Camera behavior
      }
    }
  }
  // Make the superclass to manage StartInteractionEvent
  // and StopInteractionEvent: this will make OnStart/StopInteraction()
  // to be called, or eventually the event to be forwarded.
  Superclass::OnEvent(event);
}

//----------------------------------------------------------------------------
void albaInteractorPER::FlyTo(albaEventInteraction *e,int numstep, double zoom)
//----------------------------------------------------------------------------
{
  albaVME        *picked_vme  = NULL;
  vtkProp3D     *picked_prop = NULL;
  albaInteractor *picked_bh   = NULL;
  albaMatrix     *point_pose  = e->GetMatrix();
  double         pose_2d[2];

  albaDeviceButtonsPadTracker *tracker = albaDeviceButtonsPadTracker::SafeDownCast((albaDevice*)e->GetSender());
  albaDeviceButtonsPadMouse   *mouse   = albaDeviceButtonsPadMouse::SafeDownCast((albaDevice*)e->GetSender());
  assert(tracker || mouse);

  if (point_pose && tracker && tracker->GetAvatar())
  { 
    vtkRenderer *ren = tracker->GetAvatar()->GetRenderer();
    albaView *view = tracker->GetAvatar()->GetView();
    
    if(!ren) return; // no renderer no fly to!

	  vtkCamera *cam = ren->GetActiveCamera();

    if(view && !view->FindPokedVme(tracker,*point_pose,picked_prop,picked_vme,picked_bh)) 
      return; // no picking no fly to!


    int i, j;
	  double f0[3]; // start focal point
	  cam->GetFocalPoint(f0);
	  double p0[3]; // start camera position
	  cam->GetPosition(p0);
    
    // find picked position
    double pos[3];
    view->GetPickedPosition(pos);
	  double foc_motion[3];
    for (i=0; i<3; i++) 
      foc_motion[i] = pos[i] - f0[i];
    double foc_step = vtkMath::Normalize(foc_motion)/numstep;

    if(zoom >  1) zoom = 1;
    if(zoom < -1) zoom =-1;
	  double cam_motion[3];
    for (i=0; i<3; i++) 
      cam_motion[i] = (pos[i]-f0[i]) + (f0[i]-p0[i])*zoom/2;
    double cam_step = vtkMath::Normalize(cam_motion)/numstep;
    double f1[3]; // new focal point
	  double p1[3]; // new camera position

	  for (i=0; i<=numstep; i++)
    {
      for (j=0; j<3; j++)
      {
        f1[j] = f0[j] + foc_motion[j]*i*foc_step;
        p1[j] = p0[j] + cam_motion[j]*i*cam_step;
		  }
		  cam->SetFocalPoint(f1);
      cam->SetPosition(p1);
      ren->ResetCameraClippingRange();
      // render at each cycle
      InvokeEvent(this,CAMERA_UPDATE,MCH_UP,view);

    }
    cam->OrthogonalizeViewUp();

    m_FlyToFlag = false;
  }
  else if (mouse)
  {
    mouse->GetLastPosition(pose_2d);
    albaMatrix point_pose;
    point_pose.SetElement(0,3,pose_2d[0]);
    point_pose.SetElement(1,3,pose_2d[1]);

    vtkRenderer *ren = mouse->GetRenderer();
    albaView *view = mouse->GetView();
    
    if(!ren) return; // no renderer no fly to!

	  vtkCamera *cam = ren->GetActiveCamera();

    if(view && !view->FindPokedVme(mouse,point_pose,picked_prop,picked_vme,picked_bh)) 
      return; // no picking no fly to!

    int i, j;
	  double f0[3]; // start focal point
	  cam->GetFocalPoint(f0);
	  double p0[3]; // start camera position
	  cam->GetPosition(p0);
    
    // find picked position
    double pos[3];
    view->GetPickedPosition(pos);
	  double foc_motion[3];
    for (i=0; i<3; i++) 
      foc_motion[i] = pos[i] - f0[i];
    double foc_step = vtkMath::Normalize(foc_motion)/numstep;

    if(zoom >  1) zoom = 1;
    if(zoom < -1) zoom =-1;
	  double cam_motion[3];
    for (i=0; i<3; i++) 
      cam_motion[i] = (pos[i]-f0[i]) + (f0[i]-p0[i])*zoom/2;
    double cam_step = vtkMath::Normalize(cam_motion)/numstep;
    double f1[3]; // new focal point
	  double p1[3]; // new camera position

	  for (i=0; i<=numstep; i++)
    {
      for (j=0; j<3; j++)
      {
        f1[j] = f0[j] + foc_motion[j]*i*foc_step;
        p1[j] = p0[j] + cam_motion[j]*i*cam_step;
      }
      cam->SetFocalPoint(f1);
      cam->SetPosition(p1);
      ren->ResetCameraClippingRange();
      // render at each cycle
      InvokeEvent(this,CAMERA_UPDATE,MCH_UP,view);
    }
    cam->OrthogonalizeViewUp();
    m_FlyToFlag = false;
  }
}
