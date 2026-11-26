/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolCamera
 Authors: Gianluigi Crimi, from albaInteractorCameraMove by Paolo Quadrani & Marco Petrone

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

#include "albaToolCamera.h"
#include "albaRWIBase.h"
#include "albaView.h"

#include "vtkCamera.h"
#include "vtkMath.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindow.h"

#define MAGNET_ANGLE 2

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaToolCamera);

//------------------------------------------------------------------------------
albaToolCamera::albaToolCamera()
{
  m_Interacting = false;
  m_CurrentCamera = NULL;
  m_Motion = MOTION_NONE;
  m_MousePose[0] = m_MousePose[1] = 0;
  m_LastMousePose[0] = m_LastMousePose[1] = 0;
  m_MotionFactor = 10.0;
  m_AutoResetClippingRange = true;
}

//------------------------------------------------------------------------------
bool albaToolCamera::OnLeftButtonDown(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  if (m_ShiftDown)
    return Start(rwi, ren, pos, m_CtrlDown ? MOTION_DOLLY : MOTION_PAN);
  return Start(rwi, ren, pos, m_CtrlDown ? MOTION_SPIN : MOTION_ROTATE);
}

//------------------------------------------------------------------------------
bool albaToolCamera::Start(albaRWIBase *rwi, vtkRenderer *ren, double pos[2], int motion)
{
  if (m_Interacting)
    return false;

  m_Interacting = true;
  m_Motion = motion;

  m_LastMousePose[0] = m_MousePose[0] = (int)pos[0];
  m_LastMousePose[1] = m_MousePose[1] = (int)pos[1];

  SetRendererAndView(ren, rwi->GetView());
  m_CurrentCamera = m_Renderer ? m_Renderer->GetActiveCamera() : NULL;
  return true;
}

//------------------------------------------------------------------------------
bool albaToolCamera::Stop()
{
  if (!m_Interacting)
    return false;

  m_Interacting = false;
  m_Motion = MOTION_NONE;

  if (m_Renderer)
  {
    m_Renderer->GetRenderWindow()->SetDesiredUpdateRate(0.001);
    m_Renderer->GetRenderWindow()->Render();
  }
  return true;
}

//------------------------------------------------------------------------------
bool albaToolCamera::OnDrag(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  if (!m_Interacting)
    return false;

  if (m_CurrentCamera == NULL)
    return true;

  m_MousePose[0] = (int)pos[0];
  m_MousePose[1] = (int)pos[1];

  m_Renderer->GetRenderWindow()->SetDesiredUpdateRate(15.0);

  switch (m_Motion)
  {
    case MOTION_ROTATE: Rotate(); break;
    case MOTION_PAN:    Pan();    break;
    case MOTION_DOLLY:  Dolly();  break;
    case MOTION_SPIN:   Spin();   break;
  }

  m_LastMousePose[0] = m_MousePose[0];
  m_LastMousePose[1] = m_MousePose[1];
  return true;
}

//------------------------------------------------------------------------------
bool albaToolCamera::OnWheel(albaRWIBase *rwi, vtkRenderer *ren, double pos[2], double rotation)
{
  SetRendererAndView(ren, rwi->GetView());
  if (m_Renderer == NULL)
    return true;
  m_CurrentCamera = m_Renderer->GetActiveCamera();

  double zoomFactor = 1.0 + (rotation / 1200.0);

  if (m_CurrentCamera->GetParallelProjection())
    m_CurrentCamera->SetParallelScale(m_CurrentCamera->GetParallelScale() / zoomFactor);
  else
    m_CurrentCamera->Dolly(zoomFactor);

  CameraChanged();
  return true;
}

