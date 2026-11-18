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
#include "albaDeviceButtonsPadTracker.h"

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
  if (albaDeviceButtonsPadTracker::SafeDownCast(device))
    return m_TrackerTool->IsInteracting(device) ? m_TrackerTool : NULL;
  return m_MouseTool->IsInteracting() ? m_MouseTool : NULL;
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnLeftButtonDown(albaView *view, vtkRenderer *ren, double pos[2])
{
  return m_MouseTool->OnLeftButtonDown(view, ren, pos);
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnLeftButtonUp(albaView *view, vtkRenderer *ren, double pos[2])
{
  return m_MouseTool->OnLeftButtonUp(view, ren, pos);
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnMiddleButtonDown(albaView *view, vtkRenderer *ren, double pos[2])
{
  return m_MouseTool->OnMiddleButtonDown(view, ren, pos);
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnMiddleButtonUp(albaView *view, vtkRenderer *ren, double pos[2])
{
  return m_MouseTool->OnMiddleButtonUp(view, ren, pos);
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnRightButtonDown(albaView *view, vtkRenderer *ren, double pos[2])
{
  return m_MouseTool->OnRightButtonDown(view, ren, pos);
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnRightButtonUp(albaView *view, vtkRenderer *ren, double pos[2])
{
  return m_MouseTool->OnRightButtonUp(view, ren, pos);
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnDrag(albaView *view, vtkRenderer *ren, double pos[2])
{
  return m_MouseTool->OnDrag(view, ren, pos);
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnWheel(albaView *view, vtkRenderer *ren, double pos[2], double rotation)
{
  return m_MouseTool->OnWheel(view, ren, pos, rotation);
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnTrackerButtonDown(albaDeviceButtonsPadTracker *tracker, int button, albaMatrix *pose)
{
  return m_TrackerTool->OnTrackerButtonDown(tracker, button, pose);
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnTrackerButtonUp(albaDeviceButtonsPadTracker *tracker, int button)
{
  return m_TrackerTool->OnTrackerButtonUp(tracker, button);
}

//------------------------------------------------------------------------------
bool albaCameraTool::OnTrackerDrag(albaDeviceButtonsPadTracker *tracker, albaMatrix *pose)
{
  return m_TrackerTool->OnTrackerDrag(tracker, pose);
}

//------------------------------------------------------------------------------
void albaCameraTool::SetModifierKeys(bool ctrlDown, bool altDown, bool shiftDown)
{
  Superclass::SetModifierKeys(ctrlDown, altDown, shiftDown);
  m_MouseTool->SetModifierKeys(ctrlDown, altDown, shiftDown);
  m_TrackerTool->SetModifierKeys(ctrlDown, altDown, shiftDown);
}
