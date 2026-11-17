/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSegmentationTool
 Authors: Gianluigi Crimi, from albaInteractorSegmentationPicker

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

#include "albaSegmentationTool.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaEvent.h"
#include "albaEventInteraction.h"
#include "albaPickHelper.h"
#include "albaView.h"
#include "mmuIdFactory.h"

//------------------------------------------------------------------------------
ALBA_ID_IMP(albaSegmentationTool::KEY_PRESSED);
ALBA_ID_IMP(albaSegmentationTool::KEY_RELEASED);

//------------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaSegmentationTool);

//------------------------------------------------------------------------------
albaSegmentationTool::albaSegmentationTool()
{
  m_IsPicking = false;
  m_CtrlDown = m_AltDown = m_ShiftDown = false;
  m_PickPosition[0] = m_PickPosition[1] = m_PickPosition[2] = 0;
}

//------------------------------------------------------------------------------
bool albaSegmentationTool::IsOnToolView(albaEventInteraction *e)
{
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::SafeDownCast((albaDevice *)e->GetSender());
  return mouse && m_View && mouse->GetView() == m_View;
}

//------------------------------------------------------------------------------
bool albaSegmentationTool::OnLeftButtonDown(albaEventInteraction *e)
{
  if (!IsOnToolView(e))
    return false;

  long modifier = 0;
  if (e->GetModifier(ALBA_CTRL_KEY))
    modifier = ALBA_CTRL_KEY;
  else if (e->GetModifier(ALBA_ALT_KEY))
    modifier = ALBA_ALT_KEY;

  m_IsPicking = SendPickingInformation(e, modifier, VME_PICKED);
  if (m_IsPicking)
    SendPickingInformation(e, 0, VME_PICKING);
  return true;
}

//------------------------------------------------------------------------------
bool albaSegmentationTool::OnLeftButtonUp(albaEventInteraction *e)
{
  m_IsPicking = false;
  albaEventMacro(albaEvent(this, MOUSE_UP));
  return true;
}

//------------------------------------------------------------------------------
bool albaSegmentationTool::OnMiddleButtonDown(albaEventInteraction *e)
{
  // the camera moves with the drags
  return IsOnToolView(e);
}

//------------------------------------------------------------------------------
bool albaSegmentationTool::OnRightButtonDown(albaEventInteraction *e)
{
  // the camera moves with the drags
  return IsOnToolView(e);
}

//------------------------------------------------------------------------------
bool albaSegmentationTool::OnDrag(albaEventInteraction *e)
{
  if (m_IsPicking)
    SendPickingInformation(e, 0, VME_PICKING);
  return false;
}

//------------------------------------------------------------------------------
bool albaSegmentationTool::OnKey(int keyCode, bool pressed)
{
  if (keyCode == WXK_CONTROL)
    m_CtrlDown = pressed;
  else if (keyCode == WXK_ALT)
    m_AltDown = pressed;
  else if (keyCode == WXK_SHIFT)
    m_ShiftDown = pressed;

  albaEvent keyEvent(this, pressed ? KEY_PRESSED : KEY_RELEASED, (long)keyCode);
  keyEvent.SetBool(false);
  albaEventMacro(keyEvent);
  return keyEvent.GetBool();
}

//------------------------------------------------------------------------------
bool albaSegmentationTool::SendPickingInformation(albaEventInteraction *e, long modifier, int eventId)
{
  double mousePos[2];
  e->Get2DPosition(mousePos);

  // position and VME come from the same picking; m_PickPosition changes only when something is picked
  albaVME *pickedVME = NULL;
  double pos[3];
  if (albaPickHelper::PickCell(m_View, mousePos[0], mousePos[1], pos, &pickedVME))
  {
    m_PickPosition[0] = pos[0];
    m_PickPosition[1] = pos[1];
    m_PickPosition[2] = pos[2];
  }

  if (pickedVME == NULL)
    return false;

  albaEvent pickEvent(this, eventId, modifier);
  pickEvent.SetPointer(m_PickPosition);
  albaEventMacro(pickEvent);
  return true;
}
