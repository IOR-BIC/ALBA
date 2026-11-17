/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSegmentationThresholdTool
 Authors: Gianluigi Crimi, from albaInteractorPERScalarInformation

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

#include "albaSegmentationThresholdTool.h"
#include "albaEventInteraction.h"
#include "albaPickHelper.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaSegmentationThresholdTool);

//------------------------------------------------------------------------------
bool albaSegmentationThresholdTool::OnMove(albaEventInteraction *e)
{
  if (!IsOnToolView(e))
    return false;

  double mousePos[2];
  e->Get2DPosition(mousePos);

  double pickedPos[3];
  albaVME *pickedVME = NULL;
  if (albaPickHelper::PickCell(m_View, mousePos[0], mousePos[1], pickedPos, &pickedVME) && pickedVME)
  {
    // the picked position can be slightly outside the data bounds because of approximation errors,
    // so it is clamped into the bounds to find the nearest point
    double scalarValue;
    vtkIdType pid = albaPickHelper::GetPointScalar(pickedVME, pickedPos, scalarValue, true);
    SendPickedPointEvent(MOUSE_MOVE, pickedPos, pid, scalarValue);
  }
  return true;
}
