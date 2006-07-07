/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDeviceSet.h,v $
  Language:  C++
  Date:      $Date: 2006-07-07 08:22:03 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafDeviceSet_h
#define __mafDeviceSet_h

#include "mafDevice.h"
#include <list>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafMutexLock;
/**
  This class manages a set of usb-devices, and stores a list of all child devices' pointers. It also
  dispatches some commands to child devices, e.g. initializzation commands. Being also an event 
  queue, this class gathers events coming from all devices (e.g. running on different threads) and
  serializes their dispatching.
  @sa mafDeviceManager mafEvent
  */
class mafDeviceSet : public mafDevice
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events 
      Adds a device (passed in the Data argument) to this set (to be removed) */
  MAF_ID_DEC(DEVICE_ADD);
  /** @ingroup Events 
      Remove a device (passed in the Data argument) from this set (to be removed) */
  MAF_ID_DEC(DEVICE_REMOVE);
  /** @ingroup Events 
      Issued when a new device (passed in the Data argument) is added to this set */
  MAF_ID_DEC(DEVICE_ADDED);
  /** @ingroup Events 
      Issued when a device (passed in the Data argument) is being removed from this set */
  MAF_ID_DEC(DEVICE_REMOVING); 
  /** @ingroup channel
      New channel for settings */
  //MAF_ID_DEC(MCH_DEVICE_SETUP);
  /** @ingroup channel 
      New channel for settings */
  //MAF_ID_DEC(MCH_DEVICE_SETDOWN);

  mafTypeMacro(mafDeviceSet,mafDevice);

  /** Add a new device. If the returned value is false the operation has failed! */
  virtual void AddDevice(mafDevice *device);

  /**  Return a device in the set given its ID */
  mafDevice *GetDeviceByIndex(int idx);

  /**  Return a device in the set given its ID */
  mafDevice *GetDevice(mafID id);
  
  /**  Return a device in the set given its name */
  mafDevice *GetDevice(const char *name);

  /** Return the number of devices assigned to this manager */
  int GetNumberOfDevices();
  
  /** Remove a device from the manager's list given its ID. */
  int RemoveDeviceByIndex(int idx, bool force=false);

  /** Remove a device from the manager's list given its ID. */
  int RemoveDevice(mafID id, bool force=false);
  
  /** Remove a device from the manager's list */
  int RemoveDevice(mafDevice *device, bool force=false);
  
  /** Remove a device from the manager's list given its name. */
  int RemoveDevice(const char *name, bool force=false);

  /** Remove all devices (as as RemoveAllSenders()) */
  void RemoveAllDevices(bool force=false);

  /** Reimplemented to manage AddDevice event */
  virtual void OnEvent(mafEventBase *event);

  /** Return the list of devices pluged into this device set */
  std::list<mafDevice*> *GetDevices() {return &m_Devices;}

protected:
  mafDeviceSet();
  virtual ~mafDeviceSet();

  virtual int InternalStore(mafStorageElement *node);
  virtual int InternalRestore(mafStorageElement *node);

  /** Initialization automatically initialize subdevices */
  virtual int InternalInitialize();

  /** shutdown also subdevices */
  virtual void InternalShutdown();

  std::list<mafDevice *> m_Devices;

  mafMutexLock*          m_DevicesMutex;

private:
  mafDeviceSet(const mafDeviceSet&);  // Not implemented.
  void operator=(const mafDeviceSet&);  // Not implemented.
};

#endif 
