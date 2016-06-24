/*=========================================================================

 Program: MAF2
 Module: mafInteractorPER
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafInteractorPER.h"

#include "mafView.h"
#include "mafDeviceButtonsPadTracker.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafAvatar3D.h"
#include "mafInteractor6DOFCameraMove.h"
#include "mafInteractorCameraMove.h"

#include "mafEventBase.h"
#include "mafEventInteraction.h"

#include "mafVME.h"
#include "mafVMEGizmo.h"
#include "mafTransform.h"

#include "vtkMAFAssembly.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkMath.h"
#include <map>
#include <assert.h>
#include "mafVMELandmarkCloud.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafInteractorPER)
//------------------------------------------------------------------------------
mafInteractorPER::mafInteractorPER()
//------------------------------------------------------------------------------
{
  m_DraggingLeft    = false; 
  m_CanSelect       = true; 
  m_FlyToFlag       = false;
  m_StartButton     = -1;

  m_CameraBehavior  = mafInteractor6DOFCameraMove::New(); // allocate camera behavior
  m_CameraBehavior->SetStartButton(MAF_LEFT_BUTTON); // any button
  SetModeToSingleButton();
  m_FirstTime = 0;

  m_CameraMouseBehavior = mafInteractorCameraMove::New(); // allocate mouse camera behavior
  m_CameraMouseBehavior->SetStartButton(-1);
}

//------------------------------------------------------------------------------
mafInteractorPER::~mafInteractorPER()
//------------------------------------------------------------------------------
{
  vtkDEL(m_CameraBehavior);
  vtkDEL(m_CameraMouseBehavior);
}

//----------------------------------------------------------------------------
bool mafInteractorPER::IsInteracting(mafDevice *device, int button)
//----------------------------------------------------------------------------
{
  std::map<mafID,DeviceItem>::iterator iter = m_Devices.find(device->GetID());
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
void mafInteractorPER::InsertDevice(mafDevice *device,int button)
//----------------------------------------------------------------------------
{
  if (m_Mode == SINGLE_BUTTON)
  {
    m_Devices[device->GetID()].m_Button = button;
    m_Devices[device->GetID()].m_VME = NULL; // initially set picked VME to NULL
  }
  else
  {
    std::map<mafID,DeviceItem>::iterator iter = m_Devices.find(device->GetID());
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
void mafInteractorPER::RemoveDevice(mafDevice *device)
//----------------------------------------------------------------------------
{
  std::map<mafID,DeviceItem>::iterator iter = m_Devices.find(device->GetID());

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
void mafInteractorPER::SetPickedVME(mafDevice *device,mafVME *vme)
//----------------------------------------------------------------------------
{
  if (device)
  {
    std::map<mafID,DeviceItem>::iterator iter = m_Devices.find(device->GetID());
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
mafVME *mafInteractorPER::GetPickedVME(mafDevice *device)
//----------------------------------------------------------------------------
{
  if (device)
  {
    std::map<mafID,DeviceItem>::iterator iter = m_Devices.find(device->GetID());
    //assert(iter != m_Devices.end()); // check the device has started an interaction
    return (iter != m_Devices.end()) ? (*iter).second.m_VME : NULL;
  }
  else
  {
    return m_VME;
  }
}

//----------------------------------------------------------------------------
void mafInteractorPER::OnChar(mafEventInteraction *e) 
//----------------------------------------------------------------------------
{
  switch (e->GetKey()) 
  {
    case 'r' :      
    case 'R' :
    {
      mafDeviceButtonsPadMouse *mouse = mafDeviceButtonsPadMouse::SafeDownCast((mafDevice *)e->GetSender());
      if (mouse)
      {
        mafView *view = mouse->GetView();
        InvokeEvent(CAMERA_FIT,MCH_UP,view);
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
void mafInteractorPER::OnLeftButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  if(m_FlyToFlag)
    FlyTo(e); 

  OnButtonDown(e);
  m_DraggingLeft = true;
}

//----------------------------------------------------------------------------
void mafInteractorPER::OnMiddleButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnButtonDown(e);
}
//----------------------------------------------------------------------------
void mafInteractorPER::OnRightButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
	if(m_FlyToFlag)
     FlyTo(e,20,-0.6); 

  OnButtonDown(e);

	m_ShowContextMenu = true;
}

//----------------------------------------------------------------------------
void mafInteractorPER::OnLeftButtonUp(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  //if(m_FlyToFlag)
  //  m_FlyToFlag = false; //SIL. 2-7-2004: - moved in FlyTo because if activated throught the voice label
                        //we receive a Mouseup right after 
  OnButtonUp(e);
}
//----------------------------------------------------------------------------
void mafInteractorPER::OnMiddleButtonUp(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnButtonUp(e);
}
//----------------------------------------------------------------------------
void mafInteractorPER::OnRightButtonUp(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
	//if(m_FlyToFlag)
  //  m_FlyToFlag = false; //SIL. 2-7-2004: - moved in FlyTo because if activated throught the voice label
                        //we receive a Mouseup right after

  mafDeviceButtonsPadMouse *mouse = mafDeviceButtonsPadMouse::SafeDownCast((mafDevice*)e->GetSender());

	if(m_ShowContextMenu && mouse)
	{
    mafVME *vme = GetPickedVME(mouse);
    InvokeEvent(SHOW_CONTEXTUAL_MENU,MCH_UP,vme);
	}

  OnButtonUp(e);
}
//----------------------------------------------------------------------------
void mafInteractorPER::OnMove(mafEventInteraction *e) 
//----------------------------------------------------------------------------
{
	m_ShowContextMenu = false;
}

//----------------------------------------------------------------------------
void mafInteractorPER::OnButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  mafDevice *device = mafDevice::SafeDownCast((mafDevice*)e->GetSender());

  mafView       *v = NULL;
  mafVME        *picked_vme  = NULL;
  vtkProp3D     *picked_prop = NULL;
  mafInteractor *picked_bh   = NULL;
  mafMatrix     point_pose;
  double        pos_2d[2];

  mafDeviceButtonsPadMouse   *mouse   = mafDeviceButtonsPadMouse::SafeDownCast(device);
  mafDeviceButtonsPadTracker *tracker = mafDeviceButtonsPadTracker::SafeDownCast(device);

  if (tracker)
  {
    point_pose = *e->GetMatrix();
    mafAvatar *avatar = tracker->GetAvatar();
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
    if (but_down_id == mafDeviceButtonsPadMouse::GetMouseDClickId() && !picked_vme->IsA("mafVMEGizmo"))
    {
      // Send event to inform Logic that a double click event is rised on a VME
      InvokeEvent(VME_DCLICKED,MCH_UP,picked_vme);
    }
    else if(m_CanSelect && !picked_vme->IsA("mafVMEGizmo"))
    {
			if (picked_vme->IsA("mafVMELandmarkCloud"))
			{
				double pos[3];
				v->GetPickedPosition(pos);
				picked_vme = (mafVME *)((mafVMELandmarkCloud *)picked_vme)->GetNearestLandmark(pos);

			}
      // Send a VME select event to Logic
      InvokeEvent(VME_SELECT,MCH_UP,picked_vme);
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
void mafInteractorPER::OnButtonUp(mafEventInteraction *e) 
//----------------------------------------------------------------------------
{
  m_DraggingLeft = false;
}

//------------------------------------------------------------------------------
int mafInteractorPER::StartInteraction(mafDevice *device, int button)
//------------------------------------------------------------------------------
{
  if (device&&device->IsLocked())
    return false;

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
int mafInteractorPER::StopInteraction(mafDevice *device, int button)
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

  mafWarningMacro("StopInteraction without a Start for device "<< device->GetName());
  return false;
}

//------------------------------------------------------------------------------
int mafInteractorPER::OnStartInteraction(mafEventInteraction *e)
//------------------------------------------------------------------------------
{
  mafDevice *device = mafDevice::SafeDownCast((mafDevice *)e->GetSender());
  
  assert(device);
  int button = e->GetButton();
  if (StartInteraction(device, button))
  {
    // standard action keys
    switch (button)
    {
      case MAF_LEFT_BUTTON:
        OnLeftButtonDown(e);
      break;
      case MAF_MIDDLE_BUTTON:
        OnMiddleButtonDown(e);
      break;
      case MAF_RIGHT_BUTTON:
        OnRightButtonDown(e);
      break;
      default:
        StopInteraction(device,button); // stop immediately and return false
        return 0;
    }
    return 1;
  }
  return 0;
}
//------------------------------------------------------------------------------
int mafInteractorPER::OnStopInteraction(mafEventInteraction *e)
//------------------------------------------------------------------------------
{
  mafDevice *device = mafDevice::SafeDownCast((mafDevice *)e->GetSender());
  assert(device);

  int button = e->GetButton();
  if (m_Mode == MULTI_BUTTON || IsInteracting(device, button))
  {
    // standard action keys
    switch (button)
    {
      case MAF_LEFT_BUTTON:
        OnLeftButtonUp(e);
      break;
      case MAF_MIDDLE_BUTTON:
        OnMiddleButtonUp(e);
      break;
      case MAF_RIGHT_BUTTON:
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
void mafInteractorPER::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  mafID ch = event->GetChannel();
  
  if (ch == MCH_INPUT)
  {
    mafID id = event->GetId();
    mafDevice *device = (mafDevice *)event->GetSender();
    assert(device);

    if (id == mafDeviceButtonsPadMouse::GetMouseCharEventId() && !IsInteracting(device))
    {
      mafEventInteraction *e = mafEventInteraction::SafeDownCast(event);
      OnChar(e);
    }
    // find if this device is one of those currently interacting
    if (IsInteracting(device))
    {
      // process the Move event
      if (id == mafDeviceButtonsPadTracker::GetTracker3DMoveId() || id == mafDeviceButtonsPadMouse::GetMouse2DMoveId())
      {
        mafEventInteraction *e = mafEventInteraction::SafeDownCast(event);
        OnMove(e);
      }
      // In any case, forward the event to the right behavior
      mafVME *vme = GetPickedVME(device);
      if(vme)
      {
        mafInteractor *bh = vme->GetBehavior(); //can be NULL
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
void mafInteractorPER::FlyTo(mafEventInteraction *e,int numstep, double zoom)
//----------------------------------------------------------------------------
{
  mafVME        *picked_vme  = NULL;
  vtkProp3D     *picked_prop = NULL;
  mafInteractor *picked_bh   = NULL;
  mafMatrix     *point_pose  = e->GetMatrix();
  double         pose_2d[2];

  mafDeviceButtonsPadTracker *tracker = mafDeviceButtonsPadTracker::SafeDownCast((mafDevice*)e->GetSender());
  mafDeviceButtonsPadMouse   *mouse   = mafDeviceButtonsPadMouse::SafeDownCast((mafDevice*)e->GetSender());
  assert(tracker || mouse);

  if (point_pose && tracker && tracker->GetAvatar())
  { 
    vtkRenderer *ren = tracker->GetAvatar()->GetRenderer();
    mafView *view = tracker->GetAvatar()->GetView();
    
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
      InvokeEvent(CAMERA_UPDATE,MCH_UP,view);

    }
    cam->OrthogonalizeViewUp();

    m_FlyToFlag = false;
  }
  else if (mouse)
  {
    mouse->GetLastPosition(pose_2d);
    mafMatrix point_pose;
    point_pose.SetElement(0,3,pose_2d[0]);
    point_pose.SetElement(1,3,pose_2d[1]);

    vtkRenderer *ren = mouse->GetRenderer();
    mafView *view = mouse->GetView();
    
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
      InvokeEvent(CAMERA_UPDATE,MCH_UP,view);
    }
    cam->OrthogonalizeViewUp();
    m_FlyToFlag = false;
  }
}
//----------------------------------------------------------------------------
void mafInteractorPER::LinkCameraAdd(vtkCamera *cam)
//----------------------------------------------------------------------------
{
  ((mafInteractorCameraMove *)m_CameraMouseBehavior)->AddLinkedCamera(cam);
}
//----------------------------------------------------------------------------
void mafInteractorPER::LinkCameraRemove(vtkCamera *cam)
//----------------------------------------------------------------------------
{
  ((mafInteractorCameraMove *)m_CameraMouseBehavior)->RemoveLinkedCamera(cam);
}
//----------------------------------------------------------------------------
void mafInteractorPER::LinkCameraRemoveAll()
//----------------------------------------------------------------------------
{
  ((mafInteractorCameraMove *)m_CameraMouseBehavior)->RemoveAllLinkedCamera();
}
