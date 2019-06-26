/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorGeneric6DOF
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaEvent.h"

#include "albaInteractorGeneric6DOF.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaAvatar3D.h"
#include "albaRefSys.h"

#include "albaEventInteraction.h"
#include "albaTransform.h"
#include "albaCameraTransform.h"
#include "albaVME.h"

#include <assert.h>

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractorGeneric6DOF)
//------------------------------------------------------------------------------
albaInteractorGeneric6DOF::albaInteractorGeneric6DOF()
{
  m_IgnoreTriggerEvents = 0;
  m_DifferentialMoving  = 1;
  albaNEW(m_OutputTransform);
}

//------------------------------------------------------------------------------
albaInteractorGeneric6DOF::~albaInteractorGeneric6DOF()
{
  albaDEL(m_OutputTransform);
}

//------------------------------------------------------------------------------
void albaInteractorGeneric6DOF::OnMove(albaEventInteraction *e)
//------------------------------------------------------------------------------
{
  // if the tracker does not have an avatar we don't
  // know the mapping rule and cannot proceed
  
  if (m_Avatar)
	{
    albaMatrix *tracker_pose=e->GetMatrix();
    SetTrackerPoseMatrix(tracker_pose);

    Update();
    
    if (GetResultTransform())
    {
      m_ResultTransform->SetMatrix(*m_VME->GetOutput()->GetAbsMatrix());
      GetResultTransform()->Concatenate(m_OutputTransform,POST_MULTIPLY);
    }

    // forward the event with the pose matrix through default channel
    albaEventMacro(albaEventInteraction(this,ID_TRANSFORM,m_OutputTransform->GetMatrixPointer()));
  }
}

//------------------------------------------------------------------------------
void albaInteractorGeneric6DOF::OnEvent(albaEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);
  assert(event->GetSender());
  
  albaID id=event->GetId();
  albaID channel=event->GetChannel();

  if (channel==MCH_INPUT && GetTracker())
  {
    // if the event comes from tracker which started the interaction continue...
    if (id==albaDeviceButtonsPadTracker::GetTracker3DMoveId() && event->GetSender()==GetTracker())
    {
      albaEventInteraction *e=albaEventInteraction::SafeDownCast(event);
      assert(e);        
      OnMove(e);
    }
  }
  Superclass::OnEvent(event);
}

//------------------------------------------------------------------------------
void albaInteractorGeneric6DOF::Update()
//------------------------------------------------------------------------------
{
  if (m_DifferentialMoving == 1)
  {
    // not yet implemented should not be here
    UpdateDeltaTransform();
    
    // remove the scaling factor...
    m_DeltaTransform->GetOrientation(m_OutputOrientation);
    m_DeltaTransform->GetPosition(m_OutputPosition);

    // this simply remove anything but orientation and translation...
    // Can I use polar decomposition for this? Is it more precise?
    // How does it performs?
    m_OutputTransform->SetOrientation(m_OutputOrientation);
    m_OutputTransform->SetPosition(m_OutputPosition);
    
    double pivot_position[3];
    albaTransform::GetPosition(*GetPivotRefSys()->GetMatrix(),pivot_position);

    // Rotate around the rotational constraint ref sys center
    m_OutputTransform->Translate(-pivot_position[0],-pivot_position[1],-pivot_position[2],PRE_MULTIPLY);
    m_OutputTransform->Translate(pivot_position,POST_MULTIPLY);

    // Store current pose matrix
    TrackerSnapshot(m_TrackerPoseMatrix);
  }
	else
	{
    if (m_Avatar)
    {  
      albaMatrix outmatrix;
      m_Avatar->TrackerToWorld(*m_TrackerPoseMatrix,outmatrix,false);
      m_OutputTransform->SetMatrix(outmatrix);
    }
  }
}

