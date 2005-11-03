/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiInfoImage.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-03 08:59:27 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
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
#include "mmdTracker.h"
#include "mmdMouse.h"
#include "mafAvatar3D.h"
#include "mmi6DOFCameraMove.h"
#include "mmiCameraMove.h"

#include "mafEventBase.h"
#include "mafEventInteraction.h"

#include "mafVME.h"

#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include <assert.h>

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

    if (id == mmdMouse::MOUSE_CHAR_EVENT && !IsInteracting(device))
    {
      mafEventInteraction *e = mafEventInteraction::SafeDownCast(event);
      OnChar(e);
    }
    // find if this device is one of those currently interacting
    if (IsInteracting(device))
    {
      // process the Move event
      if (id == mmdTracker::TRACKER_3D_MOVE || id == mmdMouse::MOUSE_2D_MOVE)
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
      mmdMouse *mouse = mmdMouse::SafeDownCast(device);
      if (mouse)
      {
        double pos[2];
        mouse->GetLastPosition(pos);
        mafView *v = mouse->GetView();
        if (v)
        {
          mafViewCompound *vc = mafViewCompound::SafeDownCast(v);
          if (vc)
          {
            v = vc->GetSubView(mouse->GetRWI());
          }
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
              mafString info;
              info = "";
              info << "x = " << picked_pos[0] << " y = " << picked_pos[1] << " d = " << iso_value;
              picked_vme->ForwardUpEvent(mafEvent(this,PROGRESSBAR_SET_TEXT,&info));
            }
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
