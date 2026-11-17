/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCameraTrackerTool
 Authors: Gianluigi Crimi, from albaInteractor6DOFCameraMove by Marco Petrone

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaCameraTrackerTool.h"
#include "albaAvatar3D.h"
#include "albaCameraTransform.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaEventInteraction.h"
#include "albaMatrix.h"
#include "albaTransform.h"

#include "vtkALBASmartPointer.h"
#include "vtkCamera.h"
#include "vtkMath.h"
#include "vtkRenderer.h"
#include "vtkTransform.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaCameraTrackerTool);

//------------------------------------------------------------------------------
albaCameraTrackerTool::albaCameraTrackerTool()
{
  m_Tracker = NULL;
  m_Avatar = NULL;
  m_CurrentCamera = NULL;

  vtkNEW(m_TrackerPoseMatrix);
  vtkNEW(m_StartTrackerPoseMatrix);
  vtkNEW(m_InverseTrackerPoseMatrix);
  vtkNEW(m_DeltaTransform);
  vtkNEW(m_TmpTransform);
}

//------------------------------------------------------------------------------
albaCameraTrackerTool::~albaCameraTrackerTool()
{
  vtkDEL(m_TrackerPoseMatrix);
  vtkDEL(m_StartTrackerPoseMatrix);
  vtkDEL(m_InverseTrackerPoseMatrix);
  vtkDEL(m_DeltaTransform);
  vtkDEL(m_TmpTransform);
}

//------------------------------------------------------------------------------
bool albaCameraTrackerTool::IsInteracting(albaDevice *device)
{
  return m_Tracker != NULL && (albaDevice *)m_Tracker == device;
}

//------------------------------------------------------------------------------
bool albaCameraTrackerTool::OnLeftButtonDown(albaEventInteraction *e)
{
  albaDeviceButtonsPadTracker *tracker = albaDeviceButtonsPadTracker::SafeDownCast((albaDevice *)e->GetSender());
  if (tracker == NULL || m_Tracker != NULL)
    return false;

  // without an albaAvatar3D the mapping from tracker to world is unknown
  albaAvatar3D *avatar = albaAvatar3D::SafeDownCast(tracker->GetAvatar());
  if (avatar == NULL)
    return false;

  m_Tracker = tracker;
  m_Avatar = avatar;

  if (e->GetMatrix())
    TrackerSnapshot(e->GetMatrix());
  else
  {
    m_StartTrackerPoseMatrix->Identity();
    m_StartTrackerPoseMatrix->SetTimeStamp(0);
  }

  // the camera cannot change during the motion
  SetRendererAndView(avatar->GetRenderer(), avatar->GetView());
  m_CurrentCamera = m_Renderer ? m_Renderer->GetActiveCamera() : NULL;
  if (m_CurrentCamera)
  {
    m_CurrentCamera->GetPosition(m_StartCameraPosition);
    m_CurrentCamera->GetFocalPoint(m_StartFocalPoint);
    m_CurrentCamera->GetViewUp(m_StartViewUp);
    m_StartCameraPosition[3] = m_StartFocalPoint[3] = m_StartViewUp[3] = 1.0;
  }
  return true;
}

//------------------------------------------------------------------------------
bool albaCameraTrackerTool::OnLeftButtonUp(albaEventInteraction *e)
{
  if (!IsInteracting((albaDevice *)e->GetSender()))
    return false;

  m_Tracker = NULL;
  m_Avatar = NULL;
  return true;
}

//------------------------------------------------------------------------------
bool albaCameraTrackerTool::OnDrag(albaEventInteraction *e)
{
  if (!IsInteracting((albaDevice *)e->GetSender()))
    return false;

  if (m_CurrentCamera && e->GetMatrix())
  {
    m_TrackerPoseMatrix->DeepCopy(e->GetMatrix());
    UpdateCamera();
  }
  return true;
}

//------------------------------------------------------------------------------
void albaCameraTrackerTool::TrackerSnapshot(albaMatrix *pose)
{
  m_StartTrackerPoseMatrix->DeepCopy(pose);
  albaMatrix::Invert(*m_StartTrackerPoseMatrix, *m_InverseTrackerPoseMatrix);
}

