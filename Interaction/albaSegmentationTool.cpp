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
#include "albaEvent.h"
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
  m_PickPosition[0] = m_PickPosition[1] = m_PickPosition[2] = 0;
}

//------------------------------------------------------------------------------
bool albaSegmentationTool::OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2])
{
  if (!IsOnToolView(view))
    return false;

  long modifier = 0;
  if (m_CtrlDown)
    modifier = ALBA_CTRL_KEY;
  else if (m_AltDown)
    modifier = ALBA_ALT_KEY;

  m_IsPicking = SendPickingInformation(pos, modifier, VME_PICKED);
  if (m_IsPicking)
    SendPickingInformation(pos, 0, VME_PICKING);
  return true;
}

//------------------------------------------------------------------------------
bool albaSegmentationTool::OnLeftButtonUp(albaView *view, vtkRenderer *ren, double pos[2])
{
  m_IsPicking = false;
  albaEventMacro(albaEvent(this, MOUSE_UP));
  return true;
}

//------------------------------------------------------------------------------
bool albaSegmentationTool::OnMiddleButtonDown(albaView *view, vtkRenderer *ren, double pos[2])
{
  // the camera moves with the drags
  return IsOnToolView(view);
}

//------------------------------------------------------------------------------
bool albaSegmentationTool::OnRightButtonDown(albaView *view, vtkRenderer *ren, double pos[2])
{
  // the camera moves with the drags
  return IsOnToolView(view);
}

//------------------------------------------------------------------------------
bool albaSegmentationTool::OnDrag(albaView *view, vtkRenderer *ren, double pos[2])
{
  if (m_IsPicking)
    SendPickingInformation(pos, 0, VME_PICKING);
  return false;
}

//------------------------------------------------------------------------------
bool albaSegmentationTool::OnKey(int keyCode, bool pressed)
{
  albaEvent keyEvent(this, pressed ? KEY_PRESSED : KEY_RELEASED, (long)keyCode);
  keyEvent.SetBool(false);
  albaEventMacro(keyEvent);
  return keyEvent.GetBool();
}

//------------------------------------------------------------------------------
bool albaSegmentationTool::SendPickingInformation(double pos[2], long modifier, int eventId)
{
  // position and VME come from the same picking; m_PickPosition changes only when something is picked
  albaVME *pickedVME = NULL;
  double pickedPos[3];
  if (albaPickHelper::PickCell(m_View, pos[0], pos[1], pickedPos, &pickedVME))
  {
    m_PickPosition[0] = pickedPos[0];
    m_PickPosition[1] = pickedPos[1];
    m_PickPosition[2] = pickedPos[2];
  }

  if (pickedVME == NULL)
    return false;

  albaEvent pickEvent(this, eventId, modifier);
  pickEvent.SetPointer(m_PickPosition);
  albaEventMacro(pickEvent);
  return true;
}
