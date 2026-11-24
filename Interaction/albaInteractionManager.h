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
class albaToolFlyTo;
class albaPositionalEventRouter;

/** Owner of the interaction inside the views: it creates the PER (positional event router) and registers it in
  albaServiceLocator. The render windows send it their mouse events, and it routes them to the tools and to the
  behavior of the picked VME; the tools are pushed directly on the PER. The events sent by the PER go to the listener.
  @sa albaPositionalEventRouter albaRWIBase */
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

  /** Send a key pressed or released to the tools of the PER; return true if a tool used it */
  bool OnKey(int keyCode, bool pressed);

  /** Enable/disable the event tracing: each input event routed by the PER is written in the log
  with the modifiers and who handled it. Disabled by default. */
  void SetEventTracing(bool enable);

  /** Start the fly-to mode: push the fly-to tool, the next click flies to the clicked point */
  void CameraFlyToMode();

protected:
  albaPositionalEventRouter *m_PositionalEventRouter; ///< router of the mouse events
  albaToolFlyTo             *m_FlyToTool;             ///< tool of the fly-to mode
};
#endif 