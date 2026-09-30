/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmiInfoImage
 Authors: Paolo Quadrani
 
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


#include "mmiInfoImage.h"

#include "albaView.h"
#include "albaViewCompound.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaAvatar3D.h"
#include "albaInteractor6DOFCameraMove.h"
#include "albaInteractorCameraMove.h"

#include "albaEventBase.h"
#include "albaEventInteraction.h"

#include "albaVME.h"

#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include <assert.h>

//------------------------------------------------------------------------------
albaCxxTypeMacro(mmiInfoImage)
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
void mmiInfoImage::OnEvent(albaEventBase *event)
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
      // Send to status bar text containing the pixel picked position and its density value
      albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::SafeDownCast(device);
			if (mouse)
			{

				double pos[2];
				mouse->GetLastPosition(pos);
				albaView *v = mouse->GetView();
				if (v)
				{
					albaViewCompound *vc = albaViewCompound::SafeDownCast(v);
					if (vc)
					{
						v = vc->GetSubView();
					}
					albaString info;
					info = "";
					if (v->Pick((int)pos[0], (int)pos[1]))
					{
						albaVME *picked_vme = v->GetPickedVme();
						vtkDataSet *data = picked_vme->GetOutput()->GetVTKData();
						if (data->IsA("vtkImageData") || data->IsA("vtkRectilinearGrid") && m_Renderer->GetActiveCamera()->GetParallelProjection())
						{
							double picked_pos[3], iso_value;
							v->GetPickedPosition(picked_pos);
							int pid = data->FindPoint(picked_pos);
							vtkDataArray *scalars = data->GetPointData()->GetScalars();
							scalars->GetTuple(pid, &iso_value);
							info << "x = " << (int)picked_pos[0] << " y = " << (int)picked_pos[1] << " z = " << (int)picked_pos[2] << " d = " << iso_value;
							albaEventMacro(albaEvent(this, PROGRESSBAR_SET_TEXT, &info));
						}
					}
					albaEventMacro(albaEvent(this, PROGRESSBAR_SET_TEXT, &info));
				}

			}
    }
  }
  // Make the superclass to manage StartInteractionEvent
  // and StopInteractionEvent: this will make OnStart/StopInteraction()
  // to be called, or eventually the event to be forwarded.
  Superclass::OnEvent(event);
}
