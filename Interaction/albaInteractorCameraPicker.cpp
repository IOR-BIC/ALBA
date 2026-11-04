/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorCameraPicker
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
#include "albaInteractorCameraPicker.h"
#include "albaPickHelper.h"

#include "albaView.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaAvatar3D.h"

#include "albaEventInteraction.h"

#include "albaRWIBase.h"
#include "albaVME.h"
#include "albaVMEOutput.h"
#include "albaTransform.h"

#include "vtkALBASmartPointer.h"
#include "vtkCellPicker.h"
#include "vtkALBARayCast3DPicker.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

#include <assert.h>

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractorCameraPicker)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaInteractorCameraPicker::albaInteractorCameraPicker()
//------------------------------------------------------------------------------
{
  m_ContinuousPickingFlag = false;
}

//------------------------------------------------------------------------------
albaInteractorCameraPicker::~albaInteractorCameraPicker()
//------------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaInteractorCameraPicker::OnButtonDown(albaEventInteraction *e)
//----------------------------------------------------------------------------
{
  Superclass::OnButtonDown(e);
  if (albaDeviceButtonsPadTracker *tracker=albaDeviceButtonsPadTracker::SafeDownCast((albaDevice *)e->GetSender()))
  { // is it a tracker?
    albaMatrix *tracker_pose = e->GetMatrix();
    // extract device avatar's renderer, no avatar == no picking
    albaAvatar *avatar = tracker->GetAvatar();
    if (avatar)
    {
      // compute pose in the world frame
      albaMatrix world_pose;
      albaAvatar3D *avatar3D=albaAvatar3D::SafeDownCast(avatar);
      if (avatar3D)
        avatar3D->TrackerToWorld(*tracker_pose,world_pose,albaAvatar3D::CANONICAL_TO_WORLD_SCALE);
      else
        world_pose = *tracker_pose;
      SendPickingInformation(avatar->GetView(),NULL,VME_PICKED,&world_pose,false);
    }
  }
  else if (albaDeviceButtonsPadMouse *mouse=albaDeviceButtonsPadMouse::SafeDownCast((albaDevice *)e->GetSender()))
  { 
    if (e->GetModifier(ALBA_CTRL_KEY) && e->GetButton() == ALBA_LEFT_BUTTON)
    {
	    double mouse_pos[2];
	    e->Get2DPosition(mouse_pos);
	    SendPickingInformation(mouse->GetView(), mouse_pos);
    }
  }
}

//----------------------------------------------------------------------------
void albaInteractorCameraPicker::OnButtonUp(albaEventInteraction *e) 
//----------------------------------------------------------------------------
{
  Superclass::OnButtonUp(e);
  if (m_ContinuousPickingFlag)
  {
    if (albaDeviceButtonsPadTracker *tracker=albaDeviceButtonsPadTracker::SafeDownCast((albaDevice *)e->GetSender()))
    { // is it a tracker?
      albaMatrix *tracker_pose = e->GetMatrix();
      // extract device avatar's renderer, no avatar == no picking
      albaAvatar *avatar = tracker->GetAvatar();
      if (avatar)
      {
        // compute pose in the world frame
        albaMatrix world_pose;
        albaAvatar3D *avatar3D=albaAvatar3D::SafeDownCast(avatar);
        if (avatar3D)
          avatar3D->TrackerToWorld(*tracker_pose,world_pose,albaAvatar3D::CANONICAL_TO_WORLD_SCALE);
        else
          world_pose = *tracker_pose;
        SendPickingInformation(avatar->GetView(),NULL,VME_PICKED,&world_pose,false);
      }
    }
    else if (albaDeviceButtonsPadMouse *mouse=albaDeviceButtonsPadMouse::SafeDownCast((albaDevice *)e->GetSender()))
    { 
      if (e->GetModifier(ALBA_CTRL_KEY) && e->GetButton() == ALBA_LEFT_BUTTON)
      {
        double mouse_pos[2];
        e->Get2DPosition(mouse_pos);
        SendPickingInformation(mouse->GetView(), mouse_pos);
      }
    }
  }
}
//----------------------------------------------------------------------------
void albaInteractorCameraPicker::SendPickingInformation(albaView *v, double *mouse_pos, int msg_id, albaMatrix *tracker_pos, bool mouse_flag)
//----------------------------------------------------------------------------
{
  // Send to the listener picked point coordinate through vtkPoint and the corresponding scalar value found in that position
  double pos_picked[3], scalar_value;
  albaVME *pickedVME;
  vtkIdType pid;
  if (albaPickHelper::PickPoint(v, mouse_flag ? mouse_pos : NULL, mouse_flag ? NULL : tracker_pos, pos_picked, pickedVME, pid, scalar_value))
    SendPickedPointEvent(msg_id, pos_picked, pid, scalar_value);
}

//----------------------------------------------------------------------------
void albaInteractorCameraPicker::EnableContinuousPicking(bool enable)
//----------------------------------------------------------------------------
{
  // Enable/disable continuous picking in OnEvent
  m_ContinuousPickingFlag = enable;
}

//------------------------------------------------------------------------------
void albaInteractorCameraPicker::OnEvent(albaEventBase *event)
//------------------------------------------------------------------------------
{
  Superclass::OnEvent(event);
  
  // Redefined to send picking events if continuous picking is enabled
  albaEventInteraction *e = (albaEventInteraction *)event;

  if ( m_ContinuousPickingFlag && (e->GetModifier(ALBA_CTRL_KEY)) && e->GetButton() == ALBA_LEFT_BUTTON)
  {
    if (albaDeviceButtonsPadMouse *mouse=albaDeviceButtonsPadMouse::SafeDownCast((albaDevice *)event->GetSender()))
    { 
      double mouse_pos[2];
      e->Get2DPosition(mouse_pos);
      SendPickingInformation(mouse->GetView(), mouse_pos,VME_PICKING);
    }
  }
}