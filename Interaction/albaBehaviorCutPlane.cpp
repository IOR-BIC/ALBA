/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaBehaviorCutPlane
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

#include "albaBehaviorCutPlane.h"
#include "albaDecl.h"
#include "albaEvent.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaBehaviorCutPlane);

//------------------------------------------------------------------------------
albaBehaviorCutPlane::albaBehaviorCutPlane()
{
  m_RollEnabled = false;
  m_ModifierClicks = false;
  m_TransformEnabled = true;
}

//------------------------------------------------------------------------------
albaBehaviorCutPlane::~albaBehaviorCutPlane()
{
}

//------------------------------------------------------------------------------
bool albaBehaviorCutPlane::OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2])
{
  if (m_ModifierClicks && (m_ShiftDown || m_CtrlDown))
  {
    albaEventMacro(albaEvent(this, ID_CLICK_ON_VME, (long)(m_ShiftDown ? ALBA_SHIFT_KEY : ALBA_CTRL_KEY)));
    return true;
  }

  return Superclass::OnLeftButtonDown(view, ren, pos);
}

//------------------------------------------------------------------------------
bool albaBehaviorCutPlane::PrepareTransform(int button)
{
  return m_TransformEnabled && Superclass::PrepareTransform(button);
}
