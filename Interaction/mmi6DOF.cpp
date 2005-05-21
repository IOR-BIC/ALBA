/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmi6DOF.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-21 07:55:50 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mmi6DOF.h"
#include "mmdTracker.h"
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
mafCxxAbstractTypeMacro(mmi6DOF)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmi6DOF::mmi6DOF()
//------------------------------------------------------------------------------
{
  vtkNEW(m_TrackerPoseMatrix);
  vtkNEW(m_InverseTrackerPoseMatrix);
  vtkNEW(m_StartTrackerPoseMatrix);
  vtkNEW(m_TmpTransform);
  vtkNEW(m_DeltaTransform);
    
}

//------------------------------------------------------------------------------
mmi6DOF::~mmi6DOF()
//------------------------------------------------------------------------------
{
  vtkDEL(m_TrackerPoseMatrix);
  vtkDEL(m_InverseTrackerPoseMatrix);
  vtkDEL(m_StartTrackerPoseMatrix);
  vtkDEL(m_TmpTransform);
  vtkDEL(m_DeltaTransform);
}

//------------------------------------------------------------------------------
void mmi6DOF::SetRenderer(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  Superclass::SetRenderer(ren);
}

//------------------------------------------------------------------------------
void mmi6DOF::SetTracker(mmdTracker *tracker)
//------------------------------------------------------------------------------
{
  SetDevice(tracker);
}

//------------------------------------------------------------------------------
void mmi6DOF::HideAvatar()
//------------------------------------------------------------------------------
{
  mmdTracker *tracker=GetTracker();
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
void mmi6DOF::ShowAvatar()
//------------------------------------------------------------------------------
{
  mmdTracker *tracker=GetTracker();
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
int mmi6DOF::StartInteraction(mmdTracker *tracker,mafMatrix *pose)
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
    // mmi6DOF works only with avatar of type mafAvatar3D
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
int mmi6DOF::StopInteraction(mmdTracker *tracker,mafMatrix *pose)
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
void mmi6DOF::SetTrackerPoseMatrix(mafMatrix *pose)
//------------------------------------------------------------------------------
{
  assert(pose);
  m_TrackerPoseMatrix->DeepCopy(pose);
}

// Raw Pose
//------------------------------------------------------------------------------
void mmi6DOF::TrackerSnapshot(mafMatrix *pose)
//------------------------------------------------------------------------------
{
  assert(pose);
  m_StartTrackerPoseMatrix->DeepCopy(pose);
  mafMatrix::Invert(*m_StartTrackerPoseMatrix,*m_InverseTrackerPoseMatrix);
}

//------------------------------------------------------------------------------
void mmi6DOF::UpdateDeltaTransform()
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
int mmi6DOF::OnStartInteraction(mafEventInteraction *event)
//------------------------------------------------------------------------------
{
  mmdTracker *tracker=mmdTracker::SafeDownCast((mafDevice *)event->GetSender());
  assert(tracker);
  mafMatrix *start_tracker_pose=event->GetMatrix();
  return StartInteraction(tracker,start_tracker_pose);
}
//------------------------------------------------------------------------------
int mmi6DOF::OnStopInteraction(mafEventInteraction *event)
//------------------------------------------------------------------------------
{
  mmdTracker *tracker=mmdTracker::SafeDownCast((mafDevice *)event->GetSender());
  assert(tracker);
  mafMatrix *stop_tracker_pose=event->GetMatrix();
  return StopInteraction(tracker,stop_tracker_pose);
}
