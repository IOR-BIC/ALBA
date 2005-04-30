/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiPER.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:34:58 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone 
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows
#ifdef __GNUG__
    #pragma implementation "mmiPER.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "mmiPER.h"

#include "mafInteractionDecl.h"

#include "mmdTracker.h"
#include "mmdMouse.h"
#include "mafAvatar3D.h"
#include "mmi6DOFCameraMove.h"
#include "mmiCameraMove.h"

#include "mafVmeData.h"

#include "mafEventBase.h"
#include "mflEventInteraction.h"

#include "mflAssembly.h"
#include "mflVME.h"
#include "mflTransform.h"

#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkAbstractPropPicker.h"
#include "vtkObjectFactory.h"
#include "vtkAssemblyPath.h"
#include "vtkAssemblyNode.h"
#include "vtkMath.h"
#include <map>
#include <assert.h>

//------------------------------------------------------------------------------
// PIMPL
//------------------------------------------------------------------------------
class DeviceItem
{
public:
  mflVME *VME;
  int     Button;
};
class myPIMPL
{
  public:
  std::map<mafID,DeviceItem> Ids;
};

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
//MFL_EVT_IMP(mmiPER::MoveActionEvent);

//------------------------------------------------------------------------------
vtkStandardNewMacro(mmiPER)
//------------------------------------------------------------------------------
mmiPER::mmiPER()
//------------------------------------------------------------------------------
{
  DraggingLeft    = false; 
  CanSelect       = true; 
  FlyToFlag       = false;
  StartButton     = -1;

  Devices         = new myPIMPL;
  CameraBehavior  = mmi6DOFCameraMove::New(); // allocate camera behavior
  CameraBehavior->SetStartButton(MAF_LEFT_BUTTON); // any button
  SetModeToSingleButton();
  FirstTime = 0;

  CameraMouseBehavior = mmiCameraMove::New(); // allocate mouse camera behavior
  CameraMouseBehavior->SetStartButton(-1);
}

//------------------------------------------------------------------------------
mmiPER::~mmiPER()
//------------------------------------------------------------------------------
{
  delete Devices; Devices = NULL;
  vtkDEL(CameraBehavior);
  vtkDEL(CameraMouseBehavior);
}

//----------------------------------------------------------------------------
void mmiPER::SetAutoAdjustCameraClippingRange( int value )
//----------------------------------------------------------------------------
{
  // TODO: to be implemented
  //CameraBehavior->SetAutoAdjustCameraClippingRange( value );
}

