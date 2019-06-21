/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAction
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaAction_h
#define __albaAction_h

#include "albaAgent.h"
#include "albaStorable.h"
#include "albaSmartPointer.h"
#include <list>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
template <class T> class vtkTemplatedList;
class albaDevice;
class albaInteractor;
class vtkRenderer;

/** Class used to route events from devices to interactors */
class ALBA_EXPORT albaAction : public albaAgent, public albaStorable
{
public:
  /** @ingroup Events */
  /** @{ */
  ALBA_ID_DEC(DEVICE_BIND);            ///< force the action to bind to the device
  ALBA_ID_DEC(QUERY_CONNECTED_DEVICES);///< Issued by interactors to query for connected devices. @sa ConnectedDeviceEvent
  ALBA_ID_DEC(DEVICE_PLUGGED);         ///< Issued by devices when connected to actions or when queried.
  ALBA_ID_DEC(DEVICE_UNPLUGGED);       ///< Issued by devices when connected to actions or when queried.

  enum ACTION_TYPE
  {
    SHARED_ACTION = 0, 
    EXCLUSIVE_ACTION 
  };

  typedef std::list<albaAutoPointer<albaDevice> > mmuDeviceList;
  
  albaTypeMacro(albaAction,albaAgent);

  /** Set the type of action*/
  int GetType() {return m_Type;}
  void SetType(int t) {if (t==SHARED_ACTION||t==EXCLUSIVE_ACTION) m_Type=t;}
  void SetTypeToShared() {SetType(SHARED_ACTION);}
  void SetTypeToExclusive() {SetType(EXCLUSIVE_ACTION);}
  
  /** Bind/Unbind a device to this action */
  void BindDevice(albaDevice *device);
  void UnBindDevice(albaDevice *device);

  /** Bind/Unbind an interactor to this action */
  void BindInteractor(albaInteractor *inter);
  void UnBindInteractor(albaInteractor *inter);

  /** Get list of devices assigned to this action */
  const mmuDeviceList *GetDevices() {return &m_Devices;}

  /** Store of action's bindings to an XML file. */
  virtual int InternalStore(albaStorageElement *node);

  /** Restore of action's bindings from an XML file.*/
  virtual int InternalRestore(albaStorageElement *node);

  /** Redefined to answer incoming queries about connected devices. */
  virtual void OnEvent(albaEventBase *event);

protected:
  albaAction();
  virtual ~albaAction();

  int   m_Type;
  std::list<albaAutoPointer<albaDevice> > m_Devices;

private:
  albaAction(const albaAction&);  // Not implemented.
  void operator=(const albaAction&);  // Not implemented.
};

#endif 
