/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolSelectCell
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

#include "albaToolSelectCell.h"

#include "vtkCellPicker.h"
#include "vtkPoints.h"
#include "vtkRenderer.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaToolSelectCell);

//------------------------------------------------------------------------------
albaToolSelectCell::albaToolSelectCell()
{
  m_IsPicking = false;
}

//------------------------------------------------------------------------------
bool albaToolSelectCell::OnLeftButtonDown(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  // outside the cells the button goes to the camera (CTRL: spin)
  if (!m_CtrlDown || !PickCell(ren, pos))
    return false;

  m_IsPicking = true;
  return true;
}

//------------------------------------------------------------------------------
bool albaToolSelectCell::OnLeftButtonUp(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  m_IsPicking = false;
  return true;
}

//------------------------------------------------------------------------------
bool albaToolSelectCell::OnDrag(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  if (!m_IsPicking)
    return false;

  PickCell(ren, pos);
  return true;
}

//------------------------------------------------------------------------------
bool albaToolSelectCell::PickCell(vtkRenderer *ren, double pos[2])
{
  if (!IsOnToolRenderWindow(ren))
    return false;

  vtkCellPicker *cellPicker;
  vtkNEW(cellPicker);
  cellPicker->SetTolerance(0.001);
  bool picked = cellPicker->Pick((int)pos[0], (int)pos[1], 0, ren) != 0;
  if (picked)
  {
    double pickedPos[3];
    cellPicker->GetPickedPositions()->GetPoint(0, pickedPos);
    SendPickedPointEvent(VME_PICKED, pickedPos, cellPicker->GetCellId());
  }
  vtkDEL(cellPicker);
  return picked;
}
