/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCameraTool
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

#include "albaCameraTool.h"
#include "albaCameraMouseTool.h"
#include "albaCameraTrackerTool.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaCameraTool);

//------------------------------------------------------------------------------
albaCameraTool::albaCameraTool()
{
  albaNEW(m_MouseTool);
  albaNEW(m_TrackerTool);
}

//------------------------------------------------------------------------------
albaCameraTool::~albaCameraTool()
{
  albaDEL(m_MouseTool);
  albaDEL(m_TrackerTool);
}

//------------------------------------------------------------------------------
albaInteractionTool *albaCameraTool::GetInteractingTool(albaDevice *device)
{
  if (m_MouseTool->IsInteracting(device))
    return m_MouseTool;
  if (m_TrackerTool->IsInteracting(device))
    return m_TrackerTool;
  return NULL;
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnLeftButtonDown(albaEventInteraction *e)
{
  return m_MouseTool->OnLeftButtonDown(e) || m_TrackerTool->OnLeftButtonDown(e);
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnLeftButtonUp(albaEventInteraction *e)
{
  return m_MouseTool->OnLeftButtonUp(e) || m_TrackerTool->OnLeftButtonUp(e);
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnMiddleButtonDown(albaEventInteraction *e)
{
  return m_MouseTool->OnMiddleButtonDown(e) || m_TrackerTool->OnMiddleButtonDown(e);
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnMiddleButtonUp(albaEventInteraction *e)
{
  return m_MouseTool->OnMiddleButtonUp(e) || m_TrackerTool->OnMiddleButtonUp(e);
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnRightButtonDown(albaEventInteraction *e)
{
  return m_MouseTool->OnRightButtonDown(e) || m_TrackerTool->OnRightButtonDown(e);
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnRightButtonUp(albaEventInteraction *e)
{
  return m_MouseTool->OnRightButtonUp(e) || m_TrackerTool->OnRightButtonUp(e);
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnDrag(albaEventInteraction *e)
{
  return m_MouseTool->OnDrag(e) || m_TrackerTool->OnDrag(e);
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnWheel(albaEventInteraction *e)
{
  return m_MouseTool->OnWheel(e) || m_TrackerTool->OnWheel(e);
}
