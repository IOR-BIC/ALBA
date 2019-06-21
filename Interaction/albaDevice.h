/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDevice
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaDevice_h
#define __albaDevice_h

#include "albaAgentThreaded.h"
#include "albaStorable.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaGUI;

/** 
  This abstract class manages function calls coming from devices and 
  issue specific events to interactors. Usually all evetns sent by devices are synchronized
  with the application by sending a request for dispatching to the device manager.
  Devices are typically managed by the device manager object, whose responsible for keeping a list
  of the allocated devices, storing this list and all devices configurations, and restoring all devices
  from a saved file. Devices are devided in two cathegories, persistent and non persistent. The first 
  ones are not saved and restored, but have a life span corresponding to the application's one.
  @sa albaDeviceManger
  @todo
   - check about ID restoring
 */
class ALBA_EXPORT albaDevice : public albaAgentThreaded, public albaStorable
{
public:
  /** non persistent device IDs start from ALBA_MIN_DEVICE_ID. This means I cannot have more then 
    MIN_DEVICE_ID persistent devices (i.e. devices defined by application, e.g. Mouse
    and remote devices...) */
  enum DEVICE_MINIMUM_ID {MIN_DEVICE_ID=1000000};

  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events */
  /** @{ */
  ALBA_ID_DEC(DEVICE_NAME_CHANGED) ///< Rised when the device name is changed
  ALBA_ID_DEC(DEVICE_STARTED); ///< Issued when device is initialized
  ALBA_ID_DEC(DEVICE_STOPPED); ///< Issued when device is shutdown
  /** @} */
  
  //----------------------------------------------------------------------------
  //    GUI Constants
  //----------------------------------------------------------------------------
  enum DEVICE_WIDGET_ID
  {
    ID_NAME=MINID,
    ID_ACTIVATE,
    ID_SHUTDOWN,
    ID_AUTO_START,
    ID_LAST
  };

  albaTypeMacro(albaDevice,albaAgentThreaded);

  /**
   Get the ID for this device. This is usually set by albaDeviceSet */
  unsigned long GetID() {return m_ID;};

  /**
   Set ID for this device. This is usually set by albaDeviceSet */
  void SetID(unsigned long id) {m_ID=id;};

  /** Set the Device name and rise an event to advise consumers */
  virtual void SetName(const char *name);

  /** Set the AutoStart flag. This makes the device automatically 
      restart net time the application is started */
  void SetAutoStart(bool flag) {m_AutoStart=flag;}
  bool GetAutoStart() { return m_AutoStart!=0;}

  /** 
    Enable autostarting of this device */
  void AutoStartOn() {m_AutoStart=true;}

  /** 
    Disable autostarting of this device */
  void AutoStartOff() {m_AutoStart=false;}

  /** use this function to start (initialize) the device */
  virtual int Start();

  /** use this function to stop (shutdown) the device */
  virtual void Stop();

  /** 
    This checks AutoStart flag and if true Initialize the device
    Return false if AutoStart is OFF or initialization failed */
  virtual int StartUp() { return m_AutoStart?Start():0;}

  /** 
    set peristent flag: a persistent device is not stored/restored and
    neither removed when settings are loaded from disk. It's created by
    application logic and not by user! Example of persistent devices
    are mouse and remote devices */
  void SetPersistentFlag(bool flag) { m_PersistentFalg = flag; }
  bool GetPersistentFlag() {return m_PersistentFalg;}

  /** return true if it's a persistent device. @sa SetPersistentFlag() */
  bool IsPersistent() {return m_PersistentFalg;}
  
  /** Return pointer to the GUI. */
  albaGUI *GetGui();
  
  /** used to force Gui to update its content */
  virtual void UpdateGui();
  
  /** process events sent to the device */
  virtual void OnEvent(albaEventBase *event);

protected:
  albaDevice();
  virtual ~albaDevice();

  /** start device */
  virtual int InternalInitialize();

  /** Create the dialog that show the interface for settings. */
  virtual void CreateGui();

  /** This is used to allow nested serialization of subclasses.
    This function is called by Store and is reimplemented in subclasses.
    Each subclass can store its own subelements which are
    closed inside the "Device" element. Reimplemented functions
    should first call Superclass implementation. */
  virtual int InternalStore(albaStorageElement *node);
  
  /** 
    This function fills in the device with settings restored from the node.
    Subclasses should reimplement it to restore custom settings. Reimplemented
    functions should first call Superclass implementation. */
  virtual int InternalRestore(albaStorageElement *node);
  

  albaGUI*             m_Gui;
  albaID               m_ID;
  int                 m_AutoStart;
  int                 m_Start;  //SIL. 07-jun-2006 : -- connected to a Bool in the Gui
  bool                m_Locked;
  bool                m_PersistentFalg;

private:
  albaDevice(const albaDevice&);  // Not implemented.
  void operator=(const albaDevice&);  // Not implemented.

  /** hidden to avoid usage: use Start() instead */
  int Initialize() {return Superclass::Initialize();}

  /** hidden to avoid usage: use Stop() instead */
  void Shutdown() {Superclass::Shutdown();} 
};

#endif 
