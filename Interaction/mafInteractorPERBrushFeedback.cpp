/*=========================================================================

 Program: MAF2
 Module: mafInteractorPERBrushFeedback
 Authors: Eleonora Mambrini, Gianluigi Crimi
 
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


#include "mafInteractorPERBrushFeedback.h"

#include "mafDeviceButtonsPadTracker.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafEventBase.h"
#include "mafEventInteraction.h"
#include "mafInteractorCameraMove.h"
#include "mafRWIBase.h"
#include "mafView.h"
#include "mafViewCompound.h"
#include "mafVME.h"
#include "mafVMEOutput.h"

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
//------------------------------------------------------------------------------
mafCxxTypeMacro(mafInteractorPERBrushFeedback)
//------------------------------------------------------------------------------
mafInteractorPERBrushFeedback::mafInteractorPERBrushFeedback()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mafInteractorPERBrushFeedback::~mafInteractorPERBrushFeedback()
//------------------------------------------------------------------------------
{

}

//------------------------------------------------------------------------------
void mafInteractorPERBrushFeedback::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  // Make the superclass to manage StartInteractionEvent
  // and StopInteractionEvent: this will make OnStart/StopInteraction()
  // to be called, or eventually the event to be forwarded.
  Superclass::OnEvent(event);

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
      /*m_Count = 0;*/
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
    else
    {
      double mouse_pos[2];
      mafEventInteraction *e = mafEventInteraction::SafeDownCast(event);
      e->Get2DPosition(mouse_pos);
      bool picked_something = false;

      vtkCellPicker *cellPicker;
      vtkNEW(cellPicker);
      cellPicker->SetTolerance(0.001);
      mafDeviceButtonsPadMouse *mouse = mafDeviceButtonsPadMouse::SafeDownCast(device);
      mafView *v = mouse->GetView();
      if (v)
      {
        mafViewCompound *vc = mafViewCompound::SafeDownCast(v);
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

				mafEvent pick_event(this, MOUSE_MOVE);
				pick_event.SetPointer(m_PickPosition);
				pick_event.SetArg(picked_something);
				pick_event.SetBool(e->GetModifier(MAF_CTRL_KEY) == true);
				mafEventMacro(pick_event);
      }
      vtkDEL(cellPicker);
    }
  }
}

