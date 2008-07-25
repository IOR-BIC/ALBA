/*========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDevice.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:03:38 $
  Version:   $Revision: 1.6 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafDevice_h
#define __mafDevice_h

#include "mafAgentThreaded.h"
#include "mafStorable.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafGUI;

/** 
  This abstract class manages function calls coming from devices and 
  issue specific events to interactors. Usually all evetns sent by devices are synchronized
  with the application by sending a request for dispatching to the device manager.
  Devices are typically managed by the device manager object, whose responsible for keeping a list
  of the allocated devices, storing this list and all devices configurations, and restoring all devices
  from a saved file. Devices are devided in two cathegories, persistent and non persistent. The first 
  ones are not saved and restored, but have a life span corresponding to the application's one.
  @sa mafDeviceManger
  @todo
   - check about ID restoring
 */
class mafDevice : public mafAgentThreaded, public mafStorable
{
public:
  /** non persistent device IDs start from MAF_MIN_DEVICE_ID. This means I cannot have more then 
    MIN_DEVICE_ID persistent devices (i.e. devices defined by application, e.g. Mouse
    and remote devices...) */
  enum DEVICE_MINIMUM_ID {MIN_DEVICE_ID=1000000};

  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events */
  /** @{ */
  MAF_ID_DEC(DEVICE_NAME_CHANGED) ///< Rised when the device name is changed
  MAF_ID_DEC(DEVICE_STARTED); ///< Issued when device is initialized
  MAF_ID_DEC(DEVICE_STOPPED); ///< Issued when device is shutdown
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

  mafTypeMacro(mafDevice,mafAgentThreaded);

  /**
   Get the ID for this device. This is usually set by mafDeviceSet */
  unsigned long GetID() {return m_ID;};

  /**
   Set ID for this device. This is usually set by mafDeviceSet */
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

  /** Lock this device, this is used by interactors needing
      an exclusive use of the device. This is not thread safe (!)
      since events are serialized and thus it should be for
      the event starting the interaction and calling the Lock.
      Return true if the locking was successful false if 
      another interactor has already locked the device */
  bool Lock() {return IsLocked()?false:m_Locked=true;}
  void Unlock() {m_Locked = false;}
 
  /** return true if locked */
  bool IsLocked() {return m_Locked;}

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
  mafGUI *GetGui();
  
  /** used to force Gui to update its content */
  virtual void UpdateGui();
  
  /** process events sent to the device */
  virtual void OnEvent(mafEventBase *event);

protected:
  mafDevice();
  virtual ~mafDevice();

  /** start device */
  virtual int InternalInitialize();

  /** Create the dialog that show the interface for settings. */
  virtual void CreateGui();

  /** This is used to allow nested serialization of subclasses.
    This function is called by Store and is reimplemented in subclasses.
    Each subclass can store its own subelements which are
    closed inside the "Device" element. Reimplemented functions
    should first call Superclass implementation. */
  virtual int InternalStore(mafStorageElement *node);
  
  /** 
    This function fills in the device with settings restored from the node.
    Subclasses should reimplement it to restore custom settings. Reimplemented
    functions should first call Superclass implementation. */
  virtual int InternalRestore(mafStorageElement *node);
  

  mafGUI*             m_Gui;
  mafID               m_ID;
  int                 m_AutoStart;
  int                 m_Start;  //SIL. 07-jun-2006 : -- connected to a Bool in the Gui
  bool                m_Locked;
  bool                m_PersistentFalg;

private:
  mafDevice(const mafDevice&);  // Not implemented.
  void operator=(const mafDevice&);  // Not implemented.

  /** hidden to avoid usage: use Start() instead */
  int Initialize() {return Superclass::Initialize();}

  /** hidden to avoid usage: use Stop() instead */
  void Shutdown() {Superclass::Shutdown();} 
};

#endif 
