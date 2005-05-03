/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiPER.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-03 05:58:12 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone 
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mmiPER.h"

#include "mmdTracker.h"
#include "mmdMouse.h"
#include "mafAvatar3D.h"
#include "mmi6DOFCameraMove.h"
#include "mmiCameraMove.h"

#include "mafEventBase.h"
#include "mafEventInteraction.h"

#include "mafVME.h"
#include "mafTransform.h"

#include "vtkMAFAssembly.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkAbstractPropPicker.h"
#include "vtkAssemblyPath.h"
#include "vtkAssemblyNode.h"
#include "vtkMath.h"
#include <map>
#include <assert.h>

//------------------------------------------------------------------------------
mafCxxTypeMacro(mmiPER)
//------------------------------------------------------------------------------
mmiPER::mmiPER()
//------------------------------------------------------------------------------
{
  m_DraggingLeft    = false; 
  m_CanSelect       = true; 
  m_FlyToFlag       = false;
  m_StartButton     = -1;

  m_CameraBehavior  = mmi6DOFCameraMove::New(); // allocate camera behavior
  m_CameraBehavior->SetStartButton(MAF_LEFT_BUTTON); // any button
  SetModeToSingleButton();
  m_FirstTime = 0;

  m_CameraMouseBehavior = mmiCameraMove::New(); // allocate mouse camera behavior
  m_CameraMouseBehavior->SetStartButton(-1);
}

//------------------------------------------------------------------------------
mmiPER::~mmiPER()
//------------------------------------------------------------------------------
{
  vtkDEL(m_CameraBehavior);
  vtkDEL(m_CameraMouseBehavior);
}

//----------------------------------------------------------------------------
void mmiPER::SetAutoAdjustCameraClippingRange( int value )
//----------------------------------------------------------------------------
{
  // TODO: to be implemented
  //m_CameraBehavior->SetAutoAdjustCameraClippingRange( value );
}

//----------------------------------------------------------------------------
bool mmiPER::IsInteracting(mafDevice *device, int button)
//----------------------------------------------------------------------------
{
  std::map<mafID,DeviceItem>::iterator iter = m_Devices.find(device->GetID());
  if (iter!=m_Devices.end()&&(*iter).first==device->GetID())
  {
    if (m_Mode==SINGLE_BUTTON)
    {
      // if optional button parameter is positive report if interaction
      // has been started by a specific button.
      return (button<0)?true:(*iter).second.Button==button;
    }
    else
    {
      // simply check if one or more button has been pressed.
      return (*iter).second.Button>0;
    }
  }

  return false;
}

//----------------------------------------------------------------------------
void mmiPER::InsertDevice(mafDevice *device,int button)
//----------------------------------------------------------------------------
{
  
  if (m_Mode==SINGLE_BUTTON)
  {
    m_Devices[device->GetID()].Button=button;
    m_Devices[device->GetID()].VME=NULL; // initially set picked VME to NULL
  }
  else
  {
    std::map<mafID,DeviceItem>::iterator iter = m_Devices.find(device->GetID());
    if (iter==m_Devices.end())
    {
//      if ((*iter).first==device->GetID())
      m_Devices[device->GetID()].VME=NULL; // initially set picked VME to NULL
      m_Devices[device->GetID()].Button=1;

    }
    else
    {
      //int prebuttonDevice = m_Devices[device->GetID()].Button;
      m_Devices[device->GetID()].Button++;
      //int postbuttonDevice = m_Devices[device->GetID()].Button;//only for debug
    }
  }
}

//----------------------------------------------------------------------------
void mmiPER::RemoveDevice(mafDevice *device)
//----------------------------------------------------------------------------
{
  std::map<mafID,DeviceItem>::iterator iter = m_Devices.find(device->GetID());

  if (iter == m_Devices.end()||(*iter).first!=device->GetID())
  {
    return;
  }
  
  m_Devices[device->GetID()].VME=NULL; // initially set picked VME to NULL
  
  if (Mode==SINGLE_BUTTON)
  {
    m_Devices.erase(iter);
  }
  else
  {
    // decrease counter, if counter gets to zero delte the item
    int cnt=(--m_Devices[device->GetID()].Button);
    if (cnt<=0)
    {
      m_Devices.erase(iter);
    }
  }
}

