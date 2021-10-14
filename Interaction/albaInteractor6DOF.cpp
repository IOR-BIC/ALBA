/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractor6DOF
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaInteractor6DOF.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaAvatar3D.h"

#include "albaMatrix.h"
#include "albaEventBase.h"
#include "albaTransform.h"
#include "albaCameraTransform.h"
#include "albaEventInteraction.h"
#include "albaOBB.h"

#include "vtkMath.h"

#include <assert.h>

//------------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaInteractor6DOF)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaInteractor6DOF::albaInteractor6DOF()
//------------------------------------------------------------------------------
{
  vtkNEW(m_TrackerPoseMatrix);
  vtkNEW(m_InverseTrackerPoseMatrix);
  vtkNEW(m_StartTrackerPoseMatrix);
  vtkNEW(m_TmpTransform);
  vtkNEW(m_DeltaTransform);
    
  m_Avatar = NULL;
}

//------------------------------------------------------------------------------
albaInteractor6DOF::~albaInteractor6DOF()
//------------------------------------------------------------------------------
{
  vtkDEL(m_TrackerPoseMatrix);
  vtkDEL(m_InverseTrackerPoseMatrix);
  vtkDEL(m_StartTrackerPoseMatrix);
  vtkDEL(m_TmpTransform);
  vtkDEL(m_DeltaTransform);
}



//------------------------------------------------------------------------------
void albaInteractor6DOF::SetTracker(albaDeviceButtonsPadTracker *tracker)
//------------------------------------------------------------------------------
{
  SetDevice(tracker);
}

//------------------------------------------------------------------------------
void albaInteractor6DOF::HideAvatar()
//------------------------------------------------------------------------------
{
  albaDeviceButtonsPadTracker *tracker=GetTracker();
  if (tracker)
  {
    albaAvatar *avatar=tracker->GetAvatar();
    if (avatar)
    {
      avatar->Hide();
    }
  }
}

//------------------------------------------------------------------------------
void albaInteractor6DOF::ShowAvatar()
//------------------------------------------------------------------------------
{
  albaDeviceButtonsPadTracker *tracker=GetTracker();
  if (tracker)
  {
    albaAvatar *avatar=tracker->GetAvatar();
    if (avatar)
    {
      avatar->Show();
    }
  }
}

//------------------------------------------------------------------------------
int albaInteractor6DOF::StartInteraction(albaDeviceButtonsPadTracker *tracker,albaMatrix *pose)
//------------------------------------------------------------------------------
{
  if (Superclass::StartInteraction(tracker))
  {
    if (pose)
      TrackerSnapshot(pose);
    else
    {
      m_StartTrackerPoseMatrix->Identity();
      m_StartTrackerPoseMatrix->SetTimeStamp(0);
    }
    
    m_Avatar = albaAvatar3D::SafeDownCast(GetTracker()->GetAvatar());
    // albaInteractor6DOF works only with avatar of type albaAvatar3D
    if (m_Avatar)
    {
      // store the current renderer: camera cannot be changed during interaction!!!
			albaAvatar * avatar = tracker->GetAvatar();
      SetRendererAndView(avatar->GetRenderer(),avatar->GetView());
      InvokeEvent(this,INTERACTION_STARTED,MCH_UP,m_StartTrackerPoseMatrix);
      return true;
    }
    // if wrong type of avatar force unlock device and stop interaction 
    Superclass::StopInteraction(tracker); 
  }
  return false;
}

