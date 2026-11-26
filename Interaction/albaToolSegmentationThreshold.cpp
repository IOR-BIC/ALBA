/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolSegmentationThreshold
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

#include "albaToolSegmentationThreshold.h"
#include "albaRWIBase.h"
#include "albaPickHelper.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaToolSegmentationThreshold);

//------------------------------------------------------------------------------
bool albaToolSegmentationThreshold::OnMove(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  if (!IsOnToolView(rwi->GetView()))
    return false;

  double pickedPos[3];
  albaVME *pickedVME = NULL;
  if (albaPickHelper::PickCell(m_View, pos[0], pos[1], pickedPos, &pickedVME) && pickedVME)
  {
    // the picked position can be slightly outside the data bounds because of approximation errors,
    // so it is clamped into the bounds to find the nearest point
    double scalarValue;
    vtkIdType pid = albaPickHelper::GetPointScalar(pickedVME, pickedPos, scalarValue, true);
    SendPickedPointEvent(MOUSE_MOVE, pickedPos, pid, scalarValue);
  }
  return true;
}