//------------------------------------------------------------------------------
void albaCameraTrackerTool::UpdateDeltaTransform()
{
  m_TmpTransform->SetMatrix(*m_TrackerPoseMatrix);
  m_TmpTransform->Concatenate(*m_InverseTrackerPoseMatrix, POST_MULTIPLY);

  m_DeltaTransform->Identity();
  m_DeltaTransform->CopyRotation(m_TmpTransform->GetMatrix());
  m_DeltaTransform->CopyTranslation(m_TmpTransform->GetMatrix());

  // scale according to the calibration matrices
  double wscale[3];
  double wbscale[3];
  m_Tracker->GetTrackerToCanonicalTransform()->GetScale(wscale);
  albaTransform::GetScale(m_Avatar->GetCanonicalToWorldTransform()->GetMatrix(), wbscale);
  m_DeltaTransform->Scale(wscale[0], wscale[1], wscale[2], POST_MULTIPLY);
  m_DeltaTransform->Scale(wbscale[0], wbscale[1], wbscale[2], POST_MULTIPLY);

  // the initial tracker pose in world coordinates is computed at each update to take into account
  // the changes of the tracker to canonical and canonical to world transforms (e.g. when the camera moves)
  m_TmpTransform->SetMatrix(*m_StartTrackerPoseMatrix);
  m_Tracker->TrackerToCanonical(m_TmpTransform);
  m_Avatar->CanonicalToWorld(m_TmpTransform);

  // only the rotation of the initial pose is used
  float startOrientation[3];
  m_TmpTransform->GetOrientation(startOrientation);

  m_DeltaTransform->RotateY(startOrientation[1], POST_MULTIPLY);
  m_DeltaTransform->RotateX(startOrientation[0], POST_MULTIPLY);
  m_DeltaTransform->RotateZ(startOrientation[2], POST_MULTIPLY);

  m_DeltaTransform->RotateY(-startOrientation[1], PRE_MULTIPLY);
  m_DeltaTransform->RotateX(-startOrientation[0], PRE_MULTIPLY);
  m_DeltaTransform->RotateZ(-startOrientation[2], PRE_MULTIPLY);
}

//------------------------------------------------------------------------------
void albaCameraTrackerTool::UpdateCamera()
{
  UpdateDeltaTransform();

  double XYZ[3];
  m_DeltaTransform->GetPosition(XYZ);

  double rotXYZ[3];
  m_DeltaTransform->GetOrientation(rotXYZ);

  vtkALBASmartPointer<vtkTransform> trans;
  trans->PreMultiply();
  trans->Translate(m_StartFocalPoint[0], m_StartFocalPoint[1], m_StartFocalPoint[2]);
  trans->RotateY(-rotXYZ[1]);
  trans->RotateX(-rotXYZ[0]);
  trans->RotateZ(-rotXYZ[2]);
  trans->Translate(-m_StartFocalPoint[0], -m_StartFocalPoint[1], -m_StartFocalPoint[2]);

  // translation projected along the view up and along the horizontal axis of the camera
  double vup[3];
  m_CurrentCamera->GetViewUp(vup);
  double upTransMod = vtkMath::Dot(XYZ, vup);

  double direction[3];
  m_CurrentCamera->GetViewPlaneNormal(direction);
  double xVect[3];
  vtkMath::Cross(vup, direction, xVect);
  vtkMath::Normalize(xVect);
  double horizontalTransMod = vtkMath::Dot(XYZ, xVect);

  vtkALBASmartPointer<vtkTransform> trans2;
  trans2->DeepCopy(trans);
  trans2->Translate(-upTransMod * vup[0], -upTransMod * vup[1], -upTransMod * vup[2]);
  trans2->Translate(-horizontalTransMod * xVect[0], -horizontalTransMod * xVect[1], -horizontalTransMod * xVect[2]);

  trans->Translate(-XYZ[0], -XYZ[1], -XYZ[2]);

  double posNew[4], fpNew[4], vuOld[4], vuNew[4];
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

  // the motion is incremental: store the new camera and tracker poses
  for (int i = 0; i < 3; i++)
  {
    m_StartCameraPosition[i] = posNew[i];
    m_StartFocalPoint[i] = fpNew[i];
    m_StartViewUp[i] = vuNew[i];
  }
  TrackerSnapshot(m_TrackerPoseMatrix);
}