//----------------------------------------------------------------------------
void mmiPER::SetPickedVME(mafDevice *device,mafVME *vme)
//----------------------------------------------------------------------------
{
  if (device)
  {
    std::map<mafID,DeviceItem>::iterator iter = m_Devices.find(device->GetID());
    //assert(iter != m_Devices.end()); // check the device has started an interaction
    if (iter != m_Devices.end())
      (*iter).second.VME=vme;
  }
  else
  {
    SetVME(vme);
  }
}

//----------------------------------------------------------------------------
mafVME *mmiPER::GetPickedVME(mafDevice *device)
//----------------------------------------------------------------------------
{
  if (device)
  {
    std::map<mafID,DeviceItem>::iterator iter = m_Devices.find(device->GetID());
    //assert(iter != m_Devices.end()); // check the device has started an interaction
    return (iter != m_Devices.end())?(*iter).second.VME:NULL;
  }
  else
  {
    return m_VME;
  }
}

//----------------------------------------------------------------------------
void mmiPER::OnChar(mafEventInteraction *e) 
//----------------------------------------------------------------------------
{
  switch (e->GetKey()) 
  {
    case 'r' :      
    case 'R' :
    {
      mmdMouse *mouse = mmdMouse::SafeDownCast((mafDevice *)e->GetSender());
      if (mouse)
      {
        mafView *view = mouse->GetView();
        ForwardEvent(CAMERA_FIT,MCH_UP,view);
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
void mmiPER::OnLeftButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  if(m_FlyToFlag)
    FlyTo(e); 

  OnButtonDown(e);
  m_DraggingLeft = true;
}

//----------------------------------------------------------------------------
void mmiPER::OnMiddleButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnButtonDown(e);
}
//----------------------------------------------------------------------------
void mmiPER::OnRightButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
	if(m_FlyToFlag)
     FlyTo(e,20,-0.6); 

  OnButtonDown(e);

	m_ShowContextMenu = true;
}

//----------------------------------------------------------------------------
void mmiPER::OnLeftButtonUp(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  //if(m_FlyToFlag)
  //  m_FlyToFlag = false; //SIL. 2-7-2004: - moved in FlyTo because if activated throught the voice label
                        //we receive a Mouseup right after 
  OnButtonUp(e);
}
//----------------------------------------------------------------------------
void mmiPER::OnMiddleButtonUp(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnButtonUp(e);
}
//----------------------------------------------------------------------------
void mmiPER::OnRightButtonUp(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
	//if(m_FlyToFlag)
  //  m_FlyToFlag = false; //SIL. 2-7-2004: - moved in FlyTo because if activated throught the voice label
                        //we receive a Mouseup right after

  mmdMouse *mouse = mmdMouse::SafeDownCast((mafDevice*)e->GetSender());

	if(m_ShowContextMenu && mouse)
	{
    mafVME *vme = GetPickedVME(mouse);
    ForwardEvent(SHOW_CONTEXTUAL_MENU,MCH_UP,vme);
	}

  OnButtonUp(e);
}
//----------------------------------------------------------------------------
void mmiPER::OnMove(mafEventInteraction *e) 
//----------------------------------------------------------------------------
{
	m_ShowContextMenu = false;
}

//----------------------------------------------------------------------------
void mmiPER::OnButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  mafDevice *device = mafDevice::SafeDownCast((mafDevice*)e->GetSender());

  mafVME        *picked_vme  = NULL;
  vtkProp3D     *picked_prop = NULL;
  mafInteractor *picked_bh   = NULL;
  mafMatrix     *point_pose  = e->GetMatrix();
  double        pos_2d[2];

  mmdMouse   *mouse   = mmdMouse::SafeDownCast(device);
  mmdTracker *tracker = mmdTracker::SafeDownCast(device);

  if (point_pose && tracker)
  { 
    if(FindPokedVme(device,point_pose,picked_prop,picked_vme,picked_bh)) 
    {
      // if a VME is picked its pointer is written in PickedVME
      if(m_CanSelect && !picked_vme->IsMAFType(mafVMEGizmo))
      {
        // Send a VME select event to Logic
        ForwardEvent(VME_SELECT,MCH_UP,picked_vme);
      }
    }

    // Forward the start event to the right behavior
    if(picked_bh)
    {
      // if a vme with a behavior has been picked... /****/
      SetPickedVME(device,picked_vme); 
      picked_bh->SetVME(picked_vme);   // set the VME (Marco: to be revoved, the operation should set the VME to the interactor!) /****/
      picked_bh->SetProp(picked_prop); // set the prop (Marco: to be removed, no access to the vtkProp!!!) /****/
      picked_bh->OnEvent(e); // forward the start event to picked behavior
    }
    // if I don't picked a VME or I picked but I cannot select, move the camera.
    else if (m_CameraBehavior&&(!m_CameraBehavior->IsInteracting()))
    {
      // if the camera behavior is free...
      m_CameraBehavior->OnEvent(e); // forward to the camera behavior
    }
  }
  else if (mouse)
  {
    e->Get2DPosition(pos_2d);
    mafMatrix point_pose;
    point_pose.SetElement(0,3,pos_2d[0]);
    point_pose.SetElement(1,3,pos_2d[1]);

    if(FindPokedVme(device,point_pose,picked_prop,picked_vme,picked_bh))
    {
      SetPickedVME(device,picked_vme); 
      // if a VME is picked its pointer is written in PickedVME
      if(m_CanSelect && !picked_vme->IsMAFType(mafVMEGizmo))
      {
        // Send a VME select event to Logic
        ForwardEvent(VME_SELECT,MCH_UP,picked_vme);
      }
    }

    // Forward the start event to the right behavior
    if(picked_bh)
	  {
      // if a vme with a behavior has been picked... 
//      SetPickedVME(device,picked_vme); 
      picked_bh->SetVME(picked_vme);   // set the VME (Marco: to be removed, the operation should set the VME to the interactor!) 
      picked_bh->SetProp(picked_prop); // set the prop (Marco: to be removed, no access to the vtkProp!!!) 
      picked_bh->OnEvent(e); // forward the start event to picked behavior
    }
    // if I don't picked a VME or I picked but I cannot select, move the camera.
    else if (m_CameraMouseBehavior&&(!m_CameraMouseBehavior->IsInteracting()))
    {
      // if the camera behavior is free...
      m_CameraMouseBehavior->OnEvent(e); // forward to the camera behavior
    }
  }
}

