/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiPicker.cpp,v $
  Language:  C++
  Date:      $Date: 2005-10-22 12:45:30 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone 
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mmiPicker.h"


#include "mafEvent.h"

#include "mafViewVTK.h"
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
      double pos_picked[3];
      int flag=avatar->Pick(*tracker_pose);
      if (flag) 
      {
        // find picked position 
        vtkAbstractPropPicker *picker = avatar->GetPicker();
        picker->GetPickPosition(pos_picked);
		  }
      else if (mafAvatar3D *avatar3D=mafAvatar3D::SafeDownCast(avatar))
      {
        // in case of 3D avatar return 3D position
        mafTransform::GetPosition(avatar3D->GetLastPoseMatrix(),pos_picked);
      }
      
      vtkPoints *p = vtkPoints::New();
		  p->SetNumberOfPoints(1);
		  p->SetPoint(0,pos_picked);
		  mafEventMacro(mafEvent(this,VME_PICKED,p));
		  p->Delete();
    }
  }
  else if (mmdMouse *mouse=mmdMouse::SafeDownCast((mafDevice *)e->GetSender()))
  { 
    double tmp_pose[2];
    int mouse_pos[2];
    e->Get2DPosition(tmp_pose);
    mouse_pos[0] = (int)tmp_pose[0];
    mouse_pos[1] = (int)tmp_pose[1];

    mafViewVTK *v = mafViewVTK::SafeDownCast(mouse->GetView());
    if (v)
    {
      if(v->Pick(mouse_pos[0],mouse_pos[1]))
      {
        double pos_picked[3];
        v->GetPickedPosition(pos_picked);
        vtkMAFSmartPointer<vtkPoints> p;
        p->SetNumberOfPoints(1);
        p->SetPoint(0,pos_picked);
        mafEventMacro(mafEvent(this,VME_PICKED,p.GetPointer()));
      }
    }
  }
}

