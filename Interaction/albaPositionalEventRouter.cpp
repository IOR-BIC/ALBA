/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPositionalEventRouter
 Authors: Marco Petrone

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


#include "albaPositionalEventRouter.h"

#include "albaView.h"
#include "albaRWIBase.h"
#include "albaInteractionBehavior.h"
#include "albaToolCamera.h"
#include "albaToolSelect.h"
#include "albaEventBase.h"
#include "albaVME.h"

#include <algorithm>
#include <sstream>

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaPositionalEventRouter)

//------------------------------------------------------------------------------
albaPositionalEventRouter::albaPositionalEventRouter()
{
  m_EventTracing = false;
  m_Interacting = false;
  m_RWI = NULL;
  m_Button = -1;
  m_Handler = NULL;
  m_HandlerIsBehavior = false;

  m_CameraTool = albaToolCamera::New();
  m_SelectTool = albaToolSelect::New();
  m_SelectTool->SetListener(this);
}

//------------------------------------------------------------------------------
albaPositionalEventRouter::~albaPositionalEventRouter()
{
  while (!m_Tools.empty())
    PopTool(m_Tools.back());
  if (m_Handler)
    m_Handler->UnRegister(this);

  m_CameraTool->Delete();
  m_SelectTool->Delete();
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::EnableSelect(bool enable)
{
  m_SelectTool->EnableSelect(enable);
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::PushTool(albaInteractionTool *tool)
{
  if (tool == NULL || std::find(m_Tools.begin(), m_Tools.end(), tool) != m_Tools.end())
    return;

  tool->Register(this);
  m_Tools.push_back(tool);
  tool->SetModifierKeys(m_CtrlDown, m_AltDown, m_ShiftDown);
  tool->OnPush();
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::PopTool(albaInteractionTool *tool)
{
  std::vector<albaInteractionTool *>::iterator iter = std::find(m_Tools.begin(), m_Tools.end(), tool);
  if (iter == m_Tools.end())
    return;

  m_Tools.erase(iter);

  // the interaction of the tool goes on with the camera only, until the button is released
  if (m_Handler == tool)
  {
    m_Handler->UnRegister(this);
    m_Handler = NULL;
  }

  tool->OnPop();
  tool->UnRegister(this);
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::SetModifierKeys(bool ctrlDown, bool altDown, bool shiftDown)
{
  Superclass::SetModifierKeys(ctrlDown, altDown, shiftDown);
  m_CameraTool->SetModifierKeys(ctrlDown, altDown, shiftDown);
  m_SelectTool->SetModifierKeys(ctrlDown, altDown, shiftDown);
  for (int i = 0; i < m_Tools.size(); i++)
    m_Tools[i]->SetModifierKeys(ctrlDown, altDown, shiftDown);

  // the behaviors are not on the stack
  if (m_HandlerIsBehavior && m_Handler)
    m_Handler->SetModifierKeys(ctrlDown, altDown, shiftDown);
}

//------------------------------------------------------------------------------
bool albaPositionalEventRouter::OnKey(int keyCode, bool pressed)
{
  // all the tools see the modifier keys, also when a tool above uses the key
  if (keyCode == WXK_CONTROL)
    SetModifierKeys(pressed, m_AltDown, m_ShiftDown);
  else if (keyCode == WXK_ALT)
    SetModifierKeys(m_CtrlDown, pressed, m_ShiftDown);
  else if (keyCode == WXK_SHIFT)
    SetModifierKeys(m_CtrlDown, m_AltDown, pressed);

  // a tool can push or pop tools while handling an event: iterate on a copy
  std::vector<albaInteractionTool *> tools = m_Tools;
  for (int i = tools.size() - 1; i >= 0; i--)
    if (tools[i]->OnKey(keyCode, pressed))
      return true;
  return false;
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::OnLeftButtonDown(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  // the view of the render window picks the behaviors and selects: the dialogs have no view
  albaView *view = rwi->GetView();

  // one interaction at a time: the events of the other render windows are ignored
  if (m_Interacting && rwi != m_RWI)
    return;
  m_EventTrace.clear();

  if (!m_Interacting)
  {
    // a new interaction: the behavior of the picked VME, or the tools of the stack from the top
    BeginInteraction(rwi, ALBA_LEFT_BUTTON);
    albaInteractionBehavior *behavior = PickBehavior(view, pos);

    // a tool can push or pop tools while handling an event: iterate on a copy
    albaInteractionTool *tool = NULL;
    std::vector<albaInteractionTool *> tools = m_Tools;
    for (int i = tools.size() - 1; i >= 0 && behavior == NULL && tool == NULL; i--)
      if (tools[i]->OnLeftButtonDown(rwi, ren, pos))
        tool = tools[i];

    if (tool)
      SetHandler(tool, false);
    else
    {
      // the default actions of the click on a view: selection and context menu
      if (view && m_SelectTool->OnLeftButtonDown(rwi, ren, pos))
        TraceHandler("Select");
      if (behavior && behavior->OnLeftButtonDown(rwi, ren, pos))
        SetHandler(behavior, true);
    }
  }
  else if (m_Handler)
  {
    // the other buttons pressed during the interaction go to the same handler
    TraceCurrentHandler();
    m_Handler->OnLeftButtonDown(rwi, ren, pos);
  }

  // the camera gets the button too and moves with the drags the handler does not use, if the render window
  // moves its camera and the interaction did not already end while the handler had the button down
  if (m_Interacting && !m_HandlerIsBehavior && rwi->IsCameraInteractionEnabled() && m_CameraTool->OnLeftButtonDown(rwi, ren, pos))
    TraceHandler("Camera", m_CameraTool);

  WriteEventTrace("LeftButtonDown");
}
//------------------------------------------------------------------------------
void albaPositionalEventRouter::OnMiddleButtonDown(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  // the view of the render window picks the behaviors and selects: the dialogs have no view
  albaView *view = rwi->GetView();

  // one interaction at a time: the events of the other render windows are ignored
  if (m_Interacting && rwi != m_RWI)
    return;
  m_EventTrace.clear();

  if (!m_Interacting)
  {
    // a new interaction: the behavior of the picked VME, or the tools of the stack from the top
    BeginInteraction(rwi, ALBA_MIDDLE_BUTTON);
    albaInteractionBehavior *behavior = PickBehavior(view, pos);

    // a tool can push or pop tools while handling an event: iterate on a copy
    albaInteractionTool *tool = NULL;
    std::vector<albaInteractionTool *> tools = m_Tools;
    for (int i = tools.size() - 1; i >= 0 && behavior == NULL && tool == NULL; i--)
      if (tools[i]->OnMiddleButtonDown(rwi, ren, pos))
        tool = tools[i];

    if (tool)
      SetHandler(tool, false);
    else
    {
      // the default actions of the click on a view: selection and context menu
      if (view && m_SelectTool->OnMiddleButtonDown(rwi, ren, pos))
        TraceHandler("Select");
      if (behavior && behavior->OnMiddleButtonDown(rwi, ren, pos))
        SetHandler(behavior, true);
    }
  }
  else if (m_Handler)
  {
    // the other buttons pressed during the interaction go to the same handler
    TraceCurrentHandler();
    m_Handler->OnMiddleButtonDown(rwi, ren, pos);
  }

  // the camera gets the button too and moves with the drags the handler does not use, if the render window
  // moves its camera and the interaction did not already end while the handler had the button down
  if (m_Interacting && !m_HandlerIsBehavior && rwi->IsCameraInteractionEnabled() && m_CameraTool->OnMiddleButtonDown(rwi, ren, pos))
    TraceHandler("Camera", m_CameraTool);

  WriteEventTrace("MiddleButtonDown");
}
//------------------------------------------------------------------------------
void albaPositionalEventRouter::OnRightButtonDown(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  // the view of the render window picks the behaviors and selects: the dialogs have no view
  albaView *view = rwi->GetView();

  // one interaction at a time: the events of the other render windows are ignored
  if (m_Interacting && rwi != m_RWI)
    return;
  m_EventTrace.clear();

  if (!m_Interacting)
  {
    // a new interaction: the behavior of the picked VME, or the tools of the stack from the top
    BeginInteraction(rwi, ALBA_RIGHT_BUTTON);
    albaInteractionBehavior *behavior = PickBehavior(view, pos);

    // a tool can push or pop tools while handling an event: iterate on a copy
    albaInteractionTool *tool = NULL;
    std::vector<albaInteractionTool *> tools = m_Tools;
    for (int i = tools.size() - 1; i >= 0 && behavior == NULL && tool == NULL; i--)
      if (tools[i]->OnRightButtonDown(rwi, ren, pos))
        tool = tools[i];

    if (tool)
      SetHandler(tool, false);
    else
    {
      // the default actions of the click on a view: selection and context menu
      if (view && m_SelectTool->OnRightButtonDown(rwi, ren, pos))
        TraceHandler("Select");
      if (behavior && behavior->OnRightButtonDown(rwi, ren, pos))
        SetHandler(behavior, true);
    }
  }
  else if (m_Handler)
  {
    // the other buttons pressed during the interaction go to the same handler
    TraceCurrentHandler();
    m_Handler->OnRightButtonDown(rwi, ren, pos);
  }

  // the camera gets the button too and moves with the drags the handler does not use, if the render window
  // moves its camera and the interaction did not already end while the handler had the button down
  if (m_Interacting && !m_HandlerIsBehavior && rwi->IsCameraInteractionEnabled() && m_CameraTool->OnRightButtonDown(rwi, ren, pos))
    TraceHandler("Camera", m_CameraTool);

  WriteEventTrace("RightButtonDown");
}
//------------------------------------------------------------------------------
void albaPositionalEventRouter::OnLeftButtonUp(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  // a button up without interaction, e.g. pressed outside the views, and the events of the other render windows are ignored
  if (!m_Interacting || rwi != m_RWI)
    return;
  m_EventTrace.clear();

  if (m_Handler)
  {
    TraceCurrentHandler();
    m_Handler->OnLeftButtonUp(rwi, ren, pos);
  }
  if (m_CameraTool->OnLeftButtonUp(rwi, ren, pos))
    TraceHandler("Camera", m_CameraTool);

  EndInteraction(ALBA_LEFT_BUTTON);

  WriteEventTrace("LeftButtonUp");
}
//------------------------------------------------------------------------------
void albaPositionalEventRouter::OnMiddleButtonUp(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  // a button up without interaction, e.g. pressed outside the views, and the events of the other render windows are ignored
  if (!m_Interacting || rwi != m_RWI)
    return;
  m_EventTrace.clear();

  if (m_Handler)
  {
    TraceCurrentHandler();
    m_Handler->OnMiddleButtonUp(rwi, ren, pos);
  }
  if (m_CameraTool->OnMiddleButtonUp(rwi, ren, pos))
    TraceHandler("Camera", m_CameraTool);

  EndInteraction(ALBA_MIDDLE_BUTTON);

  WriteEventTrace("MiddleButtonUp");
}
//------------------------------------------------------------------------------
void albaPositionalEventRouter::OnRightButtonUp(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  // the view of the render window shows the context menu: the dialogs have no view
  albaView *view = rwi->GetView();

  // a button up without interaction, e.g. pressed outside the views, and the events of the other render windows are ignored
  if (!m_Interacting || rwi != m_RWI)
    return;
  m_EventTrace.clear();

  if (m_Handler)
  {
    TraceCurrentHandler();
    m_Handler->OnRightButtonUp(rwi, ren, pos);
  }
  if (m_CameraTool->OnRightButtonUp(rwi, ren, pos))
    TraceHandler("Camera", m_CameraTool);

  EndInteraction(ALBA_RIGHT_BUTTON);

  // the right button released on a view without dragging shows the context menu
  if (view && m_SelectTool->OnRightButtonUp(rwi, ren, pos))
    TraceHandler("Context menu");
  WriteEventTrace("RightButtonUp");
}
//------------------------------------------------------------------------------
void albaPositionalEventRouter::OnMouseMove(albaRWIBase *rwi, vtkRenderer *ren, double pos[2])
{
  // one interaction at a time: the events of the other render windows are ignored
  if (m_Interacting && rwi != m_RWI)
    return;
  m_EventTrace.clear();

  if (!m_Interacting)
  {
    // the moves without buttons go to the tools, from the top of the stack, until one uses them
    // (a tool can push or pop tools while handling an event: iterate on a copy)
    bool used = false;
    std::vector<albaInteractionTool *> tools = m_Tools;
    for (int i = tools.size() - 1; i >= 0 && !used; i--)
      if (tools[i]->OnMove(rwi, ren, pos))
      {
        TraceHandler("Tool", tools[i]);
        used = true;
      }
  }
  else
  {
    // a drag cancels the context menu
    m_SelectTool->OnDrag(rwi, ren, pos);

    bool used = false;
    if (m_Handler)
    {
      TraceCurrentHandler();
      used = m_Handler->OnDrag(rwi, ren, pos);
    }

    // the drags the handler does not use move the camera
    if (!used && m_CameraTool->OnDrag(rwi, ren, pos))
      TraceHandler("Camera", m_CameraTool);
  }

  WriteEventTrace("MouseMove");
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::OnMouseWheel(albaRWIBase *rwi, vtkRenderer *ren, double pos[2], double rotation)
{
  // one interaction at a time: the events of the other render windows are ignored
  if (m_Interacting && rwi != m_RWI)
    return;
  m_EventTrace.clear();

  // the wheel goes to the tools, from the top of the stack, until one uses it
  // (a tool can push or pop tools while handling an event: iterate on a copy)
  bool used = false;
  std::vector<albaInteractionTool *> tools = m_Tools;
  for (int i = tools.size() - 1; i >= 0 && !used; i--)
    if (tools[i]->OnWheel(rwi, ren, pos, rotation))
    {
      TraceHandler("Tool", tools[i]);
      used = true;
    }

  // the wheel moves the camera when no tool uses it, no button is pressed and the render window moves its camera
  if (!used && !m_Interacting && rwi->IsCameraInteractionEnabled() && m_CameraTool->OnWheel(rwi, ren, pos, rotation))
    TraceHandler("Camera", m_CameraTool);

  WriteEventTrace("MouseWheel");
}
//------------------------------------------------------------------------------
albaInteractionBehavior *albaPositionalEventRouter::PickBehavior(albaView *view, double pos[2])
{
  albaVME *pickedVme = NULL;
  albaInteractionBehavior *behavior = NULL;
  double pickedPos[3] = {0, 0, 0};

  // the view picks at the position of the button down
  if (view && view->Pick((int)pos[0], (int)pos[1]))
  {
    pickedVme = view->GetPickedVme();
    view->GetPickedPosition(pickedPos);
    behavior = pickedVme ? pickedVme->GetBehavior() : NULL;
  }

  // the VME of the behavior is set by its owner: a gizmo behavior can transform another VME
  m_SelectTool->SetPickedVME(pickedVme, pickedPos);
  if (behavior)
  {
    behavior->SetPickedPosition(pickedPos);
    behavior->SetModifierKeys(m_CtrlDown, m_AltDown, m_ShiftDown);
  }
  return behavior;
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::BeginInteraction(albaRWIBase *rwi, int button)
{
  m_Interacting = true;
  m_RWI = rwi;
  m_Button = button;
  m_Handler = NULL;
  m_HandlerIsBehavior = false;
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::EndInteraction(int button)
{
  if (button != m_Button)
    return;

  if (m_Handler)
    m_Handler->UnRegister(this);
  m_Interacting = false;
  m_RWI = NULL;
  m_Button = -1;
  m_Handler = NULL;
  m_HandlerIsBehavior = false;
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::SetHandler(albaInteractionTool *handler, bool isBehavior)
{
  // the interaction already ended while the handler had the button down, e.g. a modal dialog took the mouse capture
  if (!m_Interacting)
    return;

  TraceHandler(isBehavior ? "Behavior" : "Tool", handler);

  // a tool that popped itself while handling the button does not interact
  if (isBehavior || std::find(m_Tools.begin(), m_Tools.end(), handler) != m_Tools.end())
  {
    // the handler is kept alive until the end of the interaction: an operation or a gizmo can delete it meanwhile
    handler->Register(this);
    m_Handler = handler;
    m_HandlerIsBehavior = isBehavior;
  }
}

//----------------------------------------------------------------------------
void albaPositionalEventRouter::TraceHandler(const char *handler, albaObject *obj)
{
  if (!m_EventTracing)
    return;

  if (!m_EventTrace.empty())
    m_EventTrace += ", ";
  m_EventTrace += handler;
  if (obj)
  {
    m_EventTrace += " ";
    m_EventTrace += obj->GetTypeName();
  }
}

//----------------------------------------------------------------------------
void albaPositionalEventRouter::TraceCurrentHandler()
{
  TraceHandler(m_HandlerIsBehavior ? "Behavior" : "Tool", m_Handler);
}

//----------------------------------------------------------------------------
void albaPositionalEventRouter::WriteEventTrace(const char *eventName)
{
  if (!m_EventTracing)
    return;

  // moves that nobody handled would flood the log
  if (m_EventTrace.empty() && std::string(eventName) == "MouseMove")
    return;

  std::ostringstream trace;
  trace << GetTypeName() << ": " << eventName << " ctrl=" << m_CtrlDown << " alt=" << m_AltDown << " shift=" << m_ShiftDown;
  trace << " -> " << (m_EventTrace.empty() ? "None" : m_EventTrace);

  m_LastEventTrace = trace.str();
  albaLogMessage("%s", m_LastEventTrace.c_str());
}