//----------------------------------------------------------------------------
void mmiPER::OnButtonUp(mafEventInteraction *e) 
//----------------------------------------------------------------------------
{
  m_DraggingLeft = false;
}

//------------------------------------------------------------------------------
int mmiPER::StartInteraction(mafDevice *device, int button)
//------------------------------------------------------------------------------
{
  if (device&&device->IsLocked())
    return false;

  // Check if already interacting with the same device: it could happen the user
  // contemporary pressed another button...
  if (Mode==SINGLE_BUTTON)
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
int mmiPER::StopInteraction(mafDevice *device, int button)
//------------------------------------------------------------------------------
{
  if (Mode==SINGLE_BUTTON)
  {
    if (IsInteracting(device,button))
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
int mmiPER::OnStartInteraction(mafEventInteraction *e)
//------------------------------------------------------------------------------
{
  mafDevice *device=mafDevice::SafeDownCast((mafDevice *)e->GetSender());
  
  assert(device);
  int button=e->GetButton();
  if (StartInteraction(device,button))
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
      StopInteraction(device,button); // stop immediatelly and return false
      return 0;;
    }
    return 1;
  }
  return 0;
}
//------------------------------------------------------------------------------
int mmiPER::OnStopInteraction(mafEventInteraction *e)
//------------------------------------------------------------------------------
{
  mafDevice *device=mafDevice::SafeDownCast((mafDevice *)e->GetSender());
  assert(device);

  int button=e->GetButton();
  if (Mode==MULTI_BUTTON||IsInteracting(device,button))
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
    StopInteraction(device,button);
    return 1;
  }
  return 0;
}

