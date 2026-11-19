/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaFlyToTool
 Authors: Gianluigi Crimi, from albaPositionalEventRouter::FlyTo

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

#include "albaFlyToTool.h"
#include "albaAbsLogicManager.h"
#include "albaView.h"

#include "vtkCamera.h"
#include "vtkMath.h"
#include "vtkRenderer.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaFlyToTool);

//------------------------------------------------------------------------------
bool albaFlyToTool::OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2])
{
  return FlyAndPop(view, ren, pos, 1);
}

//------------------------------------------------------------------------------
bool albaFlyToTool::OnRightButtonDown(albaView *view, vtkRenderer *ren, double pos[2])
{
  return FlyAndPop(view, ren, pos, -0.6);
}

//------------------------------------------------------------------------------
bool albaFlyToTool::FlyAndPop(albaView *view, vtkRenderer *ren, double pos[2], double zoom)
{
  FlyTo(view, ren, pos, zoom);

  // one click ends the fly-to mode, also when nothing is picked
  if (GetLogicManager())
    GetLogicManager()->PopTool(this);
  return true;
}

//------------------------------------------------------------------------------
bool albaFlyToTool::FlyTo(albaView *view, vtkRenderer *ren, double pos[2], double zoom, int numSteps)
{
  if (view == NULL || ren == NULL || numSteps < 1 || !view->Pick((int)pos[0], (int)pos[1]))
    return false;

  double picked[3];
  view->GetPickedPosition(picked);

  vtkCamera *cam = ren->GetActiveCamera();
  double f0[3], p0[3]; // start focal point and camera position
  cam->GetFocalPoint(f0);
  cam->GetPosition(p0);

  if (zoom > 1)
    zoom = 1;
  if (zoom < -1)
    zoom = -1;

  // the focal point moves to the picked point, the camera follows it and approaches it by zoom
  double focMotion[3], camMotion[3];
  for (int i = 0; i < 3; i++)
  {
    focMotion[i] = picked[i] - f0[i];
    camMotion[i] = (picked[i] - f0[i]) + (f0[i] - p0[i]) * zoom / 2;
  }

  double f1[3], p1[3];
  for (int step = 1; step <= numSteps; step++)
  {
    for (int j = 0; j < 3; j++)
    {
      f1[j] = f0[j] + focMotion[j] * step / numSteps;
      p1[j] = p0[j] + camMotion[j] * step / numSteps;
    }
    cam->SetFocalPoint(f1);
    cam->SetPosition(p1);
    ren->ResetCameraClippingRange();
    view->CameraUpdate();
  }
  cam->OrthogonalizeViewUp();
  return true;
}
