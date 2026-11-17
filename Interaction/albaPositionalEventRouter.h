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
#include <map>
#include <string>
#include <vector>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaVME;
class albaDevice;
class albaEventInteraction;
class albaInteractionBehavior;
class myPIMPL;
class vtkCamera;
class albaInteractionTool;
class albaCameraTool;

/** Class implementing routing of events based on picking.
  This class routes events from input devices depending on picking information.
  After first picking events are routed to interactor assigned to picked node
  until interaction is finished. To be completed... 
  This is a refactoring of albaISV class by Silvano Imboden
  @todo
  - complete documentation
  - create interactor for CameraBehavior
  - manage sending events to logic (VME_SELECT, CAMERA_FIT  the 
*/
class ALBA_EXPORT albaPositionalEventRouter : public albaInteractionAgent
{
public:
  albaTypeMacro(albaPositionalEventRouter,albaInteractionAgent);

  virtual void EnableSelect(bool enable) {m_CanSelect = enable;};

  /** Enable fly to mode: next click will select where to fly to */
  virtual void FlyToMode() {m_FlyToFlag = true;};

  /** 
   return true if device is in the set of devices currently interacting
   the optional parameter can be used in SINGLE_BUTTON mode to know if 
   interaction has been started by a specific button. */
  bool IsInteracting(albaDevice *device, int button=-1);

  /** Start the default interaction of a device with a button */
  virtual int StartInteraction(albaDevice *device, int button);

  /** Stop the default interaction of a device with a button */
  virtual int StopInteraction(albaDevice *device, int button);

  virtual void OnEvent(albaEventBase *event);

  void OnVmeSelected(albaVME *vme){ m_VME = vme; }

  enum modalities {SINGLE_BUTTON=0,MULTI_BUTTON};

  void SetMode(int mode) {m_Mode=mode;}
  int GetMode() {return m_Mode;}
  void SetModeToSingleButton() {m_Mode=SINGLE_BUTTON;}
  void SetModeToMultiButton() {m_Mode=MULTI_BUTTON;}

  /** Return the tool moving the camera, always at the bottom of the tool stack */
  albaCameraTool *GetCameraTool() {return m_CameraTool;}

  /** Push a tool on top of the tool stack: it receives the events before the tools
  below it and before the default behavior. The PER keeps a reference to the tool. */
  void PushTool(albaInteractionTool *tool);

  /** Remove a tool from the tool stack, wherever it is */
  void PopTool(albaInteractionTool *tool);

  /** Offer a key pressed or released to the tools, from the top of the stack; return true if a tool used it */
  bool DispatchKey(int keyCode, bool pressed);

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
  
  /** Start the default interaction on a button down, with the three buttons */
  virtual int OnStartInteraction(albaEventInteraction *e);
  /** Stop the default interaction on a button up, with the three buttons */
  virtual int OnStopInteraction(albaEventInteraction *e);

  /** 
    Perform a pick on start of interaction to find if the user pointed
    an object in the scene, and in case forward all the following 
    events to its behavior. if the user pointed to the background
    forward events to the camera interactor.*/
  virtual void OnButtonDown       (albaEventInteraction *e);
  
  virtual void OnButtonUp         (albaEventInteraction *e);
  virtual void OnLeftButtonDown   (albaEventInteraction *e);
  virtual void OnLeftButtonUp     (albaEventInteraction *e);
  virtual void OnMiddleButtonDown (albaEventInteraction *e);
  virtual void OnMiddleButtonUp   (albaEventInteraction *e);
  virtual void OnRightButtonDown  (albaEventInteraction *e);
  virtual void OnRightButtonUp    (albaEventInteraction *e);
  virtual void OnMove             (albaEventInteraction *e);
  virtual void OnChar             (albaEventInteraction *e);

  /** internally used to fly to a clicked point */
  virtual void FlyTo(albaEventInteraction *e,int numstep=20, double zoom=1);

  /** insert a device in the set of devices currently interacting */
  void InsertDevice(albaDevice *device,int button);

  /** remove a device in the set of devices currently interacting */
  void RemoveDevice(albaDevice *device);

  /** internally used to set which behavior was picked by device */
  void SetCurrentBehavior(albaDevice *device,albaInteractionBehavior *inter);
  albaInteractionBehavior *GetCurrentBehavior(albaDevice *device);

  /** internally used to set VME picked by device at button down */
  void SetPickedVME(albaDevice *device,albaVME *vme);

  /** internally used to return the picked VME for given device */
  albaVME *GetPickedVME(albaDevice *device);

  /** Add who handled the current event to its trace (only when the event tracing is on) */
  void TraceHandler(const char *handler, albaObject *obj = NULL);

  /** Write the trace of the current event in the log (only when the event tracing is on) */
  void WriteEventTrace(albaEventBase *event);

  bool            m_EventTracing;   ///< true if the events are traced in the log
  std::string     m_EventTrace;     ///< who handled the current event
  std::string     m_LastEventTrace; ///< last trace written in the log

  bool            m_FlyToFlag;
  bool            m_DraggingLeft; 
  bool            m_CanSelect; 
  bool	          m_ShowContextMenu; ///< flag for showing the context menu

  int             m_Mode;
  int             m_FirstTime;
  /** Offer the event to the tools, from the top of the stack; return true if a tool used it */
  bool DispatchToTools(albaEventInteraction *e, albaDevice *device);

  /** Call the button down method of the tool for the button of the event */
  static bool ToolButtonDown(albaInteractionTool *tool, albaEventInteraction *e);

  /** Call the button up method of the tool for the button of the event */
  static bool ToolButtonUp(albaInteractionTool *tool, albaEventInteraction *e);

  /** Return true if the button down picks a VME with a behavior: the behavior comes before the tools */
  bool PickedVmeHasBehavior(albaEventInteraction *e, albaDevice *device);

  /** The tool interacting with a device and the button that started the interaction */
  struct ToolInteraction
  {
    albaInteractionTool *Tool;
    int Button;
  };

  albaCameraTool *m_CameraTool; ///< tool moving the camera, at the bottom of the stack
  std::vector<albaInteractionTool *> m_Tools; ///< the tool stack, the top is the last one
  std::map<albaID, ToolInteraction> m_ToolInteractions; ///< the tool interacting with each device, until the starting button is released

  class DeviceItem
  {
  public:
    albaVME *m_VME;
    int     m_Button;
  };
  
  std::map<albaID,DeviceItem> m_Devices; ///< Stores the list of devices currently interacting
  albaVME *m_VME; ///< the VME selected in the tree, used as picked VME when no device is given
  
private:
  albaPositionalEventRouter(const albaPositionalEventRouter&);  // Not implemented.
  void operator=(const albaPositionalEventRouter&);  // Not implemented.

  /** test friend */
  friend class albaPositionalEventRouterTest;
};

#endif 
