/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmi6DOF.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-03 15:42:34 $
  Version:   $Revision: 1.1 $
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

#include "mmi6DOF.h"
#include "mmdTracker.h"
#include "mafAvatar3D.h"

#include "vtkObjectFactory.h"

#include "mflMatrix.h"
#include "vtkTransform.h"
#include "mflCameraTransform.h"

#include "vtkMath.h"
#include "mflEvent.h"
#include "mflEventInteraction.h"
#include "mflBounds.h"

#include <assert.h>

//------------------------------------------------------------------------------
vtkStandardNewMacro(mmi6DOF)
//------------------------------------------------------------------------------
mmi6DOF::mmi6DOF()
//------------------------------------------------------------------------------
{
  vtkNEW(TrackerPoseMatrix);
  vtkNEW(InverseTrackerPoseMatrix);
  vtkNEW(StartTrackerPoseMatrix);
  vtkNEW(TmpTransform);
  vtkNEW(DeltaTransform);
    
}

//------------------------------------------------------------------------------
mmi6DOF::~mmi6DOF()
//------------------------------------------------------------------------------
{
  vtkDEL(TrackerPoseMatrix);
  vtkDEL(InverseTrackerPoseMatrix);
  vtkDEL(StartTrackerPoseMatrix);
  vtkDEL(TmpTransform);
  vtkDEL(DeltaTransform);
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
int mmi6DOF::StartInteraction(mmdTracker *tracker,mflMatrix *pose)
//------------------------------------------------------------------------------
{
  if (Superclass::StartInteraction(tracker))
  {
    if (pose)
      TrackerSnapshot(pose);
    else
    {
      StartTrackerPoseMatrix->Identity();
      StartTrackerPoseMatrix->SetTimeStamp(0);
    }
    
    Avatar = mafAvatar3D::SafeDownCast(GetTracker()->GetAvatar());
    // mmi6DOF works only with avatar of type mafAvatar3D
    if (Avatar)
    {
      // store the current renderer: camera cannot be changed during interaction!!!
      SetRenderer(tracker->GetAvatar()->GetRenderer());
      ForwardEvent(InteractionStartedEvent,DefaultChannel,StartTrackerPoseMatrix);
      return true;
    }
    // if wrong type of avatar force unlock device and stop interaction 
    Superclass::StopInteraction(tracker); 
  }
  return false;
}

//------------------------------------------------------------------------------
int mmi6DOF::StopInteraction(mmdTracker *tracker,mflMatrix *pose)
//------------------------------------------------------------------------------
{
  if (Superclass::StopInteraction(tracker))
  {
    // ...do something
    ForwardEvent(InteractionStoppedEvent);
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
void mmi6DOF::SetTrackerPoseMatrix(mflMatrix *pose)
//------------------------------------------------------------------------------
{
  assert(pose);
  this->TrackerPoseMatrix->DeepCopy(pose);
}

// Raw Pose
//------------------------------------------------------------------------------
void mmi6DOF::TrackerSnapshot(mflMatrix *pose)
//------------------------------------------------------------------------------
{
  assert(pose);
  this->StartTrackerPoseMatrix->DeepCopy(pose);
  mflMatrix::Invert(StartTrackerPoseMatrix,InverseTrackerPoseMatrix);
}

//------------------------------------------------------------------------------
void mmi6DOF::UpdateDeltaTransform()
//------------------------------------------------------------------------------
{
  if (Avatar)
  {
    assert(Device);
    TmpTransform->SetMatrix(this->TrackerPoseMatrix);
    TmpTransform->Concatenate(this->InverseTrackerPoseMatrix,POST_MULTIPLY);

    DeltaTransform->Identity();
    if (RotationFlag) // if rotation is enabled copy rotation
      DeltaTransform->CopyRotation(TmpTransform->GetMatrix());

    if (TranslationFlag) // if translation is enabled copy translation
      DeltaTransform->CopyTranslation(TmpTransform->GetMatrix());

    // Rotate and Scale according to calibration matrixes
    float wscale[3];
    float wbscale[3];

    // Scale the DeltaTransform accordingly
    GetTracker()->GetTrackerToCanonicalTransform()->GetScale(wscale);
  
    // @todo change here to map to current constrain ref sys
    Avatar->GetCanonicalToWorldTransform()->GetScale(wbscale);
  
    DeltaTransform->Scale(wscale[0],wscale[1],wscale[2],POST_MULTIPLY);
    DeltaTransform->Scale(wbscale[0],wbscale[1],wbscale[2],POST_MULTIPLY);
  
    // compute the initial tracker pose in world coordinate, do it each iteration
    // to take in consideration possible changes in the two trasformation (tracker 
    // to canonical and canonical to world; e.g. when i move the camera the canonical 
    // to world changes accordingly)
    TmpTransform->SetMatrix(StartTrackerPoseMatrix);

    GetTracker()->TrackerToCanonical(TmpTransform);

    // @todo change here to map to current constrain ref sys
    Avatar->CanonicalToWorld(TmpTransform); // map according to current transformations

    // Use of the initial matrix pose of the tracker in the world coordinate system, but 
    // extract the rotation matrix only
    float start_orientation[3];
    this->TmpTransform->GetOrientation(start_orientation);

    /** to be rewritten */
    this->DeltaTransform->RotateY(start_orientation[1],POST_MULTIPLY);
    this->DeltaTransform->RotateX(start_orientation[0],POST_MULTIPLY);
    this->DeltaTransform->RotateZ(start_orientation[2],POST_MULTIPLY);
  
    this->DeltaTransform->RotateY(-start_orientation[1],PRE_MULTIPLY);
    this->DeltaTransform->RotateX(-start_orientation[0],PRE_MULTIPLY);
    this->DeltaTransform->RotateZ(-start_orientation[2],PRE_MULTIPLY);
  }
}

//------------------------------------------------------------------------------
int mmi6DOF::OnStartInteraction(mflEventInteraction *event)
//------------------------------------------------------------------------------
{
  mmdTracker *tracker=mmdTracker::SafeDownCast((mafDevice *)event->GetSender());
  assert(tracker);
  mflMatrix *start_tracker_pose=event->GetMatrix();
  return StartInteraction(tracker,start_tracker_pose);
}
//------------------------------------------------------------------------------
int mmi6DOF::OnStopInteraction(mflEventInteraction *event)
//------------------------------------------------------------------------------
{
  mmdTracker *tracker=mmdTracker::SafeDownCast((mafDevice *)event->GetSender());
  assert(tracker);
  mflMatrix *stop_tracker_pose=event->GetMatrix();
  return StopInteraction(tracker,stop_tracker_pose);
}
