/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVTKPickerTool
 Authors: Gianluigi Crimi, from albaInteractorVTKPicker by Josef Kohout

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

#include "albaVTKPickerTool.h"
#include "albaEvent.h"
#include "albaEventInteraction.h"

#include "vtkALBASmartPointer.h"
#include "vtkCamera.h"
#include "vtkCellPicker.h"
#include "vtkMath.h"
#include "vtkPoints.h"
#include "vtkRenderer.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaVTKPickerTool);

//------------------------------------------------------------------------------
albaVTKPickerTool::albaVTKPickerTool()
{
  m_Picker = NULL;
  m_PickRenderer = NULL;
  m_ContinuousPicking = false;
}

//------------------------------------------------------------------------------
albaVTKPickerTool::~albaVTKPickerTool()
{
  vtkDEL(m_Picker);
}

//------------------------------------------------------------------------------
bool albaVTKPickerTool::OnLeftButtonDown(albaEventInteraction *e)
{
  vtkRenderer *renderer = GetEventRenderer(e);
  if (renderer == NULL || !e->GetModifier(ALBA_CTRL_KEY))
    return false;

  // a picking not ended by a button up is ended here
  if (m_Picker != NULL)
  {
    albaEventMacro(albaEvent(this, VME_PICKED, m_Picker));
    vtkDEL(m_Picker);
  }

  double mousePos[2];
  e->Get2DPosition(mousePos);

  vtkNEW(m_Picker);
  m_Picker->SetTolerance(0.001);
  if (m_Picker->Pick((int)mousePos[0], (int)mousePos[1], 0, renderer) == 0)
  {
    // nothing was picked
    vtkDEL(m_Picker);
  }
  else
  {
    m_PickRenderer = renderer;
    albaEvent ev(this, VME_PICKING, m_Picker);
    ev.SetBool(false);
    albaEventMacro(ev);
  }
  return true;
}

//------------------------------------------------------------------------------
bool albaVTKPickerTool::OnLeftButtonUp(albaEventInteraction *e)
{
  if (m_Picker != NULL)
  {
    albaEventMacro(albaEvent(this, VME_PICKED, m_Picker));
    vtkDEL(m_Picker);
  }
  return true;
}

//------------------------------------------------------------------------------
bool albaVTKPickerTool::OnDrag(albaEventInteraction *e)
{
  if (m_Picker == NULL || !m_ContinuousPicking)
    return true;

  double mousePos[2];
  e->Get2DPosition(mousePos);

  // compute the new position of the picked point, at the same distance from the camera
  double cameraPos[3], pointOrigPos[3], pointNewPos[3];
  vtkCamera *camera = m_PickRenderer->GetActiveCamera();
  camera->GetPosition(cameraPos);

  // convert the current 2d selection point into world coordinates
  double cameraFP[4]; cameraFP[3] = 1.0;
  camera->GetFocalPoint(cameraFP);
  m_PickRenderer->SetWorldPoint(cameraFP);
  m_PickRenderer->WorldToDisplay(); //project focal point

  double selPoint[3];
  selPoint[0] = (int)mousePos[0];
  selPoint[1] = (int)mousePos[1];
  selPoint[2] = m_PickRenderer->GetDisplayPoint()[2];
  m_PickRenderer->SetDisplayPoint(selPoint);
  m_PickRenderer->DisplayToWorld();

  // convert coordinates from homogeneous coordinates to Cartesian and create a vector from cameraPos to this point
  double *wcoords = m_PickRenderer->GetWorldPoint();
  for (int i = 0; i < 3; i++)
    pointNewPos[i] = (wcoords[i] / wcoords[3]) - cameraPos[i];
  vtkMath::Normalize(pointNewPos);

  // get the original picked position and its distance from the origin of projection
  m_Picker->GetPickedPositions()->GetPoint(0, pointOrigPos);
  double dblDist = sqrt(vtkMath::Distance2BetweenPoints(cameraPos, pointOrigPos));
  for (int i = 0; i < 3; i++)
    pointNewPos[i] = cameraPos[i] + pointNewPos[i]*dblDist;

  vtkALBASmartPointer<vtkPoints> points;
  points->InsertNextPoint(pointNewPos);
  albaEvent ev(this, VME_PICKING, points);
  ev.SetBool(true); // continuous picking
  albaEventMacro(ev);
  return true;
}
