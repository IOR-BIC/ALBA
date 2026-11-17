/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSelectPointTool
 Authors: Gianluigi Crimi, from albaInteractorSelectPoint by Matteo Giacomoni, Stefano Perticoni

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

#include "albaSelectPointTool.h"
#include "albaEventInteraction.h"

#include "vtkPointPicker.h"
#include "vtkRenderer.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaSelectPointTool);

//------------------------------------------------------------------------------
albaSelectPointTool::albaSelectPointTool()
{
  m_IsPicking = false;
  m_UseCtrlModifier = true;
}

//------------------------------------------------------------------------------
bool albaSelectPointTool::OnLeftButtonDown(albaEventInteraction *e)
{
  // when no point is picked the button goes to the camera
  if ((m_UseCtrlModifier && !e->GetModifier(ALBA_CTRL_KEY)) || !PickPoint(e))
    return false;

  m_IsPicking = true;
  return true;
}

//------------------------------------------------------------------------------
bool albaSelectPointTool::OnLeftButtonUp(albaEventInteraction *e)
{
  m_IsPicking = false;
  return true;
}

//------------------------------------------------------------------------------
bool albaSelectPointTool::OnDrag(albaEventInteraction *e)
{
  return m_IsPicking;
}

//------------------------------------------------------------------------------
bool albaSelectPointTool::PickPoint(albaEventInteraction *e)
{
  vtkRenderer *renderer = GetEventRenderer(e);
  if (renderer == NULL)
    return false;

  double mousePos[2];
  e->Get2DPosition(mousePos);

  vtkPointPicker *pointPicker;
  vtkNEW(pointPicker);
  pointPicker->SetTolerance(0.01);
  bool picked = pointPicker->Pick((int)mousePos[0], (int)mousePos[1], 0, renderer) != 0;
  if (picked)
  {
    double pickedPos[3];
    pointPicker->GetPickPosition(pickedPos);
    SendPickedPointEvent(VME_PICKED, pickedPos, pointPicker->GetPointId());
  }
  vtkDEL(pointPicker);
  return picked;
}
