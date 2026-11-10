/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorPER
 Authors: Marco Petrone, originally by Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaInteractorPER_h
#define __albaInteractorPER_h

#include "albaInteractor.h"
#include <map>
#include <string>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaVME;
class myPIMPL;
class vtkCamera;

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
class ALBA_EXPORT albaInteractorPER : public albaInteractor
{
public: 
  albaTypeMacro(albaInteractorPER,albaInteractor);

  virtual void EnableSelect(bool enable) {m_CanSelect = enable;};

  /** Enable fly to mode: next click will select where to fly to */
  virtual void FlyToMode() {m_FlyToFlag = true;};

  /** 
   return true if device is in the set of devices currently interacting
   the optional parameter can be used in SINGLE_BUTTON mode to know if 
   interaction has been started by a specific button. */
  bool IsInteracting(albaDevice *device, int button=-1);

  /** Redefined to support multiple input devices */
  virtual int StartInteraction(albaDevice *device, int button);
  
  /** Redefined to support multiple input devices */
  virtual int StopInteraction(albaDevice *device, int button);

  virtual void OnEvent(albaEventBase *event);

  void OnVmeSelected(albaVME *vme){ SetPickedVME(GetDevice(),vme); }

  enum modalities {SINGLE_BUTTON=0,MULTI_BUTTON};

  void SetMode(int mode) {m_Mode=mode;}
  int GetMode() {return m_Mode;}
  void SetModeToSingleButton() {m_Mode=SINGLE_BUTTON;}
  void SetModeToMultiButton() {m_Mode=MULTI_BUTTON;}

  albaInteractor *GetCameraMouseInteractor() {return m_CameraMouseBehavior;};

  albaInteractor *GetCameraInteractor() {return m_CameraBehavior;};

  /** Enable/disable the event tracing of all the PERs: each input event is written in the log
  with the device, the button, the modifiers and who handled it. */
  static void SetEventTracing(bool enable) {m_EventTracing = enable;}
  static bool GetEventTracing() {return m_EventTracing;}

  /** Return the last trace written in the log */
  const char *GetLastEventTrace() {return m_LastEventTrace.c_str();}

protected:
  albaInteractorPER();
  virtual ~albaInteractorPER();
  
  /** redefined to support the three buttons */  
  virtual int OnStartInteraction(albaEventInteraction *e);
  /** redefined to support the three buttons */
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
  void SetCurrentBehavior(albaDevice *device,albaInteractor *inter);
  albaInteractor *GetCurrentBehavior(albaDevice *device);

  /** internally used to set VME picked by device at button down */
  void SetPickedVME(albaDevice *device,albaVME *vme);

  /** internally used to return the picked VME for given device */
  albaVME *GetPickedVME(albaDevice *device);

  /** Add who handled the current event to its trace (only when the event tracing is on) */
  void TraceHandler(const char *handler, albaObject *obj = NULL);

  /** Write the trace of the current event in the log (only when the event tracing is on) */
  void WriteEventTrace(albaEventBase *event);

  static bool     m_EventTracing;   ///< static until the PER is unique, then it can become a normal member
  std::string     m_EventTrace;     ///< who handled the current event
  std::string     m_LastEventTrace; ///< last trace written in the log

  bool            m_FlyToFlag;
  bool            m_DraggingLeft; 
  bool            m_CanSelect; 
  bool	          m_ShowContextMenu; ///< flag for showing the context menu

  int             m_Mode;
  int             m_FirstTime;
  albaInteractor*  m_CameraBehavior; ///< the camera interactor
  albaInteractor*  m_CameraMouseBehavior; ///< the mouse camera interactor

  class DeviceItem
  {
  public:
    albaVME *m_VME;
    int     m_Button;
  };
  
  std::map<albaID,DeviceItem> m_Devices; ///< Stores the list of devices currently interacting
  
private:
  albaInteractorPER(const albaInteractorPER&);  // Not implemented.
  void operator=(const albaInteractorPER&);  // Not implemented.

  /** test friend */
  friend class albaInteractorPERTest;
};

#endif 
