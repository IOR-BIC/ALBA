/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolPicker
 Authors: Nicola Vanella
 
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

#include "albaToolPicker.h"
#include "albaPickHelper.h"
#include "albaVME.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaToolPicker)

//----------------------------------------------------------------------------
bool albaToolPicker::OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2])
{
  // SHIFT leaves the button to the PER
  m_Dragged = false;
  return !m_ShiftDown;
}

//----------------------------------------------------------------------------
bool albaToolPicker::OnLeftButtonUp(albaView *view, vtkRenderer *ren, double pos[2])
{
  if (m_Dragged)
    return true;

  // gizmos are not picked
  double pickedPos[3], scalar;
  albaVME *pickedVme;
  vtkIdType pointId;
  if (albaPickHelper::PickPoint(view, pos, pickedPos, pickedVme, pointId, scalar) && !pickedVme->IsA("albaVMEGizmo"))
    SendPickedPointEvent(VME_PICKED, pickedPos, pointId, scalar, pickedVme);
  return true;
}
