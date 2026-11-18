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
bool albaSelectPointTool::OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2])
{
  // when no point is picked the button goes to the camera
  if ((m_UseCtrlModifier && !m_CtrlDown) || !PickPoint(ren, pos))
    return false;

  m_IsPicking = true;
  return true;
}

//------------------------------------------------------------------------------
bool albaSelectPointTool::OnLeftButtonUp(albaView *view, vtkRenderer *ren, double pos[2])
{
  m_IsPicking = false;
  return true;
}

//------------------------------------------------------------------------------
bool albaSelectPointTool::OnDrag(albaView *view, vtkRenderer *ren, double pos[2])
{
  return m_IsPicking;
}

//------------------------------------------------------------------------------
bool albaSelectPointTool::PickPoint(vtkRenderer *ren, double pos[2])
{
  if (!IsOnToolRenderWindow(ren))
    return false;

  vtkPointPicker *pointPicker;
  vtkNEW(pointPicker);
  pointPicker->SetTolerance(0.01);
  bool picked = pointPicker->Pick((int)pos[0], (int)pos[1], 0, ren) != 0;
  if (picked)
  {
    double pickedPos[3];
    pointPicker->GetPickPosition(pickedPos);
    SendPickedPointEvent(VME_PICKED, pickedPos, pointPicker->GetPointId());
  }
  vtkDEL(pointPicker);
  return picked;
}
