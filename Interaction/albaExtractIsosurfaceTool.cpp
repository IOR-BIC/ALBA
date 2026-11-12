/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaExtractIsosurfaceTool
 Authors: Gianluigi Crimi, from albaInteractorExtractIsosurface by Paolo Quadrani & Marco Petrone

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

#include "albaExtractIsosurfaceTool.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaEventInteraction.h"
#include "albaRWIBase.h"

#include "vtkAbstractPicker.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaExtractIsosurfaceTool);

//------------------------------------------------------------------------------
albaExtractIsosurfaceTool::albaExtractIsosurfaceTool()
{
  m_PickValue = false;
}

//------------------------------------------------------------------------------
bool albaExtractIsosurfaceTool::OnLeftButtonDown(albaEventInteraction *e)
{
  if (GetEventRenderer(e) == NULL)
    return false;

  m_PickValue = true;
  return true;
}

//------------------------------------------------------------------------------
bool albaExtractIsosurfaceTool::OnDrag(albaEventInteraction *e)
{
  m_PickValue = false;
  return false;
}

//------------------------------------------------------------------------------
bool albaExtractIsosurfaceTool::OnLeftButtonUp(albaEventInteraction *e)
{
  vtkRenderer *renderer = GetEventRenderer(e);
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::SafeDownCast((albaDevice *)e->GetSender());
  if (renderer == NULL || mouse == NULL)
    return true;

  if (m_PickValue)
  {
    double mousePos[2];
    e->Get2DPosition(mousePos);
    if (mouse->GetRWI()->GetPicker()->Pick((int)mousePos[0], (int)mousePos[1], 0, renderer))
    {
      double pickedPos[3];
      mouse->GetRWI()->GetPicker()->GetPickPosition(pickedPos);
      SendPickedPointEvent(VME_PICKED, pickedPos);
    }
  }
  m_PickValue = false;

  renderer->GetRenderWindow()->SetDesiredUpdateRate(0.001);
  renderer->GetRenderWindow()->Render();
  return true;
}
