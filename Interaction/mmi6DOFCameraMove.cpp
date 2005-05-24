/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmi6DOFCameraMove.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-24 16:43:05 $
  Version:   $Revision: 1.3 $
  Authors:   Michele Diegoli & Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mmi6DOFCameraMove.h"
#include "mmdTracker.h"
#include "mafAvatar3D.h"
#include "mafTransform.h"
#include "mafCameraTransform.h"
#include "mafEventInteraction.h"

#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkTransform.h"
#include <assert.h>

//------------------------------------------------------------------------------
mafCxxTypeMacro(mmi6DOFCameraMove)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmi6DOFCameraMove::mmi6DOFCameraMove()
//------------------------------------------------------------------------------

{
  m_IgnoreTriggerEvents = 0;
  m_CurrentCamera = NULL;
}

//------------------------------------------------------------------------------
mmi6DOFCameraMove::~mmi6DOFCameraMove()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void mmi6DOFCameraMove::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);
  assert(event->GetSender());
  
  mafID id=event->GetId();
  mafID channel=event->GetChannel();

  if (channel==MCH_INPUT && m_InteractionFlag)
  {
    mafEventInteraction *e=(mafEventInteraction *)event;
    mmdTracker *tracker=GetTracker();
    
    if (tracker==NULL)
    {
      tracker=mmdTracker::SafeDownCast((mafObject *)event->GetSender());
      SetTracker(tracker);
      
    }  
    
    // if the event comes from tracker which started the interaction continue...
    if (id==mmdTracker::TRACKER_3D_MOVE && tracker)
    {
      if (!m_CurrentCamera)
        return;

	    mafMatrix *tracker_pose=e->GetMatrix();
      SetTrackerPoseMatrix(tracker_pose);
      Update();
      return;

    }
  }
    
  Superclass::OnEvent(event);
}
//------------------------------------------------------------------------------
int mmi6DOFCameraMove::StartInteraction(mmdTracker *tracker,mafMatrix *pose)
//------------------------------------------------------------------------------
{
  //assert(ObjectPoseMatrix);
  if (Superclass::StartInteraction(tracker,pose))
  {
    if (tracker)
    {
       // if the tracker does not have an avatar we don't
      // know the mapping rule and cannot proceed
      if (m_Avatar&&m_Avatar->GetRenderer())
	    {
        vtkCamera *active_camera=m_Avatar->GetRenderer()->GetActiveCamera();
        
        m_CurrentCamera=active_camera;
        active_camera->GetPosition(m_StartCameraPosition);
        active_camera->GetFocalPoint(m_StartFocalPoint);
        active_camera->GetViewUp(m_StartViewUp);
        active_camera->GetDirectionOfProjection(m_StartOrientation);
        
        m_StartCameraPosition[3] = 1.0;
        m_StartFocalPoint[3] = 1.0;
        m_StartViewUp[3] = 1.0;
        m_OldZ=0;

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
  m_DeltaTransform->GetPosition(XYZ);

  float rotXYZ[3];
  m_DeltaTransform->GetOrientation(rotXYZ);

  vtkTransform *trans = vtkTransform::New();
  trans->PreMultiply();

  trans->Translate(m_StartFocalPoint[0],m_StartFocalPoint[1],m_StartFocalPoint[2]);   
  trans->RotateY(-rotXYZ[1]);
  trans->RotateX(-rotXYZ[0]);
  trans->RotateZ(-rotXYZ[2]);
  trans->Translate(-m_StartFocalPoint[0],-m_StartFocalPoint[1],-m_StartFocalPoint[2]);

  trans->Translate(-XYZ[0],-XYZ[1],-XYZ[2]);  

  double posNew[4],fpNew[4],vuOld[4], vuNew[4];

  vuOld[0] = m_StartViewUp[0] + m_StartCameraPosition[0];
  vuOld[1] = m_StartViewUp[1] + m_StartCameraPosition[1];
  vuOld[2] = m_StartViewUp[2] + m_StartCameraPosition[2];
  vuOld[3] = m_StartViewUp[3];

  trans->MultiplyPoint(m_StartCameraPosition, posNew);
  trans->MultiplyPoint(m_StartFocalPoint, fpNew);
  trans->MultiplyPoint(vuOld, vuNew);

  vuNew[0] -= posNew[0];
  vuNew[1] -= posNew[1];
  vuNew[2] -= posNew[2];

  m_CurrentCamera->SetPosition(posNew);
  m_CurrentCamera->SetFocalPoint(fpNew);
  m_CurrentCamera->SetViewUp(vuNew);

  // Let's make it incremental...
  // store new camera parameters
  for (int i=0;i<3;i++)
  {
    m_StartCameraPosition[i]=posNew[i];
    m_StartFocalPoint[i]=fpNew[i];
    m_StartViewUp[i]=vuNew[i];
  }

  //m_CurrentCamera->OrthogonalizeViewUp();

  // Store current pose matrix
  TrackerSnapshot(m_TrackerPoseMatrix);



  trans->Delete();
}

