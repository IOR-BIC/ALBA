/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiPicker.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:34:58 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone 
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows
#ifdef __GNUG__
    #pragma implementation "mmiPicker.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "mmiPicker.h"

#include "mafInteractionDecl.h"
#include "mafEvent.h"

#include "mmdTracker.h"
#include "mmdMouse.h"
#include "mafAvatar3D.h"

#include "mafVmeData.h"

#include "mflEventInteraction.h"

#include "mflVME.h"
#include "mflTransform.h"

#include "vtkCellPicker.h"
#include "vtkRayCast3DPicker.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"

#include <assert.h>

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vtkStandardNewMacro(mmiPicker)
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
void mmiPicker::OnButtonDown(mflEventInteraction *e)
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mmiPicker::OnButtonUp(mflEventInteraction *e) 
//----------------------------------------------------------------------------
{
  if (mmdTracker *tracker=mmdTracker::SafeDownCast((mafDevice *)e->GetSender()))
  { // is it a tracker?
    
    mflMatrix *tracker_pose = e->GetMatrix();

    // extract device avatar's renderer, no avatar == no picking
    mafAvatar *avatar = tracker->GetAvatar();
    if (avatar)
    {
      float pos_picked[3];
      int flag=avatar->Pick(tracker_pose);
      if (flag) 
      {
        // find picked position 
        vtkAbstractPropPicker *picker = avatar->GetPicker();
        picker->GetPickPosition(pos_picked);
		  }
      else if (mafAvatar3D *avatar3D=mafAvatar3D::SafeDownCast(avatar))
      {
        // in case of 3D avatar return 3D position
        mflTransform::GetPosition(avatar3D->GetLastPoseMatrix(),pos_picked);
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
    
    vtkAbstractPropPicker *picker = mouse->GetPicker();

    //modified by Stefano 19-1-2005 (begin)
    vtkRenderer *r = NULL;
    vtkRendererCollection *rc = mouse->GetInteractor()->GetRenderWindow()->GetRenderers();
    
    assert(rc);
    rc->InitTraversal();
    while(r = rc->GetNextItem())
    {
      if(picker->Pick(mouse_pos[0],mouse_pos[1],0,r))
      {
        float pos_picked[3];
        picker->GetPickPosition(pos_picked);
        //mouse->GetPicker()->GetPickPosition(pos_picked);
        vtkPoints *p = vtkPoints::New();
        p->SetNumberOfPoints(1);
        p->SetPoint(0,pos_picked);
        mafEventMacro(mafEvent(this,VME_PICKED,p));
        p->Delete();
      }
    //picker->Delete();
    }
    //modified by Stefano 19-1-2005 (end)
  }

}
