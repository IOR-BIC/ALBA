/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiPicker.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-16 13:35:49 $
  Version:   $Revision: 1.7 $
  Authors:   Marco Petrone 
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


#include "mmiPicker.h"
#include "mafEvent.h"

#include "mafView.h"
#include "mafViewCompound.h"
#include "mmdTracker.h"
#include "mmdMouse.h"
#include "mafAvatar3D.h"

#include "mafEventInteraction.h"

#include "mafVME.h"
#include "mafTransform.h"

#include "vtkMAFSmartPointer.h"
#include "vtkCellPicker.h"
#include "vtkRayCast3DPicker.h"
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
mafCxxTypeMacro(mmiPicker)
//------------------------------------------------------------------------------
mmiPicker::mmiPicker()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mmiPicker::~mmiPicker()
//------------------------------------------------------------------------------
{
}


//----------------------------------------------------------------------------
void mmiPicker::OnButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mmiPicker::OnButtonUp(mafEventInteraction *e) 
//----------------------------------------------------------------------------
{
  if (mmdTracker *tracker=mmdTracker::SafeDownCast((mafDevice *)e->GetSender()))
  { // is it a tracker?
    mafMatrix *tracker_pose = e->GetMatrix();
    // extract device avatar's renderer, no avatar == no picking
    mafAvatar *avatar = tracker->GetAvatar();
    if (avatar)
    {
      mafView *v = avatar->GetView();
      if (v)
      {
        // compute pose in the world frame
        mafMatrix world_pose;
        mafAvatar3D *avatar3D=mafAvatar3D::SafeDownCast(avatar);
        if (avatar3D)
        {
          avatar3D->TrackerToWorld(*tracker_pose,world_pose,mafAvatar3D::CANONICAL_TO_WORLD_SCALE);
        }
        else
          world_pose = *tracker_pose;
        if (v->Pick(world_pose))
        {
          SendPickingInformation(v);
        }
        else if (avatar3D)
        {
          // in case of 3D avatar return 3D position
          double pos_picked[3];
          mafTransform::GetPosition(avatar3D->GetLastPoseMatrix(),pos_picked);
        }
      }
    }
  }
  else if (mmdMouse *mouse=mmdMouse::SafeDownCast((mafDevice *)e->GetSender()))
  { 
    double tmp_pose[2];
    int mouse_pos[2];
    e->Get2DPosition(tmp_pose);
    mouse_pos[0] = (int)tmp_pose[0];
    mouse_pos[1] = (int)tmp_pose[1];

    mafView *v = mouse->GetView();
    if (v)
    {
      mafViewCompound *vc = mafViewCompound::SafeDownCast(v);
      if (vc)
      {
        v = vc->GetSubView();
      }
      if(v->Pick(mouse_pos[0],mouse_pos[1]))
      {
        SendPickingInformation(v);
      }
    }
  }
}
//----------------------------------------------------------------------------
void mmiPicker::SendPickingInformation(mafView *v)
//----------------------------------------------------------------------------
{
  double pos_picked[3];
  v->GetPickedPosition(pos_picked);
  vtkPoints *p = vtkPoints::New();
  p->SetNumberOfPoints(1);
  p->SetPoint(0,pos_picked);

  double scalar_value = 0;
  mafVME *pickedVME = v->GetPickedVme();
  vtkDataSet *vtk_data = pickedVME->GetOutput()->GetVTKData();
  vtk_data->SetUpdateExtentToWholeExtent();
  vtk_data->Update();
  int pid = vtk_data->FindPoint(pos_picked);
  vtkDataArray *scalars = vtk_data->GetPointData()->GetScalars();
  if (scalars)
  {
    scalars->GetTuple(pid,&scalar_value);
  }
  mafEvent pick_event(this,VME_PICKED,p);
  pick_event.SetDouble(scalar_value);
  mafEventMacro(pick_event);
  p->Delete();
}