//------------------------------------------------------------------------------
void mmiPER::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  mafID ch=event->GetChannel();
  
  if (ch==MCH_INPUT)
  {
    mafID id=event->GetId();
    mafDevice *device=(mafDevice *)event->GetSender();
    assert(device);

    if (id == mmdMouse::MOUSE_CHAR_EVENT && !IsInteracting(device))
    {
      mafEventInteraction *e=mafEventInteraction::SafeDownCast(event);
      OnChar(e);
    }

    // find if this device is one of those currently interacting
    if (IsInteracting(device))
    {
      // process the Move event
      if (id==mmdTracker::TRACKER_3D_MOVE || id == mmdMouse::MOUSE_2D_MOVE)
      {
        mafEventInteraction *e=mafEventInteraction::SafeDownCast(event);
        OnMove(e);
      }

      // In any case, forward the event to the right behavior
      mafVME *vme=GetPickedVME(device);
      if(vme)
      {
        mafInteractor *bh = vme->GetBehavior(); //can be NULL
        if (bh)
        {
          bh->OnEvent(event); // forward to VME behavior
        }
      }
      // if no picked behavior...
      if (m_CameraBehavior&&m_CameraBehavior->IsInteracting(device))
      {
        m_CameraBehavior->OnEvent(event); // forward to Camera behavior
      }
      else if (m_CameraMouseBehavior&&m_CameraMouseBehavior->IsInteracting(device))
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
void mmiPER::FlyTo(mafEventInteraction *e,int numstep, double zoom)
//----------------------------------------------------------------------------
{
  mafVME        *picked_vme  = NULL;
  vtkProp3D     *picked_prop = NULL;
  mafInteractor *picked_bh   = NULL;
  mafMatrix     *point_pose  = e->GetMatrix();
  double         pose_2d[2];

  mmdTracker *tracker = mmdTracker::SafeDownCast((mafDevice*)e->GetSender());
  mmdMouse   *mouse   = mmdMouse::SafeDownCast((mafDevice*)e->GetSender());
  
  assert(tracker || mouse);

  if (point_pose && tracker && tracker->GetAvatar())
  { 
    vtkRenderer *ren = tracker->GetAvatar()->GetRenderer();
    mafView *view = tracker->GetAvatar()->GetView();
    
    if(!ren) return; // no renderer no fly to!

	  vtkCamera *cam = ren->GetActiveCamera();

    if(!FindPokedVme(tracker,point_pose,picked_prop,picked_vme,picked_bh)) 
      return; // no picking no fly to!


    int i, j;
	  double f0[3]; // start focal point
	  cam->GetFocalPoint(f0);
	  double p0[3]; // start camera position
	  cam->GetPosition(p0);
    
    // find picked position
    vtkAbstractPropPicker *picker = tracker->GetAvatar()->GetPicker();
    double *pos = picker->GetPickPosition();
	  double foc_motion[3];
    for (i=0; i<3; i++) foc_motion[i] = pos[i] - f0[i];
    double foc_step = vtkMath::Normalize(foc_motion)/numstep;

    if(zoom >  1) zoom = 1;
    if(zoom < -1) zoom =-1;
	  double cam_motion[3];
    for (i=0; i<3; i++) cam_motion[i] = (pos[i]-f0[i]) + (f0[i]-p0[i])*zoom/2;
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
      ForwardEvent(CAMERA_UPDATE,MCH_UP,view);

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

    if(!FindPokedVme(mouse,point_pose,picked_prop,picked_vme,picked_bh)) 
      return; // no picking no fly to!

    int i, j;
	  double f0[3]; // start focal point
	  cam->GetFocalPoint(f0);
	  double p0[3]; // start camera position
	  cam->GetPosition(p0);
    
    // find picked position
    vtkAbstractPropPicker *picker = mouse->GetPicker();
    double *pos = picker->GetPickPosition();
	  double foc_motion[3];
    for (i=0; i<3; i++) foc_motion[i] = pos[i] - f0[i];
    double foc_step = vtkMath::Normalize(foc_motion)/numstep;

    if(zoom >  1) zoom = 1;
    if(zoom < -1) zoom =-1;
	  double cam_motion[3];
    for (i=0; i<3; i++) cam_motion[i] = (pos[i]-f0[i]) + (f0[i]-p0[i])*zoom/2;
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
      ForwardEvent(CAMERA_UPDATE,MCH_UP,view));

    }
    cam->OrthogonalizeViewUp();

    m_FlyToFlag = false;
  }

}

