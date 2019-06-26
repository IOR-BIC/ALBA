/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaObserverCallback
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaObserverCallback_h
#define __albaObserverCallback_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaObject.h"
#include "albaObserver.h"

//------------------------------------------------------------------------------
// albaObserverCallback
//------------------------------------------------------------------------------
/** Concrete implementation of Observer calling a callback function.
  albaObserverCallback is a class implementing an "observer" that launches a callback
  function. It's useful for attaching as observers objects that do not inherit from albaObserver.
  When an event is rise the callback function is called passing as argument some event content 
  plus event source Data and this class ClientData, that can be set to any value. Usually ClientData
  is used to store the (self) pointer of the class to whom the callback function belongs.
  @sa albaEventBroadcaster albaObserver
  @todo 
  - create a test
*/
class ALBA_EXPORT albaObserverCallback:public albaObject, public albaObserver
{
public:
  albaObserverCallback();
  virtual ~albaObserverCallback();

  albaTypeMacro(albaObserverCallback,albaObject);

  /** Set function callback to be called by this observer */
  void SetCallback(void (*f)(void *sender, albaID eid, void *clientdata));
   
  /** set client data to be passed to callback function */
  void SetClientData(void *cd);
    
  /** return client data passed to callback function */  
  void* GetClientData();
       
  /** process the events sent by subjects */
  virtual void OnEvent(albaEventBase *e);

protected:
  void (*m_Callback)(void *, albaID, void *);  
  void *m_ClientData;
};

#endif /* __albaObserverCallback_h */
