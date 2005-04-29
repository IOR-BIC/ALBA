/*========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafAgentEventHandler.h,v $
Language:  C++
Date:      $Date: 2005-04-29 16:10:17 $
Version:   $Revision: 1.1 $
Authors:   Marco Petrone
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
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
class mafAgentEventHandler : public mafAgentEventQueue
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
