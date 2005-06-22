/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiSER.h,v $
  Language:  C++
  Date:      $Date: 2005-06-22 16:44:19 $
  Version:   $Revision: 1.6 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmiSER_h
#define __mmiSER_h

#include "mafInteractor.h"
#include "mafAction.h"
#include "mafStorable.h"
#include <map>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafDevice;
class mafString;

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
class mmiSER : public mafInteractor, public mafStorable
{
public: 
  mafTypeMacro(mmiSER,mafInteractor);

   /** Un/Bind a device to an action */
  void BindDeviceToAction(mafDevice *device,mafAction *action);
  /** Un/Bind a device to an action */
  void BindDeviceToAction(mafDevice *device,const char *action_name);
  /** Un/Bind a device to an action */
  void UnBindDeviceFromAction(mafDevice *device,mafAction *action);
  /** Un/Bind a device to an action */
  void UnBindDeviceFromAction(mafDevice *device,const char *action_name);
  /** Unbind a device from all actions: used when a device is removed */
  void UnBindDeviceFromAllActions(mafDevice *device);

  /** return list of device bindings to actions */
  void GetDeviceBindings(mafDevice *device,std::vector<mafAction *> &actions);
  /** 
    Bind an agent to the specified action. 
    If the action is not present return -1 */
  int BindAction(const char *action,mafInteractor *agent);
  /** 
    Unbind an agent from the specified action. 
    If the action is not present return -1 */
  int UnBindAction(const char *action,mafInteractor *agent);

  /** Define a new action router.*/
  mafAction *AddAction(const char *name, float priority = 0.0, int type = mafAction::SHARED_ACTION);
  void AddAction(mafAction *action, float priority = 0.0);
  
  /** Get an action router.*/
  mafAction *GetAction(const char *name);
  
  typedef std::map<mafString,mafAutoPointer<mafAction> > mmuActionsMap;

  /** Return the actions container */
  void GetActions(std::vector<mafAction *> &actions);
  const mmuActionsMap *GetActions() {return &m_Actions;}

protected:
  virtual int InternalStore(mafStorageElement *node);
  virtual int InternalRestore(mafStorageElement *node);

  mmiSER();
  virtual ~mmiSER();

  mmuActionsMap  m_Actions; ///< takes a list of the active actions

private:
  mmiSER(const mmiSER&);  // Not implemented.
  void operator=(const mmiSER&);  // Not implemented.
};

#endif 
