/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorSER
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaInteractorSER_h
#define __albaInteractorSER_h

#include "albaInteractor.h"
#include "albaAction.h"
#include "albaStorable.h"
#include <map>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaDevice;
class albaString;

/** Class implementing static routing from devices to interactors passing through actions.
  This class routes events from input devices to interactors according to a static binding
  defined by user (or application logic). Devices are connected to "actions", special 
  bridge agents that simply deliver events on the "DeviceInputChannelsEvents" coming from
  connected devices to any connected interactor, and events coming on the "DeviceOutputChannel",
  coming from any interactor to all connected devices. This allow the user to define how to use
  devices connected to the system, and the interactor to ignore which devices are providing 
  inputs. Its clear that an interactor will ignore any event coming from incompatible devices.
  Currently there is not device type control implementation.
  @todo
  - to implement a type safe static binding mechanims
   */
class ALBA_EXPORT albaInteractorSER : public albaInteractor, public albaStorable
{
public: 
  albaTypeMacro(albaInteractorSER,albaInteractor);

   /** Un/Bind a device to an action */
  void BindDeviceToAction(albaDevice *device,albaAction *action);
  /** Un/Bind a device to an action */
  void BindDeviceToAction(albaDevice *device,const char *action_name);
  /** Un/Bind a device to an action */
  void UnBindDeviceFromAction(albaDevice *device,albaAction *action);
  /** Un/Bind a device to an action */
  void UnBindDeviceFromAction(albaDevice *device,const char *action_name);
  /** Unbind a device from all actions: used when a device is removed */
  void UnBindDeviceFromAllActions(albaDevice *device);

  /** return list of device bindings to actions */
  void GetDeviceBindings(albaDevice *device,std::vector<albaAction *> &actions);
  /** 
    Bind an agent to the specified action. 
    If the action is not present return -1 */
  int BindAction(const char *action,albaInteractor *agent);
  /** 
    Unbind an agent from the specified action. 
    If the action is not present return -1 */
  int UnBindAction(const char *action,albaInteractor *agent);

  /** Define a new action router.*/
  albaAction *AddAction(const char *name, float priority = 0.0, int type = albaAction::SHARED_ACTION);
  void AddAction(albaAction *action, float priority = 0.0);
  
  /** Get an action router.*/
  albaAction *GetAction(const char *name);
  
  typedef std::map<albaString,albaAutoPointer<albaAction> > mmuActionsMap;

  /** Return the actions container */
  void GetActions(std::vector<albaAction *> &actions);
  const mmuActionsMap *GetActions() {return &m_Actions;}

protected:
  virtual int InternalStore(albaStorageElement *node);
  virtual int InternalRestore(albaStorageElement *node);

  albaInteractorSER();
  virtual ~albaInteractorSER();

  mmuActionsMap  m_Actions; ///< takes a list of the active actions

private:
  albaInteractorSER(const albaInteractorSER&);  // Not implemented.
  void operator=(const albaInteractorSER&);  // Not implemented.
};

#endif 
