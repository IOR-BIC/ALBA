/*=========================================================================

  Program:   Multimod Foundation Library
  Module:    $RCSfile: mafEventHandler.h,v $
  Language:  C++
  Date:      $Date: 2005-04-28 16:10:11 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone m.petrone@cineca.it
  Project:   MultiMod Project (www.ior.it/multimod)

==========================================================================
  Copyright (c) 2002/2003
  CINECA - Interuniversity Consortium (www.cineca.it)
  v. Magnanelli 6/3 - 40033 Casalecchio di Reno (BO) - Italy
  ph. +39-051-6171411 (90 lines) - Fax +39-051-6132198
=========================================================================*/
// .NAME mafEventHandler - This class synchronizes events with wxWindows pump 
// .SECTION Description
// mafEventHandler is sused to synchronize event dispatching with the wxWindows 
// message pump.
// .SECTION SeeAlso
// mflEvent mflEventQueue


#ifndef __mafEventHandler_h
#define __mafEventHandler_h

#include "mflEventQueue.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class mafWXEventHandler;

class mafEventHandler : public mflEventQueue
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  

  static mafEventHandler *New();
  vtkTypeMacro(mafEventHandler,mflEventQueue);
  
protected:
  mafEventHandler();
  virtual ~mafEventHandler();

  // Description:
  // Internal function used to request the dispatching
  virtual void RequestForDispatching();
  
  mafWXEventHandler *m_EventHandler;

private:
  mafEventHandler(const mafEventHandler&);  // Not implemented.
  void operator=(const mafEventHandler&);  // Not implemented.

};


#endif 
