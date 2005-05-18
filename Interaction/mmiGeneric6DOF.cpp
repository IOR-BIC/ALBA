/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiGeneric6DOF.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-18 17:29:06 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// To be included first because of wxWindows
#ifdef __GNUG__
    #pragma implementation "mmi6DOF.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "mafEvent.h"
#include "mafInteractionDecl.h"

#include "mmiGeneric6DOF.h"
#include "mmdTracker.h"
#include "mafAvatar3D.h"

#include "vtkObjectFactory.h"
#include "mflEventInteraction.h"
#include "mflDefines.h"
#include "mflTransform.h"
#include "mflCameraTransform.h"
#include "mflVME.h"
#include <assert.h>

//------------------------------------------------------------------------------
vtkStandardNewMacro(mmiGeneric6DOF)
//------------------------------------------------------------------------------
mmiGeneric6DOF::mmiGeneric6DOF()
{
  IgnoreTriggerEvents = 0;
  DifferentialMoving  = 1;
  vtkNEW(OutputTransform);
  mflSmartPointer<mflMatrix> outmatrix;
  OutputTransform->SetMatrix(outmatrix,false); // force using an mflMatrix for the interaction event
}

//------------------------------------------------------------------------------
mmiGeneric6DOF::~mmiGeneric6DOF()
{
  vtkDEL(OutputTransform);
}

//------------------------------------------------------------------------------
void mmiGeneric6DOF::OnMove(mflEventInteraction *e)
//------------------------------------------------------------------------------
{
  // if the tracker does not have an avatar we don't
  // know the mapping rule and cannot proceed
  
  if (Avatar)
	{
    mflMatrix *tracker_pose=e->GetMatrix();
    SetTrackerPoseMatrix(tracker_pose);

    Update();
    
    if (GetResultTransform())
		{
      ResultTransform->SetMatrix(VME->GetAbsMatrix());
      GetResultTransform()->Concatenate(OutputTransform,POST_MULTIPLY);
    }

    // forward the event with the pose matrix through default channel
    mflSMART(mflEventInteraction,e)(MoveActionEvent,this,(mflMatrix *)OutputTransform->GetMatrix());
    InvokeEvent(e);

    // send also a mafEvent
    mafEvent ev;
    ev.SetSender(this);
    ev.SetId(ID_TRANSFORM);
    ev.SetMatrix(OutputTransform->GetMatrix());
    mafEventMacro(ev);
		return;
  }
}

//------------------------------------------------------------------------------
void mmiGeneric6DOF::ProcessEvent(mflEvent *event,mafID channel)
//------------------------------------------------------------------------------
{
  assert(event);
  assert(event->GetSender());
  
  unsigned int id=event->GetID();

  if (channel==mafDevice::DeviceInputChannel && GetTracker())
  {
    // if the event comes from tracker which started the interaction continue...
    if (id==mmdTracker::Move3DEvent && event->GetSender()==GetTracker())
    {
      mflEventInteraction *e=(mflEventInteraction *)event;        
      OnMove(e);
    }
  }
  Superclass::ProcessEvent(event,channel);
}

//------------------------------------------------------------------------------
void mmiGeneric6DOF::Update()
//------------------------------------------------------------------------------
{
  if (DifferentialMoving == 1)
	{
		// not yet implemented should not be here
		this->UpdateDeltaTransform();
    
    // remove the scaling factor...
    DeltaTransform->GetOrientation(OutputOrientation);
    DeltaTransform->GetPosition(OutputPosition);

    // this simply remove anything but orientation and translation...
    // Can I use polar decomposition for this? Is it more precise?
    // How does it performs?
    OutputTransform->SetOrientation(OutputOrientation);
    OutputTransform->SetPosition(OutputPosition);
    
    double pivot_position[3];
    mflTransform::GetPosition(GetPivotRefSys()->GetMatrix(),pivot_position);

    // Rotate around the rotational constraint ref sys center
    OutputTransform->Translate(-pivot_position[0],-pivot_position[1],-pivot_position[2],PRE_MULTIPLY);
    OutputTransform->Translate(pivot_position,POST_MULTIPLY);

    // Store current pose matrix
    TrackerSnapshot(TrackerPoseMatrix);
  }
	else
	{
    if (Avatar)
      Avatar->TrackerToWorld(TrackerPoseMatrix,(mflMatrix *)OutputTransform->GetMatrix(),false);
  }
}

