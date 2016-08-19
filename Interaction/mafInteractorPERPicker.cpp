/*=========================================================================

 Program: MAF2
 Module: mafInteractorPERPicker
 Authors: Nicola Vanella
 
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

#include "mafInteractorPERPicker.h"

#include "mafView.h"
#include "mafVME.h"
#include "mafObject.h"
#include "mafMatrix.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafDeviceButtonsPadTracker.h"
#include "mafAvatar.h"
#include "mafEventInteraction.h"
#include "mafRWIBase.h"

#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkDataSet.h"
#include "vtkIntArray.h"
#include "vtkDataSetAttributes.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkCellPicker.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafInteractorPERPicker)
//------------------------------------------------------------------------------
mafInteractorPERPicker::mafInteractorPERPicker() :
	mafInteractorPER()
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
mafInteractorPERPicker::~mafInteractorPERPicker()
//------------------------------------------------------------------------------
{}

//----------------------------------------------------------------------------
void mafInteractorPERPicker::OnButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  mafDevice *device = mafDevice::SafeDownCast((mafDevice*)e->GetSender());

  mafView       *v = NULL;
  mafVME        *picked_vme  = NULL;
  vtkProp3D     *picked_prop = NULL;
  mafInteractor *picked_bh   = NULL;
  mafMatrix     point_pose;
  double        pos_2d[2];

  mafDeviceButtonsPadMouse   *mouse   = mafDeviceButtonsPadMouse::SafeDownCast(device);
  mafDeviceButtonsPadTracker *tracker = mafDeviceButtonsPadTracker::SafeDownCast(device);

  if (tracker)
  {
    point_pose = *e->GetMatrix();
    mafAvatar *avatar = tracker->GetAvatar();
    if (avatar)
    {
      v = avatar->GetView();
    }
  }
  else if (mouse)
  {
    e->Get2DPosition(pos_2d);
    point_pose.Identity();
    point_pose.SetElement(0,3,pos_2d[0]);
    point_pose.SetElement(1,3,pos_2d[1]);
    v = mouse->GetView();
  }

  if(v && v->FindPokedVme(device, point_pose, picked_prop, picked_vme, picked_bh))
  {
    SetPickedVME(device, picked_vme);
    // if a VME is picked its pointer is written in PickedVME
    int but_down_id = e->GetId();
    if (but_down_id == mafDeviceButtonsPadMouse::GetMouseDClickId() && !picked_vme->IsA("mafVMEGizmo"))
    {
      // Send event to inform Logic that a double click event is rised on a VME
      InvokeEvent(VME_DCLICKED,MCH_UP,picked_vme);
    }
    else if(m_CanSelect && !picked_vme->IsA("mafVMEGizmo"))
    {
      // Send a VME select event to Logic
      InvokeEvent(VME_SELECT,MCH_UP,picked_vme);
    }
  }
  // Forward the start event to the right behavior
  if(picked_vme && !picked_vme->IsA("mafVMEGizmo"))
  {
		double mouse_pos[2];
		e->Get2DPosition(mouse_pos);

		//SendPickingInformation(v, NULL, VME_PICKED, &world_pose, false);
		SendPickingInformation(mouse->GetView(), mouse_pos);
  }
	else if (picked_bh)
	{
		// if a vme with a behavior has been picked... 
		picked_bh->SetVME(picked_vme);   // set the VME (Marco: to be removed, the operation should set the VME to the interactor!) 
		picked_bh->SetProp(picked_prop); // set the prop (Marco: to be removed, no access to the vtkProp!!!) 
		picked_bh->OnEvent(e); // forward the start event to picked behavior
	}

  // if I don't picked a VME or I picked but I cannot select, move the camera.
  else if (tracker && m_CameraBehavior&&(!m_CameraBehavior->IsInteracting()))
  {
    // if the camera behavior is free...
    m_CameraBehavior->OnEvent(e); // forward to the camera behavior
  }
  // if I don't picked a VME or I picked but I cannot select, move the camera.
  else if (mouse && m_CameraMouseBehavior&&(!m_CameraMouseBehavior->IsInteracting()))
  {
    // if the camera behavior is free...
    m_CameraMouseBehavior->OnEvent(e); // forward to the camera behavior
  }
}

//----------------------------------------------------------------------------
void mafInteractorPERPicker::SendPickingInformation(mafView *v, double *mouse_pos, int msg_id, mafMatrix *tracker_pos, bool mouse_flag)
//----------------------------------------------------------------------------
{
	bool picked_something = false;

	vtkCellPicker *cellPicker;
	vtkNEW(cellPicker);
	cellPicker->SetTolerance(0.001);
	if (v)
	{
		if (mouse_flag)
		{
			vtkRendererCollection *rc = v->GetRWI()->GetRenderWindow()->GetRenderers();
			vtkRenderer *r = NULL;
			rc->InitTraversal();
			while (r = rc->GetNextItem())
			{
				if (cellPicker->Pick(mouse_pos[0], mouse_pos[1], 0, r))
				{
					picked_something = true;
				}
			}
		}
		else
			picked_something = v->Pick(*tracker_pos);

		if (picked_something)
		{
			vtkPoints *p = vtkPoints::New();
			double pos_picked[3];
			cellPicker->GetPickPosition(pos_picked);
			p->SetNumberOfPoints(1);
			p->SetPoint(0, pos_picked);
			double scalar_value = 0;
			mafVME *pickedVME = v->GetPickedVme();

			if (pickedVME)
			{
				vtkDataSet *vtk_data = pickedVME->GetOutput()->GetVTKData();
				int pid = vtk_data->FindPoint(pos_picked);
				vtkDataArray *scalars = vtk_data->GetPointData()->GetScalars();
				if (scalars)
					scalars->GetTuple(pid, &scalar_value);
				mafEvent pick_event(this, msg_id, p);
				pick_event.SetDouble(scalar_value);
				pick_event.SetArg(pid);
				pick_event.SetVme(pickedVME);
				mafEventMacro(pick_event);
				p->Delete();
			}
		}
	}
	vtkDEL(cellPicker);
}

