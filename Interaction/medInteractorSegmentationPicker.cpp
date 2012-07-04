/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medInteractorSegmentationPicker.cpp,v $
Language:  C++
Date:      $Date: 2012-04-19 12:47:51 $
Version:   $Revision: 1.1.2.6 $
Authors:   Matteo Giacomoni, Gianluigi Crimi
==========================================================================
Copyright (c) 2010 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "medDefines.h"
#include "medInteractorSegmentationPicker.h"

#include "mafView.h"
#include "mafDeviceButtonsPadTracker.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafAvatar3D.h"

#include "mafEventInteraction.h"

#include "mafRWIBase.h"
#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafTransform.h"

#include "vtkMAFSmartPointer.h"
#include "vtkCellPicker.h"
#include "vtkMAFRayCast3DPicker.h"
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
mafCxxTypeMacro(medInteractorSegmentationPicker)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
medInteractorSegmentationPicker::medInteractorSegmentationPicker()
//------------------------------------------------------------------------------
{
  m_IsPicking = false;
  m_FullModifiersMode = false;
}

//------------------------------------------------------------------------------
medInteractorSegmentationPicker::~medInteractorSegmentationPicker()
//------------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medInteractorSegmentationPicker::OnLeftButtonDown(mafEventInteraction *e)
//----------------------------------------------------------------------------
{
  Superclass::OnLeftButtonDown(e);
  if (mafDeviceButtonsPadTracker *tracker=mafDeviceButtonsPadTracker::SafeDownCast((mafDevice *)e->GetSender()))
  { // is it a tracker?
    mafMatrix *tracker_pose = e->GetMatrix();
    // extract device avatar's renderer, no avatar == no picking
    mafAvatar *avatar = tracker->GetAvatar();
    if (avatar)
    {
      // compute pose in the world frame
      mafMatrix world_pose;
      mafAvatar3D *avatar3D=mafAvatar3D::SafeDownCast(avatar);
      if (avatar3D)
        avatar3D->TrackerToWorld(*tracker_pose,world_pose,mafAvatar3D::CANONICAL_TO_WORLD_SCALE);
      else
        world_pose = *tracker_pose;
      SendPickingInformation(avatar->GetView(),NULL,VME_PICKED,&world_pose,false);
    }
  }
  else if (mafDeviceButtonsPadMouse *mouse=mafDeviceButtonsPadMouse::SafeDownCast((mafDevice *)e->GetSender()))
  { 
    if (!m_FullModifiersMode && e->GetButton() == MAF_LEFT_BUTTON && e->GetModifiers() == 0 || m_FullModifiersMode && e->GetModifier(MAF_CTRL_KEY) && e->GetButton() == MAF_LEFT_BUTTON)
    {
	    double mouse_pos[2];
	    e->Get2DPosition(mouse_pos);
	    SendPickingInformation(mouse->GetView(), mouse_pos);
    }
    else if (!m_FullModifiersMode && e->GetModifier(MAF_CTRL_KEY) && e->GetButton() == MAF_LEFT_BUTTON || m_FullModifiersMode && e->GetModifier(MAF_ALT_KEY) && e->GetButton() == MAF_LEFT_BUTTON)
    {
      double mouse_pos[2];
      e->Get2DPosition(mouse_pos);
      SendPickingInformation(mouse->GetView(), mouse_pos, VME_ALT_PICKED);
    }
  }
  m_IsPicking = true;
}

//----------------------------------------------------------------------------
void medInteractorSegmentationPicker::OnLeftButtonUp() 
//----------------------------------------------------------------------------
{
  Superclass::OnLeftButtonUp();
  m_IsPicking = false;
}
//----------------------------------------------------------------------------
void medInteractorSegmentationPicker::SendPickingInformation(mafView *v, double *mouse_pos, int msg_id, mafMatrix *tracker_pos, bool mouse_flag)
//----------------------------------------------------------------------------
{
  bool picked_something = false;

  vtkCellPicker *cellPicker;
  vtkNEW(cellPicker);
  cellPicker->SetTolerance(0.001);
  if (v)
  {
    if(mouse_flag)
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
    }
    else
      picked_something = v->Pick(*tracker_pos);
    if (picked_something)
    {
      vtkPoints *p = vtkPoints::New();
      double pos_picked[3];
      cellPicker->GetPickPosition(pos_picked);
      p->SetNumberOfPoints(1);
      p->SetPoint(0,pos_picked);
      double scalar_value = 0;
      mafVME *pickedVME = v->GetPickedVme();
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
        mafEvent pick_event(this,msg_id,p);
        pick_event.SetArg(pid);
        mafEventMacro(pick_event);
        p->Delete();
      }
    }
  }
  vtkDEL(cellPicker);
}


//------------------------------------------------------------------------------
void medInteractorSegmentationPicker::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  Superclass::OnEvent(event);
  mafEventInteraction *e = (mafEventInteraction *)event;

  if ( m_IsPicking && (!m_FullModifiersMode && e->GetButton() == MAF_LEFT_BUTTON && e->GetModifiers() == 0 || m_FullModifiersMode && e->GetModifier(MAF_CTRL_KEY) && e->GetButton() == MAF_LEFT_BUTTON || !m_FullModifiersMode && e->GetModifier(MAF_CTRL_KEY) && e->GetButton() == MAF_LEFT_BUTTON || m_FullModifiersMode && e->GetModifier(MAF_ALT_KEY) && e->GetButton() == MAF_LEFT_BUTTON))
  {
    if (mafDeviceButtonsPadMouse *mouse=mafDeviceButtonsPadMouse::SafeDownCast((mafDevice *)event->GetSender()))
    { 
      double mouse_pos[2];
      e->Get2DPosition(mouse_pos);
      SendPickingInformation(mouse->GetView(), mouse_pos,VME_PICKING);
    }
  }
}