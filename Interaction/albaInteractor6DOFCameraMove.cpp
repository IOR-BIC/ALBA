/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractor6DOFCameraMove
 Authors: Michele Diegoli & Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaInteractor6DOFCameraMove.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaAvatar3D.h"
#include "albaTransform.h"
#include "albaCameraTransform.h"
#include "albaEventInteraction.h"

#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkTransform.h"
#include "vtkMath.h"
#include <assert.h>

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractor6DOFCameraMove)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaInteractor6DOFCameraMove::albaInteractor6DOFCameraMove()
//------------------------------------------------------------------------------

{
  m_IgnoreTriggerEvents = 0;
  m_CurrentCamera = NULL;
}

//------------------------------------------------------------------------------
albaInteractor6DOFCameraMove::~albaInteractor6DOFCameraMove()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void albaInteractor6DOFCameraMove::OnEvent(albaEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);
  assert(event->GetSender());
  
  albaID id=event->GetId();
  albaID channel=event->GetChannel();

  if (channel==MCH_INPUT && m_InteractionFlag)
  {
    albaEventInteraction *e=(albaEventInteraction *)event;
    albaDeviceButtonsPadTracker *tracker=GetTracker();
    
    if (tracker==NULL)
    {
      tracker=albaDeviceButtonsPadTracker::SafeDownCast((albaObject *)event->GetSender());
      SetTracker(tracker);
      
    }  
    
    // if the event comes from tracker which started the interaction continue...
    if (id==albaDeviceButtonsPadTracker::GetTracker3DMoveId() && tracker)
    {
      if (!m_CurrentCamera)
        return;

	    albaMatrix *tracker_pose=e->GetMatrix();
      SetTrackerPoseMatrix(tracker_pose);
      Update();
      return;

    }
  }
    
  Superclass::OnEvent(event);
}
//------------------------------------------------------------------------------
int albaInteractor6DOFCameraMove::StartInteraction(albaDeviceButtonsPadTracker *tracker,albaMatrix *pose)
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
void albaInteractor6DOFCameraMove::Update()
//------------------------------------------------------------------------------
{
  this->UpdateDeltaTransform();

  double XYZ[3];
  m_DeltaTransform->GetPosition(XYZ);

  double rotXYZ[3];
  m_DeltaTransform->GetOrientation(rotXYZ);

  vtkTransform *trans = vtkTransform::New();
  trans->PreMultiply();

  trans->Translate(m_StartFocalPoint[0],m_StartFocalPoint[1],m_StartFocalPoint[2]);   
  trans->RotateY(-rotXYZ[1]);
  trans->RotateX(-rotXYZ[0]);
  trans->RotateZ(-rotXYZ[2]);
  trans->Translate(-m_StartFocalPoint[0],-m_StartFocalPoint[1],-m_StartFocalPoint[2]);


  double vup[3];
  m_CurrentCamera->GetViewUp(vup);

  // project along the view up
  double up_trans_mod;
  up_trans_mod = vtkMath::Dot(XYZ,vup);

  double up_trans[3];
  up_trans[0] = up_trans_mod * vup[0];
  up_trans[1] = up_trans_mod * vup[1];
  up_trans[2] = up_trans_mod * vup[2];

  // compute the horizontal vector with respect to camera
  double direction[3];
  m_CurrentCamera->GetViewPlaneNormal(direction);

  double x_vect[3];
  vtkMath::Cross(vup,direction,x_vect);

  vtkMath::Normalize(x_vect);

  // translation along the X axis of the camera
  double oriz_trans_mod = vtkMath::Dot(XYZ,x_vect);

  double oriz_trans[3];
  oriz_trans[0] = oriz_trans_mod * x_vect[0];
  oriz_trans[1] = oriz_trans_mod * x_vect[1];
  oriz_trans[2] = oriz_trans_mod * x_vect[2];
  
  vtkALBASmartPointer<vtkTransform> trans2;
  trans2->DeepCopy(trans);
  trans2->Translate(-up_trans[0],-up_trans[1],-up_trans[2]);
  trans2->Translate(-oriz_trans[0],-oriz_trans[1],-oriz_trans[2]);

  trans->Translate(-XYZ[0],-XYZ[1],-XYZ[2]);  

  double posNew[4],fpNew[4],vuOld[4], vuNew[4];

  vuOld[0] = m_StartViewUp[0] + m_StartCameraPosition[0];
  vuOld[1] = m_StartViewUp[1] + m_StartCameraPosition[1];
  vuOld[2] = m_StartViewUp[2] + m_StartCameraPosition[2];
  vuOld[3] = m_StartViewUp[3];

  trans->MultiplyPoint(m_StartCameraPosition, posNew);
  trans2->MultiplyPoint(m_StartFocalPoint, fpNew);
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

  // Store current pose matrix
  TrackerSnapshot(m_TrackerPoseMatrix);



  trans->Delete();
}

