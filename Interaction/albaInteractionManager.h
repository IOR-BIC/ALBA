/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractionManager
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaInteractionManager_h
#define __albaInteractionManager_h

#include "albaObject.h"
#include "albaObserver.h"
#include "albaEventSender.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaEventBase;
class albaDeviceButtonsPadMouse;
class albaView;
class albaToolFlyTo;
class albaPositionalEventRouter;
class albaInteractionTool;

/** Owner of the interaction inside the views: it creates the mouse device and the PER
  (positional event router), which observes the mouse and routes its events to the tools
  and to the behavior of the picked VME. The events sent by the PER go to the listener.
  @sa albaPositionalEventRouter albaDeviceButtonsPadMouse */
class ALBA_EXPORT albaInteractionManager : public albaObject, public albaObserver, public albaEventSender
{
public:
  albaInteractionManager();
  virtual ~albaInteractionManager();

  albaTypeMacro(albaInteractionManager,albaObject);

  /** Enable/Disable VME selection by picking */
  void EnableSelect(bool enable);

  /** The events of the PER are forwarded to the listener */
  virtual void OnEvent(albaEventBase *event);

  /** return the mouse device */
  albaDeviceButtonsPadMouse *GetMouseDevice() {return m_Mouse;}

  /** Set the selected view: the mouse works on it. */
  void ViewSelected(albaView *view);
  
  /** Return the currently selected view */
  albaView *GetSelectedView() {return m_SelectedView;}

  /** return the positional event router */
  albaPositionalEventRouter *GetPER() {return m_PositionalEventRouter;}

  /** Push a tool on the tool stack of the PER: it receives the events before the default interaction */
  void PushTool(albaInteractionTool *tool);

  /** Remove a tool from the tool stack of the PER */
  void PopTool(albaInteractionTool *tool);

  /** Send a key pressed or released to the tools of the PER; return true if a tool used it */
  bool OnKey(int keyCode, bool pressed);

  /** Enable/disable the event tracing: each input event routed by the PER is written in the log
  with the device, the button, the modifiers and who handled it. Disabled by default. */
  void SetEventTracing(bool enable);

  /** Start the fly-to mode: push the fly-to tool, the next click flies to the clicked point */
  void CameraFlyToMode();

protected:
  albaDeviceButtonsPadMouse *m_Mouse;                 ///< the mouse device, observed by the PER
  albaPositionalEventRouter *m_PositionalEventRouter; ///< router of the mouse events
  albaToolFlyTo             *m_FlyToTool;             ///< tool of the fly-to mode
  albaView                  *m_SelectedView;          ///< the view currently selected
};
#endif 