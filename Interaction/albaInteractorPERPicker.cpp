/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorPERPicker
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

#include "albaInteractorPERPicker.h"
#include "albaPickHelper.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaEventInteraction.h"
#include "albaVME.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractorPERPicker)

//----------------------------------------------------------------------------
bool albaInteractorPERPicker::OnLeftButtonDown(albaEventInteraction *e)
{
  // only the mouse picks, and SHIFT leaves the button to the PER
  m_Dragged = false;
  return albaDeviceButtonsPadMouse::SafeDownCast((albaDevice *)e->GetSender()) && !e->GetModifier(ALBA_SHIFT_KEY);
}

//----------------------------------------------------------------------------
bool albaInteractorPERPicker::OnLeftButtonUp(albaEventInteraction *e)
{
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::SafeDownCast((albaDevice *)e->GetSender());
  if (mouse == NULL)
    return false;
  if (m_Dragged)
    return true;

  // gizmos are not picked
  double mousePos[2], pickedPos[3], scalar;
  albaVME *pickedVme;
  vtkIdType pointId;
  e->Get2DPosition(mousePos);
  if (albaPickHelper::PickPoint(mouse->GetView(), mousePos, NULL, pickedPos, pickedVme, pointId, scalar) && !pickedVme->IsA("albaVMEGizmo"))
    SendPickedPointEvent(VME_PICKED, pickedPos, pointId, scalar, pickedVme);
  return true;
}
