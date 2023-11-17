/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorPERBrushFeedback
 Authors: Eleonora Mambrini, Gianluigi Crimi
 
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


#include "albaInteractorPERBrushFeedback.h"

#include "albaDeviceButtonsPadTracker.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaEventBase.h"
#include "albaEventInteraction.h"
#include "albaInteractorCameraMove.h"
#include "albaRWIBase.h"
#include "albaView.h"
#include "albaViewCompound.h"
#include "albaVME.h"
#include "albaVMEOutput.h"

#include "vtkActor2D.h"
#include "vtkCamera.h"
#include "vtkCellPicker.h"
#include "vtkCoordinate.h"
#include "vtkCubeSource.h"
#include "vtkDataSet.h"
#include "vtkImageMapper.h"
#include "vtkMapper2D.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindow.h"
#include "vtkSphereSource.h"

#include <assert.h>
#include "wx\event.h"
//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractorPERBrushFeedback)


//------------------------------------------------------------------------------
albaInteractorPERBrushFeedback::albaInteractorPERBrushFeedback()
{
}

//------------------------------------------------------------------------------
albaInteractorPERBrushFeedback::~albaInteractorPERBrushFeedback()
{
}

//------------------------------------------------------------------------------
void albaInteractorPERBrushFeedback::OnEvent(albaEventBase *event)
{
  // Make the superclass to manage StartInteractionEvent
  // and StopInteractionEvent: this will make OnStart/StopInteraction()
  // to be called, or eventually the event to be forwarded.

  albaID ch = event->GetChannel();

  if (ch == MCH_INPUT)
  {
    albaID id = event->GetId();
    albaDevice *device = (albaDevice *)event->GetSender();
    assert(device);

		albaEventInteraction *e = albaEventInteraction::SafeDownCast(event);

    if (id == albaDeviceButtonsPadMouse::GetMouseCharEventId() && !IsInteracting(device))
    {
      OnChar(e);
    }
		else if (id == albaDeviceButtonsPadMouse::GetWheelId() && e->GetModifier(ALBA_SHIFT_KEY))
		{
			long rotation = *(double *)e->GetData();

			albaEvent scrollEvent(this, MOUSE_WHEEL, rotation);
			scrollEvent.SetPointer(m_PickPosition);
			albaEventMacro(scrollEvent);
			
			return;
		}

    // find if this device is one of those currently interacting
    if (IsInteracting(device))
    {
      /*m_Count = 0;*/
      // process the Move event
      if (id == albaDeviceButtonsPadTracker::GetTracker3DMoveId() || id == albaDeviceButtonsPadMouse::GetMouse2DMoveId())
      {
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
						break;
          }
        }
				
				cellPicker->GetPickPosition(m_PickPosition);

				albaEvent pick_event(this, MOUSE_MOVE);
				pick_event.SetPointer(m_PickPosition);
				pick_event.SetArg(picked_something);
				pick_event.SetBool(e->GetModifier(ALBA_CTRL_KEY) == true);
				albaEventMacro(pick_event);
      }
      vtkDEL(cellPicker);
    }
  }

	Superclass::OnEvent(event);
}