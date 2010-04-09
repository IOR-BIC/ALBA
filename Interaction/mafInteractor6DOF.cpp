/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafInteractor6DOF.cpp,v $
  Language:  C++
  Date:      $Date: 2010-04-09 07:01:11 $
  Version:   $Revision: 1.1.2.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafInteractor6DOF.h"
#include "mafDeviceButtonsPadTracker.h"
#include "mafAvatar3D.h"

#include "mafMatrix.h"
#include "mafEventBase.h"
#include "mafTransform.h"
#include "mafCameraTransform.h"
#include "mafEventInteraction.h"
#include "mafOBB.h"

#include "vtkMath.h"

#include <assert.h>

//------------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafInteractor6DOF)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafInteractor6DOF::mafInteractor6DOF()
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
mafInteractor6DOF::~mafInteractor6DOF()
//------------------------------------------------------------------------------
{
  vtkDEL(m_TrackerPoseMatrix);
  vtkDEL(m_InverseTrackerPoseMatrix);
  vtkDEL(m_StartTrackerPoseMatrix);
  vtkDEL(m_TmpTransform);
  vtkDEL(m_DeltaTransform);
}

//------------------------------------------------------------------------------
void mafInteractor6DOF::SetRenderer(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  Superclass::SetRenderer(ren);
}

//------------------------------------------------------------------------------
void mafInteractor6DOF::SetTracker(mafDeviceButtonsPadTracker *tracker)
//------------------------------------------------------------------------------
{
  SetDevice(tracker);
}

//------------------------------------------------------------------------------
void mafInteractor6DOF::HideAvatar()
//------------------------------------------------------------------------------
{
  mafDeviceButtonsPadTracker *tracker=GetTracker();
  if (tracker)
  {
    mafAvatar *avatar=tracker->GetAvatar();
    if (avatar)
    {
      avatar->Hide();
    }
  }
}

//------------------------------------------------------------------------------
void mafInteractor6DOF::ShowAvatar()
//------------------------------------------------------------------------------
{
  mafDeviceButtonsPadTracker *tracker=GetTracker();
  if (tracker)
  {
    mafAvatar *avatar=tracker->GetAvatar();
    if (avatar)
    {
      avatar->Show();
    }
  }
}

//------------------------------------------------------------------------------
int mafInteractor6DOF::StartInteraction(mafDeviceButtonsPadTracker *tracker,mafMatrix *pose)
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
    
    m_Avatar = mafAvatar3D::SafeDownCast(GetTracker()->GetAvatar());
    // mafInteractor6DOF works only with avatar of type mafAvatar3D
    if (m_Avatar)
    {
      // store the current renderer: camera cannot be changed during interaction!!!
      SetRenderer(tracker->GetAvatar()->GetRenderer());
      InvokeEvent(INTERACTION_STARTED,MCH_UP,m_StartTrackerPoseMatrix);
      return true;
    }
    // if wrong type of avatar force unlock device and stop interaction 
    Superclass::StopInteraction(tracker); 
  }
  return false;
}

//------------------------------------------------------------------------------
int mafInteractor6DOF::StopInteraction(mafDeviceButtonsPadTracker *tracker,mafMatrix *pose)
//------------------------------------------------------------------------------
{
  if (Superclass::StopInteraction(tracker))
  {
    // ...do something
    InvokeEvent(INTERACTION_STOPPED);
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
void mafInteractor6DOF::SetTrackerPoseMatrix(mafMatrix *pose)
//------------------------------------------------------------------------------
{
  assert(pose);
  m_TrackerPoseMatrix->DeepCopy(pose);
}

// Raw Pose
//------------------------------------------------------------------------------
void mafInteractor6DOF::TrackerSnapshot(mafMatrix *pose)
//------------------------------------------------------------------------------
{
  assert(pose);
  m_StartTrackerPoseMatrix->DeepCopy(pose);
  mafMatrix::Invert(*m_StartTrackerPoseMatrix,*m_InverseTrackerPoseMatrix);
}

//------------------------------------------------------------------------------
void mafInteractor6DOF::UpdateDeltaTransform()
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
    mafTransform::GetScale(m_Avatar->GetCanonicalToWorldTransform()->GetMatrix(), wbscale);
  
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
int mafInteractor6DOF::OnStartInteraction(mafEventInteraction *event)
//------------------------------------------------------------------------------
{
  mafDeviceButtonsPadTracker *tracker=mafDeviceButtonsPadTracker::SafeDownCast((mafDevice *)event->GetSender());
  assert(tracker);
  mafMatrix *start_tracker_pose=event->GetMatrix();
  return StartInteraction(tracker,start_tracker_pose);
}
//------------------------------------------------------------------------------
int mafInteractor6DOF::OnStopInteraction(mafEventInteraction *event)
//------------------------------------------------------------------------------
{
  mafDeviceButtonsPadTracker *tracker=mafDeviceButtonsPadTracker::SafeDownCast((mafDevice *)event->GetSender());
  assert(tracker);
  mafMatrix *stop_tracker_pose=event->GetMatrix();
  return StopInteraction(tracker,stop_tracker_pose);
}