//----------------------------------------------------------------------------
bool mmiPER::IsInteracting(mafDevice *device, int button)
//----------------------------------------------------------------------------
{
  std::map<mafID,DeviceItem>::iterator iter = Devices->Ids.find(device->GetID());
  if (iter!=Devices->Ids.end()&&(*iter).first==device->GetID())
  {
    if (Mode==SINGLE_BUTTON)
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
  
  if (Mode==SINGLE_BUTTON)
  {
    Devices->Ids[device->GetID()].Button=button;
    Devices->Ids[device->GetID()].VME=NULL; // initially set picked VME to NULL
  }
  else
  {
    std::map<mafID,DeviceItem>::iterator iter = Devices->Ids.find(device->GetID());
    if (iter==Devices->Ids.end())
    {
//      if ((*iter).first==device->GetID())
      Devices->Ids[device->GetID()].VME=NULL; // initially set picked VME to NULL
      Devices->Ids[device->GetID()].Button=1;

    }
    else
    {
      //int prebuttonDevice = Devices->Ids[device->GetID()].Button;
      Devices->Ids[device->GetID()].Button++;
      //int postbuttonDevice = Devices->Ids[device->GetID()].Button;//only for debug
    }
  }
}

//----------------------------------------------------------------------------
void mmiPER::RemoveDevice(mafDevice *device)
//----------------------------------------------------------------------------
{
  std::map<mafID,DeviceItem>::iterator iter = Devices->Ids.find(device->GetID());

  if (iter == Devices->Ids.end()||(*iter).first!=device->GetID())
  {
    return;
  }
  
  Devices->Ids[device->GetID()].VME=NULL; // initially set picked VME to NULL
  
  if (Mode==SINGLE_BUTTON)
  {
    Devices->Ids.erase(iter);
  }
  else
  {
    // decrease counter, if counter gets to zero delte the item
    int cnt=(--Devices->Ids[device->GetID()].Button);
    if (cnt<=0)
    {
      Devices->Ids.erase(iter);
    }
  }
}

//----------------------------------------------------------------------------
void mmiPER::SetPickedVME(mafDevice *device,mflVME *vme)
//----------------------------------------------------------------------------
{
  if (device)
  {
    std::map<mafID,DeviceItem>::iterator iter = Devices->Ids.find(device->GetID());
    //assert(iter != Devices->Ids.end()); // check the device has started an interaction
    if (iter != Devices->Ids.end())
      (*iter).second.VME=vme;
  }
  else
  {
    SetVME(vme);
  }
}

//----------------------------------------------------------------------------
mflVME *mmiPER::GetPickedVME(mafDevice *device)
//----------------------------------------------------------------------------
{
  if (device)
  {
    std::map<mafID,DeviceItem>::iterator iter = Devices->Ids.find(device->GetID());
    //assert(iter != Devices->Ids.end()); // check the device has started an interaction
    return (iter != Devices->Ids.end())?(*iter).second.VME:NULL;
  }
  else
  {
    return VME;
  }
}

//----------------------------------------------------------------------------
void mmiPER::OnChar(mflEventInteraction *e) 
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
        ForwardEvent(mafSmartEvent(this,CameraFitEvent,view));
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
void mmiPER::OnLeftButtonDown(mflEventInteraction *e)
//----------------------------------------------------------------------------
{
  if(FlyToFlag)
    FlyTo(e); 

  OnButtonDown(e);
  DraggingLeft = true;
}

//----------------------------------------------------------------------------
void mmiPER::OnMiddleButtonDown(mflEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnButtonDown(e);
}
//----------------------------------------------------------------------------
void mmiPER::OnRightButtonDown(mflEventInteraction *e)
//----------------------------------------------------------------------------
{
	if(FlyToFlag)
     FlyTo(e,20,-0.6); 

  OnButtonDown(e);

	ShowContextMenu = true;
}

//----------------------------------------------------------------------------
void mmiPER::OnLeftButtonUp(mflEventInteraction *e)
//----------------------------------------------------------------------------
{
  //if(FlyToFlag)
  //  FlyToFlag = false; //SIL. 2-7-2004: - moved in FlyTo because if activated throught the voice label
                        //we receive a Mouseup right after 
  OnButtonUp(e);
}
//----------------------------------------------------------------------------
void mmiPER::OnMiddleButtonUp(mflEventInteraction *e)
//----------------------------------------------------------------------------
{
  OnButtonUp(e);
}
//----------------------------------------------------------------------------
void mmiPER::OnRightButtonUp(mflEventInteraction *e)
//----------------------------------------------------------------------------
{
	//if(FlyToFlag)
  //  FlyToFlag = false; //SIL. 2-7-2004: - moved in FlyTo because if activated throught the voice label
                        //we receive a Mouseup right after

  mmdMouse *mouse = mmdMouse::SafeDownCast((mafDevice*)e->GetSender());

	if(ShowContextMenu && mouse)
	{
    mflVME *vme = GetPickedVME(mouse);
    ForwardEvent(mafSmartEvent(this,ShowContextualMenuEvent,vme));
	}

  OnButtonUp(e);
}
//----------------------------------------------------------------------------
void mmiPER::OnMove(mflEventInteraction *e) 
//----------------------------------------------------------------------------
{
	ShowContextMenu = false;
}

//----------------------------------------------------------------------------
void mmiPER::OnButtonDown(mflEventInteraction *e)
//----------------------------------------------------------------------------
{
  mafDevice *device = mafDevice::SafeDownCast((mafDevice*)e->GetSender());

  mflVME        *picked_vme  = NULL;
  vtkProp3D     *picked_prop = NULL;
  mafInteractor *picked_bh   = NULL;
  mflMatrix     *point_pose  = e->GetMatrix();
  double        pos_2d[2];

  mmdMouse   *mouse   = mmdMouse::SafeDownCast(device);
  mmdTracker *tracker = mmdTracker::SafeDownCast(device);

  if (point_pose && tracker)
  { 
    if(FindPokedVme(device,point_pose,picked_prop,picked_vme,picked_bh)) 
    {
      // if a VME is picked its pointer is written in PickedVME
      if(CanSelect && !picked_vme->IsA("mflVMEGizmo"))
		  {
        // Send a VME select event to Logic
        ForwardEvent(VMESelectEvent,DefaultChannel,picked_vme);
		  }
    }

    // Forward the start event to the right behavior
    if(picked_bh)
	  {
      // if a vme with a behavior has been picked... /****/
      SetPickedVME(device,picked_vme); 
      picked_bh->SetVME(picked_vme);   // set the VME (Marco: to be revoved, the operation should set the VME to the interactor!) /****/
      picked_bh->SetProp(picked_prop); // set the prop (Marco: to be removed, no access to the vtkProp!!!) /****/
      picked_bh->ProcessEvent(e,mafDevice::DeviceInputChannel); // forward the start event to picked behavior
    }
    // if I don't picked a VME or I picked but I cannot select, move the camera.
    else if (CameraBehavior&&(!CameraBehavior->IsInteracting()))
    {
      // if the camera behavior is free...
      CameraBehavior->ProcessEvent(e,mafDevice::DeviceInputChannel); // forward to the camera behavior
    }
  }
  else if (mouse)
  {
    e->Get2DPosition(pos_2d);
    mflSmartPointer<mflMatrix> point_pose;
    point_pose->SetElement(0,3,pos_2d[0]);
    point_pose->SetElement(1,3,pos_2d[1]);

    if(FindPokedVme(device,point_pose,picked_prop,picked_vme,picked_bh))
    {
      SetPickedVME(device,picked_vme); 
      // if a VME is picked its pointer is written in PickedVME
      if(CanSelect && !picked_vme->IsA("mflVMEGizmo"))
		  {
        // Send a VME select event to Logic
        ForwardEvent(VMESelectEvent,DefaultChannel,picked_vme);
		  }
    }

    // Forward the start event to the right behavior
    if(picked_bh)
	  {
      // if a vme with a behavior has been picked... 
//      SetPickedVME(device,picked_vme); 
      picked_bh->SetVME(picked_vme);   // set the VME (Marco: to be removed, the operation should set the VME to the interactor!) 
      picked_bh->SetProp(picked_prop); // set the prop (Marco: to be removed, no access to the vtkProp!!!) 
      picked_bh->ProcessEvent(e,mafDevice::DeviceInputChannel); // forward the start event to picked behavior
    }
    // if I don't picked a VME or I picked but I cannot select, move the camera.
    else if (CameraMouseBehavior&&(!CameraMouseBehavior->IsInteracting()))
    {
      // if the camera behavior is free...
      CameraMouseBehavior->ProcessEvent(e,mafDevice::DeviceInputChannel); // forward to the camera behavior
    }
  }

}

//----------------------------------------------------------------------------
void mmiPER::OnButtonUp(mflEventInteraction *e) 
//----------------------------------------------------------------------------
{
  DraggingLeft = false;
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

  vtkGenericWarningMacro("StopInteraction without a Start for device "<< device->GetName());
  return false;
}

//------------------------------------------------------------------------------
int mmiPER::OnStartInteraction(mflEventInteraction *e)
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
int mmiPER::OnStopInteraction(mflEventInteraction *e)
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
void mmiPER::ProcessEvent(mflEvent *event,mafID ch)
//------------------------------------------------------------------------------
{
  if (ch==mafDevice::DeviceInputChannel)
  {
    mafID id=event->GetID();
    mafDevice *device=(mafDevice *)event->GetSender();
    assert(device);

    if (id == mmdMouse::CharEvent && !IsInteracting(device))
    {
      mflEventInteraction *e=mflEventInteraction::SafeDownCast(event);
      OnChar(e);
    }

    // find if this device is one of those currently interacting
    if (IsInteracting(device))
    {
      // process the Move event
      if (id==mmdTracker::Move3DEvent || id == mmdMouse::Move2DEvent)
      {
        mflEventInteraction *e=mflEventInteraction::SafeDownCast(event);
        OnMove(e);
      }

      // In any case, forward the event to the right behavior
      mflVME *vme=GetPickedVME(device);
      if(vme && vme->GetClientData() )
      {
        mafVmeData *vd = (mafVmeData*) vme->GetClientData();
        mafInteractor *bh = vd->m_behavior; //can be NULL
        if (bh)
        {
          bh->ProcessEvent(event,ch); // forward to VME behavior
        }
      }
      // if no picked behavior...
      if (CameraBehavior&&CameraBehavior->IsInteracting(device))
      {
        CameraBehavior->ProcessEvent(event,ch); // forward to Camera behavior
      }
      else if (CameraMouseBehavior&&CameraMouseBehavior->IsInteracting(device))
      {
        CameraMouseBehavior->ProcessEvent(event,ch); // forward to Camera behavior
      }
    }
  }

  // Make the superclass to manage StartInteractionEvent
  // and StopInteractionEvent: this will make OnStart/StopInteraction()
  // to be called, or eventually the event to be forwarded.
  Superclass::ProcessEvent(event,ch);
    
}

//----------------------------------------------------------------------------
void mmiPER::FlyTo(mflEventInteraction *e,int numstep, float zoom)
//----------------------------------------------------------------------------
{
  mflVME        *picked_vme  = NULL;
  vtkProp3D     *picked_prop = NULL;
  mafInteractor *picked_bh   = NULL;
  mflMatrix     *point_pose  = e->GetMatrix();
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
	  float f0[3]; // start focal point
	  cam->GetFocalPoint(f0);
	  float p0[3]; // start camera position
	  cam->GetPosition(p0);
    
    // find picked position
    vtkAbstractPropPicker *picker = tracker->GetAvatar()->GetPicker();
    float *pos = picker->GetPickPosition();
	  float foc_motion[3];
    for (i=0; i<3; i++) foc_motion[i] = pos[i] - f0[i];
    float foc_step = vtkMath::Normalize(foc_motion)/numstep;

    if(zoom >  1) zoom = 1;
    if(zoom < -1) zoom =-1;
	  float cam_motion[3];
    for (i=0; i<3; i++) cam_motion[i] = (pos[i]-f0[i]) + (f0[i]-p0[i])*zoom/2;
    float cam_step = vtkMath::Normalize(cam_motion)/numstep;
    float f1[3]; // new focal point
	  float p1[3]; // new camera position

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
      ForwardEvent(mafSmartEvent(this,CameraUpdateEvent,view));

    }
    cam->OrthogonalizeViewUp();

    FlyToFlag = false;
  }
  else if (mouse)
  {
    mouse->GetLastPosition(pose_2d);
    mflSmartPointer<mflMatrix> point_pose;
    point_pose->SetElement(0,3,pose_2d[0]);
    point_pose->SetElement(1,3,pose_2d[1]);

    vtkRenderer *ren = mouse->GetRenderer();
    mafView *view = mouse->GetView();
    
    if(!ren) return; // no renderer no fly to!

	  vtkCamera *cam = ren->GetActiveCamera();

    if(!FindPokedVme(mouse,point_pose,picked_prop,picked_vme,picked_bh)) 
      return; // no picking no fly to!

    int i, j;
	  float f0[3]; // start focal point
	  cam->GetFocalPoint(f0);
	  float p0[3]; // start camera position
	  cam->GetPosition(p0);
    
    // find picked position
    vtkAbstractPropPicker *picker = mouse->GetPicker();
    float *pos = picker->GetPickPosition();
	  float foc_motion[3];
    for (i=0; i<3; i++) foc_motion[i] = pos[i] - f0[i];
    float foc_step = vtkMath::Normalize(foc_motion)/numstep;

    if(zoom >  1) zoom = 1;
    if(zoom < -1) zoom =-1;
	  float cam_motion[3];
    for (i=0; i<3; i++) cam_motion[i] = (pos[i]-f0[i]) + (f0[i]-p0[i])*zoom/2;
    float cam_step = vtkMath::Normalize(cam_motion)/numstep;
    float f1[3]; // new focal point
	  float p1[3]; // new camera position

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
      ForwardEvent(mafSmartEvent(this,CameraUpdateEvent,view));

    }
    cam->OrthogonalizeViewUp();

    FlyToFlag = false;
  }

}

