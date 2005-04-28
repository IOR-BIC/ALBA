/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDeviceManager.h,v $
  Language:  C++
  Date:      $Date: 2005-04-28 16:10:11 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/ 
#ifndef __mafDeviceManager_h
#define __mafDeviceManager_h

#ifdef __GNUG__
    #pragma interface "mafDeviceManager.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mafEventHandler.h"
#include "mflDefines.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafDevice;
class mafDeviceSet;
class mflXMLWriter;
class vtkXMLDataElement;
class vtkXMLDataParser;
template <class T> class vtkTemplatedList;

/**
  Class managing input device managment and synchronization with wxWindows message pump.
  This class is used both as a container for all active devices and manages synchronization
  of events coming from input devices with wxWindows messsage pump. To store devices
  a mafDeviceSet object is used.
  @sa mafDevice mafDeviceSet
 */
class mafDeviceManager : public mafEventHandler
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events
      advise dispatching has started */
  MFL_EVT_DEC(StartDispatchingEvent)
  /** @ingroup Events
      advise dispatching has finished  */
  MFL_EVT_DEC(EndDispatchingEvent) 

  static mafDeviceManager *New();
  vtkTypeMacro(mafDeviceManager,mafEventHandler);

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
  vtkTemplatedList<mafDevice> *GetDevices();

  /** return the device set storing first level devices */
  mafDeviceSet *GetDeviceSet() {return DeviceSet;}

  /**
    Implement storing and restoring of this object. To store
    an XML Writer must be passed as argument, while to restore the node
    of the XML structure from where starting the restoring must must be passed.
  */
  virtual int Store(mflXMLWriter *writer);
  virtual int Restore(vtkXMLDataElement *parent,vtkXMLDataParser *parser);

  virtual void ProcessEvent(mflEvent *event,mafID channel=mflAgent::DefaultChannel);

  /**
    Redefined to manage the synchronization with display rendering: during the event
    dispatching all rendering requests must be queued. For this reason at dispatching
    start a StartDispatchingEvent is (synchronously) sent on the default channel, and
    StopDispatchingEvent is sent when dispatching is finished */
  virtual bool DispatchEvents();
protected:
  mafDeviceManager();
  virtual ~mafDeviceManager();

  virtual int InternalInitialize();
  virtual void InternalShutdown();

  mafDeviceSet *DeviceSet;

  mafID DeviceIdCounter;
  mafID PersistentDeviceIdCounter;
  bool  RestoringFlag;
  
private:
  mafDeviceManager(const mafDeviceManager&);  // Not implemented.
  void operator=(const mafDeviceManager&);  // Not implemented.
};

#endif 
