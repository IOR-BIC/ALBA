/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmi6DOFCameraMove.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-03 15:42:35 $
  Version:   $Revision: 1.1 $
  Authors:   Michele Diegoli & Marco Petrone
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

#include "mmi6DOFCameraMove.h"
#include "mmdTracker.h"
#include "mafAvatar3D.h"

#include "vtkObjectFactory.h"
#include "mflEventInteraction.h"
#include "mflDefines.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkTransform.h"
#include "mflTransform.h"
#include "mflCameraTransform.h"
#include <assert.h>

//------------------------------------------------------------------------------
vtkStandardNewMacro(mmi6DOFCameraMove)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmi6DOFCameraMove::mmi6DOFCameraMove()
//------------------------------------------------------------------------------

{
  IgnoreTriggerEvents = 0;
  CurrentCamera = NULL;
}

//------------------------------------------------------------------------------
mmi6DOFCameraMove::~mmi6DOFCameraMove()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void mmi6DOFCameraMove::ProcessEvent(mflEvent *event,mafID channel)
//------------------------------------------------------------------------------
{
  assert(event);
  assert(event->GetSender());
  
  unsigned int id=event->GetID();

  if (channel==mafDevice::DeviceInputChannel&&InteractionFlag)
  {
    mflEventInteraction *e=(mflEventInteraction *)event;
    mmdTracker *tracker=GetTracker();
    
    if (tracker==NULL)
    {
      tracker=mmdTracker::SafeDownCast((vtkObject *)event->GetSender());
      SetTracker(tracker);
      
    }  
    
    // if the event comes from tracker which started the interaction continue...
    if (id==mmdTracker::Move3DEvent&&tracker)
    {
      if (!CurrentCamera)
        return;

	    mflMatrix *tracker_pose=e->GetMatrix();
      SetTrackerPoseMatrix(tracker_pose);
      Update();
      return;

    }
  }
    
  Superclass::ProcessEvent(event,channel);
}
//------------------------------------------------------------------------------
int mmi6DOFCameraMove::StartInteraction(mmdTracker *tracker,mflMatrix *pose)
//------------------------------------------------------------------------------
{
  //assert(ObjectPoseMatrix);
  if (Superclass::StartInteraction(tracker,pose))
  {
    if (tracker)
    {
       // if the tracker does not have an avatar we don't
      // know the mapping rule and cannot proceed
      if (Avatar&&Avatar->GetRenderer())
	    {
        vtkCamera *active_camera=Avatar->GetRenderer()->GetActiveCamera();
        
        this->CurrentCamera=active_camera;
        active_camera->GetPosition(this->StartCameraPosition);
        active_camera->GetFocalPoint(this->StartFocalPoint);
        active_camera->GetViewUp(this->StartViewUp);
        active_camera->GetDirectionOfProjection(this->StartOrientation);
		
		this->StartCameraPosition[3] = 1.0;
        this->StartFocalPoint[3] = 1.0;
        this->StartViewUp[3] = 1.0;
		this->OldZ=0;

      }
    }
    
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
void mmi6DOFCameraMove::Update()
//------------------------------------------------------------------------------
{
  // not yet implemented should not be here
  this->UpdateDeltaTransform();

  float XYZ[3];
  this->DeltaTransform->GetPosition(XYZ);

  float rotXYZ[3];
  this->DeltaTransform->GetOrientation(rotXYZ);

  vtkTransform *trans = vtkTransform::New();
  trans->PreMultiply();

  trans->Translate(StartFocalPoint[0],StartFocalPoint[1],StartFocalPoint[2]);   
  trans->RotateY(-rotXYZ[1]);
  trans->RotateX(-rotXYZ[0]);
  trans->RotateZ(-rotXYZ[2]);
  trans->Translate(-StartFocalPoint[0],-StartFocalPoint[1],-StartFocalPoint[2]);

  trans->Translate(-XYZ[0],-XYZ[1],-XYZ[2]);  

  double posNew[4],fpNew[4],vuOld[4], vuNew[4];

  vuOld[0] = this->StartViewUp[0] + this->StartCameraPosition[0];
  vuOld[1] = this->StartViewUp[1] + this->StartCameraPosition[1];
  vuOld[2] = this->StartViewUp[2] + this->StartCameraPosition[2];
  vuOld[3] = this->StartViewUp[3];

  trans->MultiplyPoint(StartCameraPosition, posNew);
  trans->MultiplyPoint(this->StartFocalPoint, fpNew);
  trans->MultiplyPoint(vuOld, vuNew);

  vuNew[0] -= posNew[0];
  vuNew[1] -= posNew[1];
  vuNew[2] -= posNew[2];

  CurrentCamera->SetPosition(posNew);
  CurrentCamera->SetFocalPoint(fpNew);
  CurrentCamera->SetViewUp(vuNew);

  // Let's make it incremental...
  // store new camera parameters
  for (int i=0;i<3;i++)
  {
    StartCameraPosition[i]=posNew[i];
    StartFocalPoint[i]=fpNew[i];
    StartViewUp[i]=vuNew[i];
  }

  //CurrentCamera->OrthogonalizeViewUp();

  // Store current pose matrix
  TrackerSnapshot(TrackerPoseMatrix);



  trans->Delete();
}

