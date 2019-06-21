/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceSet
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaDeviceSet_h
#define __albaDeviceSet_h

#include "albaDevice.h"
#include <list>

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_LIST(ALBA_EXPORT,albaDevice *);
#endif

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaMutexLock;
/**
  This class manages a set of usb-devices, and stores a list of all child devices' pointers. It also
  dispatches some commands to child devices, e.g. initializzation commands. Being also an event 
  queue, this class gathers events coming from all devices (e.g. running on different threads) and
  serializes their dispatching.
  @sa albaDeviceManager albaEvent
  */
class ALBA_EXPORT albaDeviceSet : public albaDevice
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events 
      Adds a device (passed in the Data argument) to this set (to be removed) */
  ALBA_ID_DEC(DEVICE_ADD);
  /** @ingroup Events 
      Remove a device (passed in the Data argument) from this set (to be removed) */
  ALBA_ID_DEC(DEVICE_REMOVE);
  /** @ingroup Events 
      Issued when a new device (passed in the Data argument) is added to this set */
  ALBA_ID_DEC(DEVICE_ADDED);
  /** @ingroup Events 
      Issued when a device (passed in the Data argument) is being removed from this set */
  ALBA_ID_DEC(DEVICE_REMOVING); 
  /** @ingroup channel
      New channel for settings */
  //ALBA_ID_DEC(MCH_DEVICE_SETUP);
  /** @ingroup channel 
      New channel for settings */
  //ALBA_ID_DEC(MCH_DEVICE_SETDOWN);

  albaTypeMacro(albaDeviceSet,albaDevice);

  /** Add a new device. If the returned value is false the operation has failed! */
  virtual void AddDevice(albaDevice *device);

  /**  Return a device in the set given its ID */
  albaDevice *GetDeviceByIndex(int idx);

  /**  Return a device in the set given its ID */
  albaDevice *GetDevice(albaID id);
  
  /**  Return a device in the set given its name */
  albaDevice *GetDevice(const char *name);

  /** Return the number of devices assigned to this manager */
  int GetNumberOfDevices();
  
  /** Remove a device from the manager's list given its ID. */
  int RemoveDeviceByIndex(int idx, bool force=false);

  /** Remove a device from the manager's list given its ID. */
  int RemoveDevice(albaID id, bool force=false);
  
  /** Remove a device from the manager's list */
  int RemoveDevice(albaDevice *device, bool force=false);
  
  /** Remove a device from the manager's list given its name. */
  int RemoveDevice(const char *name, bool force=false);

  /** Remove all devices (as as RemoveAllSenders()) */
  void RemoveAllDevices(bool force=false);

  /** Reimplemented to manage AddDevice event */
  virtual void OnEvent(albaEventBase *event);

  /** Return the list of devices pluged into this device set */
  std::list<albaDevice*> *GetDevices() {return &m_Devices;}

protected:
  albaDeviceSet();
  virtual ~albaDeviceSet();

  virtual int InternalStore(albaStorageElement *node);
  virtual int InternalRestore(albaStorageElement *node);

  /** Initialization automatically initialize subdevices */
  virtual int InternalInitialize();

  /** shutdown also subdevices */
  virtual void InternalShutdown();

  std::list<albaDevice *> m_Devices;

  albaMutexLock*          m_DevicesMutex;

private:
  albaDeviceSet(const albaDeviceSet&);  // Not implemented.
  void operator=(const albaDeviceSet&);  // Not implemented.
};

#endif 
