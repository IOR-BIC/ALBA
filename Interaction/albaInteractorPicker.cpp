/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorPicker
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


#include "albaInteractorPicker.h"
#include "albaPickHelper.h"
#include "albaEvent.h"

#include "albaView.h"
//#include "albaViewCompound.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaAvatar3D.h"

#include "albaEventInteraction.h"

#include "albaRWIBase.h"
#include "albaVME.h"
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
albaCxxTypeMacro(albaInteractorPicker)
//------------------------------------------------------------------------------
albaInteractorPicker::albaInteractorPicker()
//------------------------------------------------------------------------------
{
  m_ContinuousPickingFlag = false;
}

//------------------------------------------------------------------------------
albaInteractorPicker::~albaInteractorPicker()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void albaInteractorPicker::OnEvent(albaEventBase *event)
//------------------------------------------------------------------------------
{
  Superclass::OnEvent(event);
  if (m_ContinuousPickingFlag)
  {
    if (albaDeviceButtonsPadMouse *mouse=albaDeviceButtonsPadMouse::SafeDownCast((albaDevice *)event->GetSender()))
    { 
      double mouse_pos[2];
      ((albaEventInteraction *)event)->Get2DPosition(mouse_pos);
      SendPickingInformation(mouse->GetView(), mouse_pos,VME_PICKING);
    }
  }
}
//----------------------------------------------------------------------------
void albaInteractorPicker::OnButtonDown(albaEventInteraction *e)
//----------------------------------------------------------------------------
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
    double mouse_pos[2];
    e->Get2DPosition(mouse_pos);
    SendPickingInformation(mouse->GetView(), mouse_pos);
  }
}

//----------------------------------------------------------------------------
void albaInteractorPicker::OnButtonUp(albaEventInteraction *e) 
//----------------------------------------------------------------------------
{
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
      double mouse_pos[2];
      e->Get2DPosition(mouse_pos);
      SendPickingInformation(mouse->GetView(), mouse_pos);
    }
  }
}
//----------------------------------------------------------------------------
void albaInteractorPicker::SendPickingInformation(albaView *v, double *mouse_pos, int msg_id, albaMatrix *tracker_pos, bool mouse_flag)
//----------------------------------------------------------------------------
{
  double pos_picked[3], scalar_value;
  albaVME *pickedVME;
  vtkIdType pid;
  if (albaPickHelper::PickPoint(v, mouse_flag ? mouse_pos : NULL, mouse_flag ? NULL : tracker_pos, pos_picked, pickedVME, pid, scalar_value))
  {
    vtkALBASmartPointer<vtkPoints> p;
    p->SetNumberOfPoints(1);
    p->SetPoint(0,pos_picked);
    albaEvent pick_event(this,msg_id,(vtkObject *)p.GetPointer());
    pick_event.SetDouble(scalar_value);
    pick_event.SetArg(pid);
    albaEventMacro(pick_event);
  }
}