//------------------------------------------------------------------------------
int albaInteractor6DOF::StopInteraction(albaDeviceButtonsPadTracker *tracker,albaMatrix *pose)
//------------------------------------------------------------------------------
{
  if (Superclass::StopInteraction(tracker))
  {
    // ...do something
    InvokeEvent(this,INTERACTION_STOPPED);
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
void albaInteractor6DOF::SetTrackerPoseMatrix(albaMatrix *pose)
//------------------------------------------------------------------------------
{
  assert(pose);
  m_TrackerPoseMatrix->DeepCopy(pose);
}

// Raw Pose
//------------------------------------------------------------------------------
void albaInteractor6DOF::TrackerSnapshot(albaMatrix *pose)
//------------------------------------------------------------------------------
{
  assert(pose);
  m_StartTrackerPoseMatrix->DeepCopy(pose);
  albaMatrix::Invert(*m_StartTrackerPoseMatrix,*m_InverseTrackerPoseMatrix);
}

//------------------------------------------------------------------------------
void albaInteractor6DOF::UpdateDeltaTransform()
//------------------------------------------------------------------------------
{
  if (m_Avatar)
  {
    assert(m_Device);
    m_TmpTransform->SetMatrix(*m_TrackerPoseMatrix);
    m_TmpTransform->Concatenate(*m_InverseTrackerPoseMatrix,POST_MULTIPLY);

    m_DeltaTransform->Identity();
    if (m_RotationFlag) // if rotation is enabled copy rotation
      m_DeltaTransform->CopyRotation(m_TmpTransform->GetMatrix());

    if (m_TranslationFlag) // if translation is enabled copy translation
      m_DeltaTransform->CopyTranslation(m_TmpTransform->GetMatrix());

    // Rotate and Scale according to calibration matrixes
    double wscale[3];
    double wbscale[3];

    // Scale the m_DeltaTransform accordingly
    GetTracker()->GetTrackerToCanonicalTransform()->GetScale(wscale);
  
    // @todo change here to map to current constrain ref sys
    albaTransform::GetScale(m_Avatar->GetCanonicalToWorldTransform()->GetMatrix(), wbscale);
  
    m_DeltaTransform->Scale(wscale[0],wscale[1],wscale[2],POST_MULTIPLY);
    m_DeltaTransform->Scale(wbscale[0],wbscale[1],wbscale[2],POST_MULTIPLY);
  
    // compute the initial tracker pose in world coordinate, do it each iteration
    // to take in consideration possible changes in the two trasformation (tracker 
    // to canonical and canonical to world; e.g. when i move the camera the canonical 
    // to world changes accordingly)
    m_TmpTransform->SetMatrix(*m_StartTrackerPoseMatrix);

    GetTracker()->TrackerToCanonical(m_TmpTransform);

    // @todo change here to map to current constrain ref sys
    m_Avatar->CanonicalToWorld(m_TmpTransform); // map according to current transformations

    // Use of the initial matrix pose of the tracker in the world coordinate system, but 
    // extract the rotation matrix only
    float start_orientation[3];
    this->m_TmpTransform->GetOrientation(start_orientation);

    /** to be rewritten */
    this->m_DeltaTransform->RotateY(start_orientation[1],POST_MULTIPLY);
    this->m_DeltaTransform->RotateX(start_orientation[0],POST_MULTIPLY);
    this->m_DeltaTransform->RotateZ(start_orientation[2],POST_MULTIPLY);
  
    this->m_DeltaTransform->RotateY(-start_orientation[1],PRE_MULTIPLY);
    this->m_DeltaTransform->RotateX(-start_orientation[0],PRE_MULTIPLY);
    this->m_DeltaTransform->RotateZ(-start_orientation[2],PRE_MULTIPLY);
  }
}

//------------------------------------------------------------------------------
int albaInteractor6DOF::OnStartInteraction(albaEventInteraction *event)
//------------------------------------------------------------------------------
{
  albaDeviceButtonsPadTracker *tracker=albaDeviceButtonsPadTracker::SafeDownCast((albaDevice *)event->GetSender());
  assert(tracker);
  albaMatrix *start_tracker_pose=event->GetMatrix();
  return StartInteraction(tracker,start_tracker_pose);
}
//------------------------------------------------------------------------------
int albaInteractor6DOF::OnStopInteraction(albaEventInteraction *event)
//------------------------------------------------------------------------------
{
  albaDeviceButtonsPadTracker *tracker=albaDeviceButtonsPadTracker::SafeDownCast((albaDevice *)event->GetSender());
  assert(tracker);
  albaMatrix *stop_tracker_pose=event->GetMatrix();
  return StopInteraction(tracker,stop_tracker_pose);
}
