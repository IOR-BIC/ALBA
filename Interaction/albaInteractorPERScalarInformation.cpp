/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorPERScalarInformation
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
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


#include "albaInteractorPERScalarInformation.h"

#include "albaView.h"
#include "albaViewCompound.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaAvatar3D.h"
#include "albaInteractor6DOFCameraMove.h"
#include "albaInteractorCameraMove.h"

#include "albaEventBase.h"
#include "albaEventInteraction.h"

#include "albaRWIBase.h"

#include "albaVME.h"
#include "albaVMEOutput.h"

#include "vtkCellPicker.h"
#include "vtkRenderWindow.h"
#include "vtkPoints.h"
#include "vtkRendererCollection.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include <assert.h>

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractorPERScalarInformation)
//------------------------------------------------------------------------------
albaInteractorPERScalarInformation::albaInteractorPERScalarInformation()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
albaInteractorPERScalarInformation::~albaInteractorPERScalarInformation()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void albaInteractorPERScalarInformation::OnEvent(albaEventBase *event)
//------------------------------------------------------------------------------
{
  albaID ch = event->GetChannel();

  if (ch == MCH_INPUT)
  {
    albaID id = event->GetId();
    albaDevice *device = (albaDevice *)event->GetSender();
    assert(device);

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
    else
    {
      double mouse_pos[2];
      albaEventInteraction *e = albaEventInteraction::SafeDownCast(event);
      e->Get2DPosition(mouse_pos);
      bool picked_something = false;

      vtkCellPicker *cellPicker;
      vtkNEW(cellPicker);
      cellPicker->SetTolerance(0.001);
      albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::SafeDownCast(device);
      albaView *v = mouse->GetView();
      if (v)
      {
        albaViewCompound *vc = albaViewCompound::SafeDownCast(v);
        if (vc)
        {
          v = vc->GetSubView();
        }
      }
      if (v)
      {
        vtkRendererCollection *rc = v->GetRWI()->GetRenderWindow()->GetRenderers();
        vtkRenderer *r = NULL;
        rc->InitTraversal();
        while(r = rc->GetNextItem())
        {
          if(cellPicker->Pick(mouse_pos[0],mouse_pos[1],0,r))
          {
            picked_something = true;
          }
        }
        if (picked_something)
        {
          vtkPoints *p = vtkPoints::New();
          double pos_picked[3];
          cellPicker->GetPickPosition(pos_picked);
          p->SetNumberOfPoints(1);
          p->SetPoint(0,pos_picked);
          v->Pick(mouse_pos[0],mouse_pos[1]);
          double scalar_value = 0;
          albaVME *pickedVME = v->GetPickedVme();
          if(pickedVME)
          {
            vtkDataSet *vtk_data = pickedVME->GetOutput()->GetVTKData();
            //GetPickPosition calulate the picking position with matrix multiplication 
            //the return value can be affected of some approximation errors, if the value
            //is outside the bounds FindPoint will return -1;
            double bounds[6];
            vtk_data->GetBounds(bounds);
            if (pos_picked[0]<bounds[0]) pos_picked[0]=bounds[0];
            if (pos_picked[0]>bounds[1]) pos_picked[0]=bounds[1];
            if (pos_picked[1]<bounds[2]) pos_picked[1]=bounds[2];
            if (pos_picked[1]>bounds[3]) pos_picked[1]=bounds[3];
            if (pos_picked[2]<bounds[4]) pos_picked[2]=bounds[4];
            if (pos_picked[2]>bounds[5]) pos_picked[2]=bounds[5];
            int pid = vtk_data->FindPoint(pos_picked);
            vtkDataArray *scalars = vtk_data->GetPointData()->GetScalars();
            if (scalars)
              scalars->GetTuple(pid,&scalar_value);
            albaEvent pick_event(this,MOUSE_MOVE,p);
            pick_event.SetDouble(scalar_value);
            pick_event.SetArg(pid);
            albaEventMacro(pick_event);
            p->Delete();
          }
        }
      }
      vtkDEL(cellPicker);
    }
  }
  // Make the superclass to manage StartInteractionEvent
  // and StopInteractionEvent: this will make OnStart/StopInteraction()
  // to be called, or eventually the event to be forwarded.
  Superclass::OnEvent(event);
}