//------------------------------------------------------------------------------
void albaToolCamera::Rotate()
{
  if (m_CurrentCamera->GetParallelProjection())
    return;

  int dx = m_MousePose[0] - m_LastMousePose[0];
  int dy = m_MousePose[1] - m_LastMousePose[1];

  int *size = m_Renderer->GetRenderWindow()->GetSize();
  double deltaElevation = -20.0 / size[1];
  double deltaAzimuth = -20.0 / size[0];

  m_CurrentCamera->Azimuth((double)dx * deltaAzimuth * m_MotionFactor);
  m_CurrentCamera->Elevation((double)dy * deltaElevation * m_MotionFactor);
  m_CurrentCamera->OrthogonalizeViewUp();

  CameraChanged();
}

//------------------------------------------------------------------------------
void albaToolCamera::Spin()
{
  if (m_View == NULL || !m_View->GetCanSpin())
    return;

  double *center = m_Renderer->GetCenter();

  double newAngle = vtkMath::DegreesFromRadians(atan2((double)m_MousePose[1] - center[1], (double)m_MousePose[0] - center[0]));
  double oldAngle = vtkMath::DegreesFromRadians(atan2((double)m_LastMousePose[1] - center[1], (double)m_LastMousePose[0] - center[0]));
  double angleDiff = newAngle - oldAngle;

  double cameraRoll = m_CurrentCamera->GetRoll() + 180.0;
  double predictedRoll = cameraRoll + angleDiff;
  if (predictedRoll > 360)
    predictedRoll -= 360;
  if (predictedRoll < 0)
    predictedRoll += 360;

  // near the multiples of 90 degrees the roll snaps ("magnet"): the mouse pose
  // is kept at the last one so that the mouse movements cumulate
  double rollAngle = angleDiff;
  bool magnet = true;
  if (predictedRoll > 90 - MAGNET_ANGLE && predictedRoll < 90 + MAGNET_ANGLE)
    rollAngle = 90 - cameraRoll;
  else if (predictedRoll > 180 - MAGNET_ANGLE && predictedRoll < 180 + MAGNET_ANGLE)
    rollAngle = 180 - cameraRoll;
  else if (predictedRoll > 270 - MAGNET_ANGLE && predictedRoll < 270 + MAGNET_ANGLE)
    rollAngle = 270 - cameraRoll;
  else if (predictedRoll > 360 - MAGNET_ANGLE)
    rollAngle = 360 - cameraRoll;
  else if (predictedRoll < 0 + MAGNET_ANGLE)
    rollAngle = -cameraRoll;
  else
    magnet = false;

  if (magnet)
  {
    m_MousePose[0] = m_LastMousePose[0];
    m_MousePose[1] = m_LastMousePose[1];
  }

  m_CurrentCamera->Roll(rollAngle);
  m_CurrentCamera->OrthogonalizeViewUp();

  CameraChanged();
}

//------------------------------------------------------------------------------
void albaToolCamera::Pan()
{
  double viewFocus[4], focalDepth, viewPoint[3];
  double newPickPoint[4], oldPickPoint[4], motionVector[3];

  m_CurrentCamera->GetFocalPoint(viewFocus);
  ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2], viewFocus);
  focalDepth = viewFocus[2];

  ComputeDisplayToWorld((double)m_MousePose[0], (double)m_MousePose[1], focalDepth, newPickPoint);
  ComputeDisplayToWorld((double)m_LastMousePose[0], (double)m_LastMousePose[1], focalDepth, oldPickPoint);

  // camera motion is reversed
  motionVector[0] = oldPickPoint[0] - newPickPoint[0];
  motionVector[1] = oldPickPoint[1] - newPickPoint[1];
  motionVector[2] = oldPickPoint[2] - newPickPoint[2];

  m_CurrentCamera->GetFocalPoint(viewFocus);
  m_CurrentCamera->GetPosition(viewPoint);
  m_CurrentCamera->SetFocalPoint(motionVector[0] + viewFocus[0], motionVector[1] + viewFocus[1], motionVector[2] + viewFocus[2]);
  m_CurrentCamera->SetPosition(motionVector[0] + viewPoint[0], motionVector[1] + viewPoint[1], motionVector[2] + viewPoint[2]);

  CameraChanged();
}

