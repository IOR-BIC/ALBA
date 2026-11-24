/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPositionalEventRouter
 Authors: Marco Petrone, originally by Silvano Imboden

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaPositionalEventRouter_h
#define __albaPositionalEventRouter_h

#include "albaInteractionAgent.h"
#include <string>
#include <vector>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaDevice;
class albaEventInteraction;
class albaInteractionBehavior;
class albaInteractionTool;
class albaToolCamera;
class albaToolSelect;

/** Router of the input events of the mouse devices.
  One device at a time interacts: while a device has a button pressed, the events of the other devices are ignored.
  A button down picks the scene and chooses the handler of the interaction, which receives
  the buttons and the drags of the device until the button that started the interaction is released:
  - the behavior of the picked VME (e.g. a gizmo), if any and if it uses the button;
  - otherwise the tools of the stack, from the top: the first one using the button.
  When no tool uses the button the select tool gets the click (selection and context menu).
  The camera tool gets the buttons too, except when a behavior uses them, and moves with the
  drags the handler does not use. The moves without buttons and the wheel go to the tools
  of the stack, and the wheel moves the camera if no tool uses it and no button is pressed. */
class ALBA_EXPORT albaPositionalEventRouter : public albaInteractionAgent
{
public:
  albaTypeMacro(albaPositionalEventRouter,albaInteractionAgent);

  /** Enable/disable the selection of the picked VME with a click (see albaToolSelect) */
  virtual void EnableSelect(bool enable);

  /** Return true if a button of the device is pressed, from the button down to the release of the button that started the interaction */
  bool IsInteracting(albaDevice *device);

  /** Return the tool or the behavior interacting with the device, NULL if none (e.g. only the camera moves) */
  albaInteractionTool *GetInteractingTool(albaDevice *device);

  virtual void OnEvent(albaEventBase *event);

  /** Return the tool moving the camera, always at the bottom of the tool stack */
  albaToolCamera *GetCameraTool() {return m_CameraTool;}

  /** Return the tool of the default actions of the clicks: selection and context menu */
  albaToolSelect *GetSelectTool() {return m_SelectTool;}

  /** Push a tool on top of the tool stack: it receives the events before the tools
  below it and before the default actions. The PER keeps a reference to the tool. */
  void PushTool(albaInteractionTool *tool);

  /** Remove a tool from the tool stack, wherever it is */
  void PopTool(albaInteractionTool *tool);

  /** Offer a key pressed or released to the tools, from the top of the stack; return true if a tool used it.
    CTRL, ALT and SHIFT update the state of the modifier keys of all the tools first. */
  bool OnKey(int keyCode, bool pressed);

  /** Set the state of the modifier keys, also on the camera and select tools, on the tools of the stack and on the interacting behaviors */
  virtual void SetModifierKeys(bool ctrlDown, bool altDown, bool shiftDown);

  /** Return the number of tools pushed on the stack (the camera tool is not counted) */
  int GetNumberOfTools() {return m_Tools.size();}

  /** Enable/disable the event tracing: each input event is written in the log
  with the device, the button, the modifiers and who handled it. */
  void SetEventTracing(bool enable) {m_EventTracing = enable;}
  bool GetEventTracing() {return m_EventTracing;}

  /** Return the last trace written in the log */
  const char *GetLastEventTrace() {return m_LastEventTrace.c_str();}

protected:
  albaPositionalEventRouter();
  virtual ~albaPositionalEventRouter();

  /** The button down of the mouse: a new interaction starts, or the button goes to the handler of the current one, and to the camera */
  void OnLeftButtonDown(albaEventInteraction *e);
  void OnMiddleButtonDown(albaEventInteraction *e);
  void OnRightButtonDown(albaEventInteraction *e);

  /** The button up of the mouse goes to the handler and to the camera; the release of the starting button ends the interaction */
  void OnLeftButtonUp(albaEventInteraction *e);
  void OnMiddleButtonUp(albaEventInteraction *e);
  void OnRightButtonUp(albaEventInteraction *e);

  /** The drags go to the handler and, if it does not use them, to the camera; the moves without buttons go to the tools */
  void OnMouseMove(albaEventInteraction *e);

  /** The wheel goes to the tools and, if no tool uses it and no button is pressed, to the camera */
  void OnMouseWheel(albaEventInteraction *e);

  /** Pick the scene at the button down: the select tool and the behavior of the picked VME get the picking; return the behavior, NULL if none */
  albaInteractionBehavior *PickBehavior(albaEventInteraction *e);

  /** Start the interaction of the device with the button, still without handler */
  void BeginInteraction(albaDevice *device, int button);

  /** End the interaction if the button is the one that started it */
  void EndInteraction(int button);

  /** Set the handler of the interaction; a tool that popped itself while handling the button is not set */
  void SetHandler(albaInteractionTool *handler, bool isBehavior);

  /** Add the handler of the interaction to the trace of the current event */
  void TraceCurrentHandler();

  /** Add who handled the current event to its trace (only when the event tracing is on) */
  void TraceHandler(const char *handler, albaObject *obj = NULL);

  /** Write the trace of the current event in the log (only when the event tracing is on) */
  void WriteEventTrace(albaEventBase *event);

  bool            m_EventTracing;   ///< true if the events are traced in the log
  std::string     m_EventTrace;     ///< who handled the current event
  std::string     m_LastEventTrace; ///< last trace written in the log

  albaToolCamera *m_CameraTool; ///< tool moving the camera, at the bottom of the stack
  albaToolSelect *m_SelectTool; ///< default actions of the clicks: selection and context menu
  std::vector<albaInteractionTool *> m_Tools; ///< the tool stack, the top is the last one

  albaDevice          *m_Device;             ///< the device interacting, NULL if none
  int                  m_Button;             ///< the button that started the interaction
  albaInteractionTool *m_Handler;            ///< the tool or the behavior handling the interaction, NULL if none
  bool                 m_HandlerIsBehavior;  ///< a behavior keeps the buttons for itself: the camera does not get them

private:
  albaPositionalEventRouter(const albaPositionalEventRouter&);  // Not implemented.
  void operator=(const albaPositionalEventRouter&);  // Not implemented.

  /** test friend */
  friend class albaPositionalEventRouterTest;
};

#endif
