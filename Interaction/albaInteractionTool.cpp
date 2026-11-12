/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionTool
 Authors: Gianluigi Crimi

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

#include "albaInteractionTool.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaEventInteraction.h"

#include "vtkRenderer.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaInteractionTool);

//------------------------------------------------------------------------------
vtkRenderer *albaInteractionTool::GetEventRenderer(albaEventInteraction *e)
{
  albaDeviceButtonsPadMouse *mouse = albaDeviceButtonsPadMouse::SafeDownCast((albaDevice *)e->GetSender());
  vtkRenderer *renderer = mouse ? mouse->GetRenderer() : NULL;
  if (renderer == NULL || m_Renderer == NULL || renderer->GetRenderWindow() != m_Renderer->GetRenderWindow())
    return NULL;
  return renderer;
}
