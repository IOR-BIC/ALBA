/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSegmentationEditTool
 Authors: Gianluigi Crimi, from albaInteractorPERBrushFeedback

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

#include "albaSegmentationEditTool.h"
#include "albaEvent.h"
#include "albaEventInteraction.h"
#include "albaPickHelper.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaSegmentationEditTool);

//------------------------------------------------------------------------------
bool albaSegmentationEditTool::OnMove(albaEventInteraction *e)
{
  if (!IsOnToolView(e))
    return false;

  double mousePos[2];
  e->Get2DPosition(mousePos);

  // the position is sent even if nothing has been picked, (0, 0, 0) in that case
  bool pickedSomething = albaPickHelper::PickCell(m_View, mousePos[0], mousePos[1], m_PickPosition);
  albaEvent pickEvent(this, MOUSE_MOVE);
  pickEvent.SetPointer(m_PickPosition);
  pickEvent.SetArg(pickedSomething);
  pickEvent.SetBool(e->GetModifier(ALBA_CTRL_KEY));
  albaEventMacro(pickEvent);
  return true;
}

//------------------------------------------------------------------------------
bool albaSegmentationEditTool::OnWheel(albaEventInteraction *e)
{
  if (!IsOnToolView(e) || !e->GetModifier(ALBA_SHIFT_KEY))
    return false;

  long rotation = *(double *)e->GetData();
  albaEvent scrollEvent(this, MOUSE_WHEEL, rotation);
  scrollEvent.SetPointer(m_PickPosition);
  albaEventMacro(scrollEvent);
  return true;
}
