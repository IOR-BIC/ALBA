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
  if ((m_UseCtrlModifier && !e->GetModifier(ALBA_CTRL_KEY)) || GetEventRenderer(e) == NULL)
    return false;

  m_IsPicking = true;
  PickPoint(e);
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
void albaSelectPointTool::PickPoint(albaEventInteraction *e)
{
  vtkRenderer *renderer = GetEventRenderer(e);
  if (renderer == NULL)
    return;

  double mousePos[2];
  e->Get2DPosition(mousePos);

  vtkPointPicker *pointPicker;
  vtkNEW(pointPicker);
  pointPicker->SetTolerance(0.01);
  if (pointPicker->Pick((int)mousePos[0], (int)mousePos[1], 0, renderer))
  {
    double pickedPos[3];
    pointPicker->GetPickPosition(pickedPos);
    SendPickedPointEvent(VME_PICKED, pickedPos, pointPicker->GetPointId());
  }
  vtkDEL(pointPicker);
}
