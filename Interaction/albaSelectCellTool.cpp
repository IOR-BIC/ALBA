/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSelectCellTool
 Authors: Gianluigi Crimi, from albaInteractorSelectCell by Stefano Perticoni

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

#include "albaSelectCellTool.h"
#include "albaEventInteraction.h"

#include "vtkCellPicker.h"
#include "vtkPoints.h"
#include "vtkRenderer.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaSelectCellTool);

//------------------------------------------------------------------------------
albaSelectCellTool::albaSelectCellTool()
{
  m_IsPicking = false;
}

//------------------------------------------------------------------------------
bool albaSelectCellTool::OnLeftButtonDown(albaEventInteraction *e)
{
  // outside the cells the button goes to the camera (CTRL: spin)
  if (!e->GetModifier(ALBA_CTRL_KEY) || !PickCell(e))
    return false;

  m_IsPicking = true;
  return true;
}

//------------------------------------------------------------------------------
bool albaSelectCellTool::OnLeftButtonUp(albaEventInteraction *e)
{
  m_IsPicking = false;
  return true;
}

//------------------------------------------------------------------------------
bool albaSelectCellTool::OnDrag(albaEventInteraction *e)
{
  if (!m_IsPicking)
    return false;

  PickCell(e);
  return true;
}

//------------------------------------------------------------------------------
bool albaSelectCellTool::PickCell(albaEventInteraction *e)
{
  vtkRenderer *renderer = GetEventRenderer(e);
  if (renderer == NULL)
    return false;

  double mousePos[2];
  e->Get2DPosition(mousePos);

  vtkCellPicker *cellPicker;
  vtkNEW(cellPicker);
  cellPicker->SetTolerance(0.001);
  bool picked = cellPicker->Pick((int)mousePos[0], (int)mousePos[1], 0, renderer) != 0;
  if (picked)
  {
    double pickedPos[3];
    cellPicker->GetPickedPositions()->GetPoint(0, pickedPos);
    SendPickedPointEvent(VME_PICKED, pickedPos, cellPicker->GetCellId());
  }
  vtkDEL(cellPicker);
  return picked;
}
