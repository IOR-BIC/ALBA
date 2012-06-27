/*=========================================================================

 Program: MAF2
 Module: mafAgentEventHandler
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafAgentEventHandler_h
#define __mafAgentEventHandler_h

#include "mafAgentEventQueue.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class mafWXEventHandler;

/** mafAgentEventHandler - This class synchronizes events with wxWindows event pump.
  mafAgentEventHandler is sused to synchronize event dispatching with the wxWindows 
  message pump.
@sa mflEvent mafAgentEventQueue */
class MAF_EXPORT mafAgentEventHandler : public mafAgentEventQueue
{
public:
  mafTypeMacro(mafAgentEventHandler,mafAgentEventQueue);
  
protected:
  mafAgentEventHandler();
  virtual ~mafAgentEventHandler();

  /** Internal function used to request the dispatching */
  virtual void RequestForDispatching();
  
  mafWXEventHandler *m_EventHandler;
private:
  mafAgentEventHandler(const mafAgentEventHandler&);  // Not implemented.
  void operator=(const mafAgentEventHandler&);  // Not implemented.

};


#endif 
