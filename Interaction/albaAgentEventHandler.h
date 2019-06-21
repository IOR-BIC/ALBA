/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAgentEventHandler
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaAgentEventHandler_h
#define __albaAgentEventHandler_h

#include "albaAgentEventQueue.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class albaWXEventHandler;

/** albaAgentEventHandler - This class synchronizes events with wxWindows event pump.
  albaAgentEventHandler is sused to synchronize event dispatching with the wxWindows 
  message pump.
@sa mflEvent albaAgentEventQueue */
class ALBA_EXPORT albaAgentEventHandler : public albaAgentEventQueue
{
public:
  albaTypeMacro(albaAgentEventHandler,albaAgentEventQueue);
  
protected:
  albaAgentEventHandler();
  virtual ~albaAgentEventHandler();

  /** Internal function used to request the dispatching */
  virtual void RequestForDispatching();
  
  albaWXEventHandler *m_EventHandler;
private:
  albaAgentEventHandler(const albaAgentEventHandler&);  // Not implemented.
  void operator=(const albaAgentEventHandler&);  // Not implemented.

};


#endif 
