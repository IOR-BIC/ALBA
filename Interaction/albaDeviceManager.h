/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceManager
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/ 
#ifndef __albaDeviceManager_h
#define __albaDeviceManager_h

#include "albaAgentEventHandler.h"
#include "albaStorable.h"

#include <list>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaDevice;
class albaDeviceSet;

/**
  Class managing the devices inside a ALBA application and the synchronization with wxWindows message pump.
  This class is used both as a container for all active devices and manages synchronization
  of events coming from input devices with wxWindows message pump.
  The synchronization mechanisms is accomplished by executing the "dispatching" on a separate thread, and this is 
  currently imlemented by sending an event on the GUI message pump with a request for dispatching the events. This
  make dispatching to occurs on the right thread @sa albaAgentEventThreaded.

  Device instances can be of two different types: persistent and non-persistent. The first class of devices are
  typically created by the application code, while the seconds are created by user using the Interaction Settings
  configuration dialog. Persistent devices are not stored to disk and neither they are stopped or destroyed when
  a new device configuration is loaded from disk. Typically a non persistent device object pointer should not be
  stored by other componenents, since when loading new configuration these devices are destroyed a new one are 
  created.  An example of persistent device is the mouse, which is created at application start.
  
  To store devices a albaDeviceSet object is internally used as the root of the devices tree. Devices that could have
  subdevices can inherit from albaDeviceSet to correctly manage the start/stop mechanisms of all subdevices.
  
  @sa albaDevice albaDeviceSet
  @todo 
  - implement AddDevice()
 */
class ALBA_EXPORT albaDeviceManager : public albaAgentEventHandler, public albaStorable
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events
      advise dispatching has started */
  ALBA_ID_DEC(DISPATCH_START)
  /** @ingroup Events
      advise dispatching has finished  */
  ALBA_ID_DEC(DISPATCH_END) 

  albaDeviceManager();
  virtual ~albaDeviceManager();

  albaTypeMacro(albaDeviceManager,albaAgentEventHandler);

  /**
    Add a new device. If the returned value is 0 the operation has failed! */
  virtual albaID AddDevice(albaDevice *device);

  /**
    Add a new device specifying only its class name.
    If the returned pointer is NULL the operation has failed! "persistent" flag must be
    used to add persistent devices */
  virtual albaDevice *AddDevice(const char *type, bool persistent=false);

  /** Remove a device from the manager's list. force flag must be used to remove
      persistent devices */
  int RemoveDevice(albaDevice *device, bool force=false);
  /** Remove a device with the given name from the manager's list. */
  int RemoveDevice(const char *name, bool force=false);

  /**  Return a device handler given its name. */
  albaDevice *GetDevice(const char *name);

  /**  Return a device handler given its ID. Persistent devices have ID < MIN_DEVICE_ID */
  albaDevice *GetDevice(albaID id);

  /** Return the number of devices assigned to this manager */
  int GetNumberOfDevices();

   /** Remove all devices (as as RemoveAllSenders()), the "force" flag must be used
       to remove also persistent devices */
  void RemoveAllDevices(bool force=false);

  /** return the list of active devices */
  std::list<albaDevice *> *GetDevices();

  /** return the device set storing first level devices */
  albaDeviceSet *GetDeviceSet() {return m_DeviceSet;}

  virtual void OnEvent(albaEventBase *event);

  /**
    Redefined to manage the synchronization with display rendering: during the event
    dispatching all rendering requests must be queued. For this reason at dispatching
    start a StartDispatchingEvent is (synchronously) sent on the default channel, and
    StopDispatchingEvent is sent when dispatching is finished */
  virtual bool DispatchEvents();
protected:
  /**
  Implement storing and restoring of this object. To store
  an XML Writer must be passed as argument, while to restore the node
  of the XML structure from where starting the restoring must must be passed.  */
  virtual int InternalStore(albaStorageElement *node);
  virtual int InternalRestore(albaStorageElement *node);

  virtual int InternalInitialize();
  virtual void InternalShutdown();

  albaDeviceSet*   m_DeviceSet;        ///< container for first level devices
  albaID           m_DeviceIdCounter;  ///< number of plugged devices
  albaID           m_PersistentDeviceIdCounter; ///< number of persistent plugged devices
  bool            m_RestoringFlag;
  
private:
  albaDeviceManager(const albaDeviceManager&);  // Not implemented.
  void operator=(const albaDeviceManager&);  // Not implemented.
};

#endif 
