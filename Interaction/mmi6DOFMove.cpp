/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmi6DOFMove.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-03 15:42:35 $
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

#include "mmi6DOFMove.h"
#include "mmdTracker.h"
#include "mafAvatar3D.h"

#include "mflEventInteraction.h"
#include "mflDefines.h"
#include "mflTransform.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "mflTransform.h"
#include "mflCameraTransform.h"
#include <assert.h>

//------------------------------------------------------------------------------
vtkStandardNewMacro(mmi6DOFMove)
vtkCxxSetObjectMacro(mmi6DOFMove,ObjectPoseMatrix,mflMatrix)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmi6DOFMove::mmi6DOFMove()
//------------------------------------------------------------------------------
{
  ObjectPoseMatrix = NULL;
  vtkNEW(OutputObjectPoseMatrix);
  IgnoreTriggerEvents = 0;
  DifferentialMoving  = 0;
}

//------------------------------------------------------------------------------
mmi6DOFMove::~mmi6DOFMove()
//------------------------------------------------------------------------------
{
  vtkDEL(OutputObjectPoseMatrix);
  vtkDEL(ObjectPoseMatrix);
}

//------------------------------------------------------------------------------
void mmi6DOFMove::ProcessEvent(mflEvent *event,mafID channel)
//------------------------------------------------------------------------------
{
  assert(event);
  assert(event->GetSender());
  
  unsigned int id=event->GetID();

  if (channel==mafDevice::DeviceInputChannel && GetTracker())
  {
    // if the event comes from tracker which started the interaction continue...
    mmdTracker *tracker=mmdTracker::SafeDownCast((vtkObject *)event->GetSender());
    if (id==mmdTracker::Move3DEvent && tracker==GetTracker())
    {
      mflEventInteraction *e=(mflEventInteraction *)event;        
      
      // if the tracker does not have an avatar we don't
      // know the mapping rule and cannot proceed
      if (Avatar)
	    {
        mflMatrix *tracker_pose=e->GetMatrix();
        SetTrackerPoseMatrix(tracker_pose);

        Update();
        
        if (GetResultTransform())
		    {
          GetResultTransform()->SetMatrix(OutputObjectPoseMatrix);
        }

        // forward the event with the pose matrix through default channel
        mflSMART(mflEventInteraction,e)(MoveActionEvent,this,OutputObjectPoseMatrix);
        ForwardEvent(e);
		    return;
      }
    }
  }
  Superclass::ProcessEvent(event,channel);
}
//------------------------------------------------------------------------------
int mmi6DOFMove::StartInteraction(mmdTracker *tracker,mflMatrix *pose)
//------------------------------------------------------------------------------
{
  assert(ObjectPoseMatrix);
  if (Superclass::StartInteraction(tracker,pose))
  {
	  mflTransform::GetPosition(ObjectPoseMatrix,this->StartObjPosition);
    mflTransform::GetOrientation(ObjectPoseMatrix,this->StartObjOrientation);
    //mflTransform::GetScale(ObjectPoseMatrix,this->StartScale);
	  return true;
  }
 
  return false;
}

//----------------------------------------------------------------------------
void mmi6DOFMove::Update()
//------------------------------------------------------------------------------
{
  if (DifferentialMoving == 1)
	{
		// not yet implemented should not be here
		this->UpdateDeltaTransform();
  
    //this->DeltaTransform->Push();
    //this->DeltaTransform->PreMultiply();
    DeltaTransform->RotateZ(this->StartObjOrientation[2],PRE_MULTIPLY);
    DeltaTransform->RotateX(this->StartObjOrientation[0],PRE_MULTIPLY);
    DeltaTransform->RotateY(this->StartObjOrientation[1],PRE_MULTIPLY);
    //this->DeltaTransform->PostMultiply();
    DeltaTransform->Translate(this->StartObjPosition[0],this->StartObjPosition[1],this->StartObjPosition[2],POST_MULTIPLY);
    //this->DeltaTransform->PreMultiply();
    DeltaTransform->GetOrientation(this->ObjectOrientation);
    DeltaTransform->GetPosition(this->ObjectPosition);
    //this->DeltaTransform->Pop();

    mflTransform::SetOrientation(OutputObjectPoseMatrix,ObjectOrientation);
    mflTransform::SetPosition(OutputObjectPoseMatrix,ObjectPosition);
  }
	else
	{
    Avatar->TrackerToWorld(TrackerPoseMatrix,OutputObjectPoseMatrix,false);
  }
 
  
}

