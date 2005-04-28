/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDeviceSet.h,v $
  Language:  C++
  Date:      $Date: 2005-04-28 16:10:11 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafDeviceSet_h
#define __mafDeviceSet_h

#ifdef __GNUG__
    #pragma interface "mafDeviceSet.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mafDevice.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
template <class T> class vtkTemplatedList;

/**
  This class manages a set of devices storing the list of device pointers and 
  dispatching commands to strored devices, e.g. initializzation commands. Being an event 
  queue it gathers events coming from all devices (e.g. running on different threads) and
  serializes their dispatching.
  @sa mafDevice mflDispatcher mflEvent
  @todo
  - Improve Add/Remove device to manage the device tear up and shut down
  */
class mafDeviceSet : public mafDevice
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events 
      Adds a device (in Data argument) to this set (to be removed) */
  MFL_EVT_DEC(AddDeviceEvent);
  /** @ingroup Events 
      Remove a device (in Data argument) from this set (to be removed) */
  MFL_EVT_DEC(RemoveDeviceEvent);
  /** @ingroup Events 
      Issued when a new device (in Data argument) is added to this set */
  MFL_EVT_DEC(DeviceAddedEvent);
  /** @ingroup Events 
      Issued when a device (in Data argument) is being removed from this set */
  MFL_EVT_DEC(DeviceRemovingEvent); 
  /** @ingroup channel
      New channel for settings */
  MFL_EVT_DEC(DeviceSetUpChannel);
  /** @ingroup channel 
      New channel for settings */
  MFL_EVT_DEC(DeviceSetDownChannel);

  static mafDeviceSet *New();
  vtkTypeMacro(mafDeviceSet,mafDevice);

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
  virtual void ProcessEvent(mflEvent *event,mafID channel=mflAgent::DefaultChannel);

  /** Return the list of devices pluged into this device set */
  vtkTemplatedList<mafDevice> *GetDevices() {return Devices;}

protected:
  mafDeviceSet();
  virtual ~mafDeviceSet();

  virtual int InternalStore(mflXMLWriter *writer);
  virtual int InternalRestore(vtkXMLDataElement *node,vtkXMLDataParser *parser);

  /** Initialization automatically initialize subdevices */
  virtual int InternalInitialize();

  /** shutdowns also subdevices */
  virtual void InternalShutdown();

  vtkTemplatedList<mafDevice> *Devices;

  vtkSimpleCriticalSection *DevicesMutex;

private:
  mafDeviceSet(const mafDeviceSet&);  // Not implemented.
  void operator=(const mafDeviceSet&);  // Not implemented.
};

#endif 
