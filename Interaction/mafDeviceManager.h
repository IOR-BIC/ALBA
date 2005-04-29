/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDeviceManager.h,v $
  Language:  C++
  Date:      $Date: 2005-04-29 16:10:17 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/ 
#ifndef __mafDeviceManager_h
#define __mafDeviceManager_h

#include "mafAgentEventHandler.h"
#include "mafStorable.h"

#include <list>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafDevice;
class mafDeviceSet;

/**
  Class managing the devices and the synchronization with wxWindows message pump.
  This class is used both as a container for all active devices and manages synchronization
  of events coming from input devices with wxWindows message pump. To store devices
  a mafDeviceSet object is used.
  @sa mafDevice mafDeviceSet
  @todo 
  - implement AddDevice()
 */
class mafDeviceManager : public mafAgentEventHandler
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events
      advise dispatching has started */
  MAF_ID_DEC(DISPATCH_START)
  /** @ingroup Events
      advise dispatching has finished  */
  MAF_ID_DEC(DISPATCH_END) 

  mafDeviceManager();
  virtual ~mafDeviceManager();

  mafTypeMacro(mafDeviceManager,mafAgentEventHandler);

  /**
    Add a new device. If the returned value is 0 the operation has failed! */
  virtual mafID AddDevice(mafDevice *device);

  /**
    Add a new device specifying only its class name. If the returned pointer is NULL the
    operation has failed! "persistent" flag must be used to add persistent devices */
  virtual mafDevice *AddDevice(const char *type, bool persistent=false);

  /** Remove a device from the manager's list. force flag must be used to remove
      persistent devices */
  int RemoveDevice(mafDevice *device, bool force=false);
  /** Remove a device with the given name from the manager's list. */
  int RemoveDevice(const char *name, bool force=false);

  /**  Return a device handler given its name. */
  mafDevice *GetDevice(const char *name);

  /**  Return a device handler given its ID. Persistent devices have ID < MIN_DEVICE_ID */
  mafDevice *GetDevice(mafID id);

  /** Return the number of devices assigned to this manager */
  int GetNumberOfDevices();

   /** Remove all devices (as as RemoveAllSenders()), the "force" flag must be used
       to remove also persistent devices */
  void RemoveAllDevices(bool force=false);

  /** return the list of active devices */
  std::list<mafDevice *> *GetDevices();

  /** return the device set storing first level devices */
  mafDeviceSet *GetDeviceSet() {return m_DeviceSet;}

  virtual void OnEvent(mafEventBase *event);

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
  virtual int InternalStore(mafStorageElement *node);
  virtual int InternalRestore(mafStorageElement *node);

  virtual int InternalInitialize();
  virtual void InternalShutdown();

  mafDeviceSet*   m_DeviceSet;        ///< container for first level devices
  mafID           m_DeviceIdCounter;  ///< number of plugged devices
  mafID           m_PersistentDeviceIdCounter; ///< number of persistent plugged devices
  bool            m_RestoringFlag;
  
private:
  mafDeviceManager(const mafDeviceManager&);  // Not implemented.
  void operator=(const mafDeviceManager&);  // Not implemented.
};

#endif 
