/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolSegmentationEdit
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

#include "albaToolSegmentationEdit.h"
#include "albaRWIBase.h"
#include "albaEvent.h"
#include "albaPickHelper.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaToolSegmentationEdit);

//------------------------------------------------------------------------------
bool albaToolSegmentationEdit::OnMoveImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  // the position is sent even if nothing has been picked, (0, 0, 0) in that case
  bool pickedSomething = albaPickHelper::PickCell(m_View, pos[0], pos[1], m_PickPosition);
  albaEvent pickEvent(this, MOUSE_MOVE);
  pickEvent.SetPointer(m_PickPosition);
  pickEvent.SetArg(pickedSomething);
  pickEvent.SetBool(m_CtrlDown);
  albaEventMacro(pickEvent);
  return true;
}

//------------------------------------------------------------------------------
bool albaToolSegmentationEdit::OnWheelImpl(albaRWIBase *rwi, vtkRenderer *ren, double pos[2], double rotation)
{
  if (!m_ShiftDown)
    return false;

  albaEvent scrollEvent(this, MOUSE_WHEEL, (long)rotation);
  scrollEvent.SetPointer(m_PickPosition);
  albaEventMacro(scrollEvent);
  return true;
}
