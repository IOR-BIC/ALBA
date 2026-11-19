/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaBehaviorPicker
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


#include "albaBehaviorPicker.h"
#include "albaPickHelper.h"
#include "albaEvent.h"

#include "albaView.h"
//#include "albaViewCompound.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaAvatar3D.h"

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
albaCxxTypeMacro(albaBehaviorPicker)
//------------------------------------------------------------------------------
albaBehaviorPicker::albaBehaviorPicker()
//------------------------------------------------------------------------------
{
  m_ContinuousPickingFlag = false;
}

//------------------------------------------------------------------------------
albaBehaviorPicker::~albaBehaviorPicker()
//------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
bool albaBehaviorPicker::OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2])
{
  SendPickingInformation(view, pos);
  return true;
}

//----------------------------------------------------------------------------
bool albaBehaviorPicker::OnLeftButtonUp(albaView *view, vtkRenderer *ren, double pos[2])
{
  if (m_ContinuousPickingFlag)
    SendPickingInformation(view, pos);
  return true;
}

//----------------------------------------------------------------------------
bool albaBehaviorPicker::OnDrag(albaView *view, vtkRenderer *ren, double pos[2])
{
  if (m_ContinuousPickingFlag)
    SendPickingInformation(view, pos, VME_PICKING);
  return true;
}

//----------------------------------------------------------------------------
bool albaBehaviorPicker::OnTrackerButtonDown(albaDeviceButtonsPadTracker *tracker, int button, albaMatrix *pose)
{
  if (button != ALBA_LEFT_BUTTON)
    return false;

  // the tracker picks along its pose in the world frame, no avatar == no picking
  albaMatrix worldPose;
  if (pose && tracker->TrackerToWorld(*pose, worldPose))
    SendPickingInformation(tracker->GetView(), NULL, VME_PICKED, &worldPose, false);
  return true;
}

//----------------------------------------------------------------------------
void albaBehaviorPicker::SendPickingInformation(albaView *v, double *mouse_pos, int msg_id, albaMatrix *tracker_pos, bool mouse_flag)
//----------------------------------------------------------------------------
{
  double pos_picked[3], scalar_value;
  albaVME *pickedVME;
  vtkIdType pid;
  if (albaPickHelper::PickPoint(v, mouse_flag ? mouse_pos : NULL, mouse_flag ? NULL : tracker_pos, pos_picked, pickedVME, pid, scalar_value))
    SendPickedPointEvent(msg_id, pos_picked, pid, scalar_value);
}