//------------------------------------------------------------------------------
void albaToolCamera::Dolly()
{
  double *center = m_Renderer->GetCenter();

  int dy = m_MousePose[1] - m_LastMousePose[1];
  double dyf = m_MotionFactor * (double)(dy) / center[1];
  double zoomFactor = pow(1.1, dyf);

  if (m_CurrentCamera->GetParallelProjection())
    m_CurrentCamera->SetParallelScale(m_CurrentCamera->GetParallelScale() / zoomFactor);
  else
    m_CurrentCamera->Dolly(zoomFactor);

  CameraChanged();
}

//------------------------------------------------------------------------------
void albaToolCamera::CameraChanged()
{
  if (m_AutoResetClippingRange)
    ResetClippingRange();

  m_Renderer->GetRenderWindow()->Render();
}

//------------------------------------------------------------------------------
void albaToolCamera::ResetClippingRange()
{
  vtkRendererCollection *rc = m_Renderer->GetRenderWindow()->GetRenderers();
  rc->InitTraversal();

  // Layers can be ONE or TWO; the third layer ALWAYS_VISIBLE doesn't count, as implemented in albaRWI.
  int numberOfLayers = m_Renderer->GetRenderWindow()->GetNumberOfLayers();

  // The order of the renderers has been set in albaRWI:
  // always visible renderer, front renderer and, only with 3 layers, back renderer
  // (otherwise the third one can be a renderer added by vtkALBAOrientationWidget)
  vtkRenderer *rAV = rc->GetNextItem();
  vtkRenderer *rFR = rc->GetNextItem();
  vtkRenderer *rBR = (numberOfLayers == 3) ? rc->GetNextItem() : NULL;

  double b1[6], b2[6], b3[6], b[6];
  if (rFR == NULL)
    return;

  if (rAV == NULL && rBR == NULL)
  {
    rFR->ResetCameraClippingRange();
    return;
  }

  if (rAV)
  {
    // the always visible layer has default bounds (-1,1,-1,1,-1,1):
    // the clipping range must be computed with the bounds of the front renderer
    rFR->ComputeVisiblePropBounds(b1);
    rAV->ComputeVisiblePropBounds(b2);

    if (b1[0] == VTK_FLOAT_MAX && b2[0] == VTK_FLOAT_MAX)
    {
      for (int i = 0; i < 6; i++)
        b[i] = b1[i];
      rFR->ResetCameraClippingRange();
    }
    else if (b1[0] == VTK_FLOAT_MAX)
    {
      for (int i = 0; i < 6; i++)
        b[i] = b2[i];
      rFR->ResetCameraClippingRange(b2);
    }
    else
    {
      // WORKAROUND: if the only actor shown is the axis actor in the bottom left corner it is not considered,
      // otherwise there are other actors (like the gizmos) to take into account
      bool onlyAxes = b2[0] == -1 && b2[1] == 1 && b2[2] == -1 && b2[3] == 1 && b2[4] == -1 && b2[5] == 1;
      for (int i = 0; i < 6; i += 2)
      {
        b[i] = (onlyAxes || b1[i] < b2[i]) ? b1[i] : b2[i];
        b[i + 1] = (onlyAxes || b1[i + 1] > b2[i + 1]) ? b1[i + 1] : b2[i + 1];
      }
      rFR->ResetCameraClippingRange(b);
    }

    // the clipping range must also include the bounds of the back renderer
    if (rBR)
    {
      rBR->ComputeVisiblePropBounds(b3);
      if (b3[0] != VTK_FLOAT_MAX)
      {
        for (int i = 0; i < 6; i += 2)
        {
          b[i] = (b[i] < b3[i]) ? b[i] : b3[i];
          b[i + 1] = (b[i + 1] > b3[i + 1]) ? b[i + 1] : b3[i + 1];
        }
        rFR->ResetCameraClippingRange(b);
      }
    }
  }
}
