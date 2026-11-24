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
#include "albaDevice.h"
#include "albaInteractionBehavior.h"
#include "albaToolCamera.h"
#include "albaToolSelect.h"
#include "albaEventBase.h"
#include "albaEventInteraction.h"
#include "albaVME.h"

#include <algorithm>
#include <sstream>

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaPositionalEventRouter)

//------------------------------------------------------------------------------
albaPositionalEventRouter::albaPositionalEventRouter()
{
  m_EventTracing = false;
  m_Device = NULL;
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
bool albaPositionalEventRouter::IsInteracting(albaDevice *device)
{
  return device != NULL && device == m_Device;
}

//------------------------------------------------------------------------------
albaInteractionTool *albaPositionalEventRouter::GetInteractingTool(albaDevice *device)
{
  return IsInteracting(device) ? m_Handler : NULL;
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::OnEvent(albaEventBase *event)
{
  if (event->GetChannel() != MCH_INPUT)
  {
    Superclass::OnEvent(event); // the events of the other channels are forwarded
    return;
  }

  // one interaction at a time: the events of the other devices are ignored
  albaEventInteraction *e = albaEventInteraction::SafeDownCast(event);
  albaDevice *device = (albaDevice *)event->GetSender();
  if (e == NULL || (m_Device && device != m_Device))
    return;

  m_EventTrace.clear();

  // the mouse events carry the state of the modifier keys
  SetModifierKeys(e->GetModifier(ALBA_CTRL_KEY), e->GetModifier(ALBA_ALT_KEY), e->GetModifier(ALBA_SHIFT_KEY));

  switch (event->GetId())
  {
    case DEVICE_BUTTON_DOWN:
      switch (e->GetButton())
      {
        case ALBA_LEFT_BUTTON:   OnLeftButtonDown(e);   break;
        case ALBA_MIDDLE_BUTTON: OnMiddleButtonDown(e); break;
        case ALBA_RIGHT_BUTTON:  OnRightButtonDown(e);  break;
      }
      break;
    case DEVICE_BUTTON_UP:
      // a button up without interaction, e.g. pressed outside the views, is ignored
      if (m_Device == NULL)
        return;
      switch (e->GetButton())
      {
        case ALBA_LEFT_BUTTON:   OnLeftButtonUp(e);   break;
        case ALBA_MIDDLE_BUTTON: OnMiddleButtonUp(e); break;
        case ALBA_RIGHT_BUTTON:  OnRightButtonUp(e);  break;
      }
      break;
    case DEVICE_MOUSE_2D_MOVE: OnMouseMove(e);  break;
    case DEVICE_MOUSE_WHEEL:   OnMouseWheel(e); break;
    default: return;
  }

  WriteEventTrace(event);
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::OnLeftButtonDown(albaEventInteraction *e)
{
  albaDevice *device = (albaDevice *)e->GetSender();
  albaView *view = device->GetView();
  vtkRenderer *ren = device->GetRenderer();
  double pos[2];
  e->Get2DPosition(pos);

  if (m_Device == NULL)
  {
    // a new interaction: the behavior of the picked VME, or the tools of the stack from the top
    BeginInteraction(device, ALBA_LEFT_BUTTON);
    albaInteractionBehavior *behavior = PickBehavior(view, pos);

    // a tool can push or pop tools while handling an event: iterate on a copy
    albaInteractionTool *tool = NULL;
    std::vector<albaInteractionTool *> tools = m_Tools;
    for (int i = tools.size() - 1; i >= 0 && behavior == NULL && tool == NULL; i--)
      if (tools[i]->OnLeftButtonDown(view, ren, pos))
        tool = tools[i];

    if (tool)
      SetHandler(tool, false);
    else
    {
      // the default actions of the click: selection and context menu
      if (m_SelectTool->OnLeftButtonDown(view, ren, pos))
        TraceHandler("Select");
      if (behavior && behavior->OnLeftButtonDown(view, ren, pos))
        SetHandler(behavior, true);
    }
  }
  else if (m_Handler)
  {
    // the other buttons pressed during the interaction go to the same handler
    TraceCurrentHandler();
    m_Handler->OnLeftButtonDown(view, ren, pos);
  }

  // the camera gets the button too and moves with the drags the handler does not use,
  // unless the interaction already ended while the handler had the button down
  if (m_Device == NULL)
    return;
  if (!m_HandlerIsBehavior && m_CameraTool->OnLeftButtonDown(view, ren, pos))
    TraceHandler("Camera", m_CameraTool);
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::OnMiddleButtonDown(albaEventInteraction *e)
{
  albaDevice *device = (albaDevice *)e->GetSender();
  albaView *view = device->GetView();
  vtkRenderer *ren = device->GetRenderer();
  double pos[2];
  e->Get2DPosition(pos);

  if (m_Device == NULL)
  {
    // a new interaction: the behavior of the picked VME, or the tools of the stack from the top
    BeginInteraction(device, ALBA_MIDDLE_BUTTON);
    albaInteractionBehavior *behavior = PickBehavior(view, pos);

    // a tool can push or pop tools while handling an event: iterate on a copy
    albaInteractionTool *tool = NULL;
    std::vector<albaInteractionTool *> tools = m_Tools;
    for (int i = tools.size() - 1; i >= 0 && behavior == NULL && tool == NULL; i--)
      if (tools[i]->OnMiddleButtonDown(view, ren, pos))
        tool = tools[i];

    if (tool)
      SetHandler(tool, false);
    else
    {
      // the default actions of the click: selection and context menu
      if (m_SelectTool->OnMiddleButtonDown(view, ren, pos))
        TraceHandler("Select");
      if (behavior && behavior->OnMiddleButtonDown(view, ren, pos))
        SetHandler(behavior, true);
    }
  }
  else if (m_Handler)
  {
    // the other buttons pressed during the interaction go to the same handler
    TraceCurrentHandler();
    m_Handler->OnMiddleButtonDown(view, ren, pos);
  }

  // the camera gets the button too and moves with the drags the handler does not use,
  // unless the interaction already ended while the handler had the button down
  if (m_Device == NULL)
    return;
  if (!m_HandlerIsBehavior && m_CameraTool->OnMiddleButtonDown(view, ren, pos))
    TraceHandler("Camera", m_CameraTool);
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::OnRightButtonDown(albaEventInteraction *e)
{
  albaDevice *device = (albaDevice *)e->GetSender();
  albaView *view = device->GetView();
  vtkRenderer *ren = device->GetRenderer();
  double pos[2];
  e->Get2DPosition(pos);

  if (m_Device == NULL)
  {
    // a new interaction: the behavior of the picked VME, or the tools of the stack from the top
    BeginInteraction(device, ALBA_RIGHT_BUTTON);
    albaInteractionBehavior *behavior = PickBehavior(view, pos);

    // a tool can push or pop tools while handling an event: iterate on a copy
    albaInteractionTool *tool = NULL;
    std::vector<albaInteractionTool *> tools = m_Tools;
    for (int i = tools.size() - 1; i >= 0 && behavior == NULL && tool == NULL; i--)
      if (tools[i]->OnRightButtonDown(view, ren, pos))
        tool = tools[i];

    if (tool)
      SetHandler(tool, false);
    else
    {
      // the default actions of the click: selection and context menu
      if (m_SelectTool->OnRightButtonDown(view, ren, pos))
        TraceHandler("Select");
      if (behavior && behavior->OnRightButtonDown(view, ren, pos))
        SetHandler(behavior, true);
    }
  }
  else if (m_Handler)
  {
    // the other buttons pressed during the interaction go to the same handler
    TraceCurrentHandler();
    m_Handler->OnRightButtonDown(view, ren, pos);
  }

  // the camera gets the button too and moves with the drags the handler does not use,
  // unless the interaction already ended while the handler had the button down
  if (m_Device == NULL)
    return;
  if (!m_HandlerIsBehavior && m_CameraTool->OnRightButtonDown(view, ren, pos))
    TraceHandler("Camera", m_CameraTool);
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::OnLeftButtonUp(albaEventInteraction *e)
{
  albaDevice *device = (albaDevice *)e->GetSender();
  albaView *view = device->GetView();
  vtkRenderer *ren = device->GetRenderer();
  double pos[2];
  e->Get2DPosition(pos);

  if (m_Handler)
  {
    TraceCurrentHandler();
    m_Handler->OnLeftButtonUp(view, ren, pos);
  }
  if (m_CameraTool->OnLeftButtonUp(view, ren, pos))
    TraceHandler("Camera", m_CameraTool);

  EndInteraction(ALBA_LEFT_BUTTON);
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::OnMiddleButtonUp(albaEventInteraction *e)
{
  albaDevice *device = (albaDevice *)e->GetSender();
  albaView *view = device->GetView();
  vtkRenderer *ren = device->GetRenderer();
  double pos[2];
  e->Get2DPosition(pos);

  if (m_Handler)
  {
    TraceCurrentHandler();
    m_Handler->OnMiddleButtonUp(view, ren, pos);
  }
  if (m_CameraTool->OnMiddleButtonUp(view, ren, pos))
    TraceHandler("Camera", m_CameraTool);

  EndInteraction(ALBA_MIDDLE_BUTTON);
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::OnRightButtonUp(albaEventInteraction *e)
{
  albaDevice *device = (albaDevice *)e->GetSender();
  albaView *view = device->GetView();
  vtkRenderer *ren = device->GetRenderer();
  double pos[2];
  e->Get2DPosition(pos);

  if (m_Handler)
  {
    TraceCurrentHandler();
    m_Handler->OnRightButtonUp(view, ren, pos);
  }
  if (m_CameraTool->OnRightButtonUp(view, ren, pos))
    TraceHandler("Camera", m_CameraTool);

  EndInteraction(ALBA_RIGHT_BUTTON);

  // the right button released without dragging shows the context menu
  if (m_SelectTool->OnRightButtonUp(view, ren, pos))
    TraceHandler("Context menu");
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::OnMouseMove(albaEventInteraction *e)
{
  albaDevice *device = (albaDevice *)e->GetSender();
  albaView *view = device->GetView();
  vtkRenderer *ren = device->GetRenderer();
  double pos[2];
  e->Get2DPosition(pos);

  // the moves without buttons go to the tools, from the top of the stack
  if (m_Device == NULL)
  {
    // a tool can push or pop tools while handling an event: iterate on a copy
    std::vector<albaInteractionTool *> tools = m_Tools;
    for (int i = tools.size() - 1; i >= 0; i--)
      if (tools[i]->OnMove(view, ren, pos))
      {
        TraceHandler("Tool", tools[i]);
        return;
      }
    return;
  }

  // a drag cancels the context menu
  m_SelectTool->OnDrag(view, ren, pos);

  if (m_Handler)
  {
    TraceCurrentHandler();
    if (m_Handler->OnDrag(view, ren, pos))
      return;
  }

  // the drags the handler does not use move the camera
  if (m_CameraTool->OnDrag(view, ren, pos))
    TraceHandler("Camera", m_CameraTool);
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::OnMouseWheel(albaEventInteraction *e)
{
  albaDevice *device = (albaDevice *)e->GetSender();
  albaView *view = device->GetView();
  vtkRenderer *ren = device->GetRenderer();
  double pos[2];
  e->Get2DPosition(pos);
  double rotation = e->GetData() ? *(double *)e->GetData() : 0;

  // a tool can push or pop tools while handling an event: iterate on a copy
  std::vector<albaInteractionTool *> tools = m_Tools;
  for (int i = tools.size() - 1; i >= 0; i--)
    if (tools[i]->OnWheel(view, ren, pos, rotation))
    {
      TraceHandler("Tool", tools[i]);
      return;
    }

  // the wheel moves the camera when no button is pressed
  if (m_Device == NULL && m_CameraTool->OnWheel(view, ren, pos, rotation))
    TraceHandler("Camera", m_CameraTool);
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
void albaPositionalEventRouter::BeginInteraction(albaDevice *device, int button)
{
  m_Device = device;
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
  m_Device = NULL;
  m_Button = -1;
  m_Handler = NULL;
  m_HandlerIsBehavior = false;
}

//------------------------------------------------------------------------------
void albaPositionalEventRouter::SetHandler(albaInteractionTool *handler, bool isBehavior)
{
  // the interaction already ended while the handler had the button down, e.g. a modal dialog took the mouse capture
  if (m_Device == NULL)
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
void albaPositionalEventRouter::WriteEventTrace(albaEventBase *event)
{
  if (!m_EventTracing)
    return;

  // moves that nobody handled would flood the log
  albaID id = event->GetId();
  if (m_EventTrace.empty() && id == DEVICE_MOUSE_2D_MOVE)
    return;

  wxString eventName = albaIdString(id);
  albaDevice *device = (albaDevice *)event->GetSender();
  albaEventInteraction *e = albaEventInteraction::SafeDownCast(event);

  std::ostringstream trace;
  trace << GetTypeName() << ": " << (const char *)eventName.ToAscii() << " (" << id << ")";
  if (device && device->GetName())
    trace << " device=" << device->GetName();
  if (e)
    trace << " button=" << e->GetButton() << " modifiers=" << e->GetModifiers();
  trace << " -> " << (m_EventTrace.empty() ? "None" : m_EventTrace);

  m_LastEventTrace = trace.str();
  albaLogMessage("%s", m_LastEventTrace.c_str());
}
