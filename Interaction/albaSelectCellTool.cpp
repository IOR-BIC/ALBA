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
  if (!e->GetModifier(ALBA_CTRL_KEY) || GetEventRenderer(e) == NULL)
    return false;

  m_IsPicking = true;
  PickCell(e);
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
void albaSelectCellTool::PickCell(albaEventInteraction *e)
{
  vtkRenderer *renderer = GetEventRenderer(e);
  if (renderer == NULL)
    return;

  double mousePos[2];
  e->Get2DPosition(mousePos);

  vtkCellPicker *cellPicker;
  vtkNEW(cellPicker);
  cellPicker->SetTolerance(0.001);
  if (cellPicker->Pick((int)mousePos[0], (int)mousePos[1], 0, renderer))
  {
    double pickedPos[3];
    cellPicker->GetPickedPositions()->GetPoint(0, pickedPos);
    SendPickedPointEvent(VME_PICKED, pickedPos, cellPicker->GetCellId());
  }
  vtkDEL(cellPicker);
}
