/*=========================================================================

 Program: MAF2
 Module: mmiInfoImage
 Authors: Paolo Quadrani
 
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


#include "mmiInfoImage.h"

#include "mafView.h"
#include "mafViewCompound.h"
#include "mafDeviceButtonsPadTracker.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafAvatar3D.h"
#include "mafInteractor6DOFCameraMove.h"
#include "mafInteractorCameraMove.h"

#include "mafEventBase.h"
#include "mafEventInteraction.h"

#include "mafVME.h"

#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include <assert.h>
#include "vtkFloatArray.h"
#include "vtkDataArray.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mmiInfoImage)
//------------------------------------------------------------------------------
mmiInfoImage::mmiInfoImage()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mmiInfoImage::~mmiInfoImage()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void mmiInfoImage::OnEvent(mafEventBase *event)
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
    else
    {
      // Send to status bar text containing the pixel picked position and its density value
      mafDeviceButtonsPadMouse *mouse = mafDeviceButtonsPadMouse::SafeDownCast(device);
      if (mouse)
      {
        if (!mouse->IsUpdateRWIDuringMotion())
        {
          mouse->UpdateRWIDuringMotionOn();
        }
        else
        {
          double pos[2];
          mouse->GetLastPosition(pos);
          mafView *v = mouse->GetView();
          if (v)
          {
            mafViewCompound *vc = mafViewCompound::SafeDownCast(v);
            if (vc)
            {
              v = vc->GetSubView();
            }
            mafString info;
            info = "";
            if(v->Pick((int)pos[0], (int)pos[1]))
            {
              mafVME *picked_vme = v->GetPickedVme();
              vtkDataSet *data = picked_vme->GetOutput()->GetVTKData();
              if (data->IsA("vtkImageData") || data->IsA("vtkRectilinearGrid") && m_Renderer->GetActiveCamera()->GetParallelProjection())
              {
                double picked_pos[3], iso_value;
                v->GetPickedPosition(picked_pos);
                int pid = data->FindPoint(picked_pos);
                vtkDataArray *scalars = data->GetPointData()->GetScalars();
                scalars->GetTuple(pid,&iso_value);
                info << "x = " << (int)picked_pos[0] << " y = " << (int)picked_pos[1] << " z = " << (int)picked_pos[2] << " d = " << iso_value;
                mafEventMacro(mafEvent(this,PROGRESSBAR_SET_TEXT,&info));
              }
            }
            mafEventMacro(mafEvent(this,PROGRESSBAR_SET_TEXT,&info));
          }
        }
      }
    }
  }
  // Make the superclass to manage StartInteractionEvent
  // and StopInteractionEvent: this will make OnStart/StopInteraction()
  // to be called, or eventually the event to be forwarded.
  Superclass::OnEvent(event);
}
