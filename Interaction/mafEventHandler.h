/*=========================================================================

  Program:   Multimod Foundation Library
  Module:    $RCSfile: mafEventHandler.h,v $
  Language:  C++
  Date:      $Date: 2009-06-03 15:28:53 $
  Version:   $Revision: 1.1.22.1 $
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
/**
  Class Name: mafEventHandler.
   A class that can handle events from the windowing system, via bridge class 
   mafWXEventHandler.
*/
class mafEventHandler : public mflEventQueue
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  
  /** static function for create new instance of the class. */
  static mafEventHandler *New();
  /** RTTI macro. */
  vtkTypeMacro(mafEventHandler,mflEventQueue);
  
protected:
  /** constructor.*/
  mafEventHandler();
  /** destructor. */
  virtual ~mafEventHandler();

  
  /**Internal function used to request the dispatching. */
  virtual void RequestForDispatching();
  
  mafWXEventHandler *m_EventHandler;

private:
  /**copy constructor not implemeted .*/
  mafEventHandler(const mafEventHandler&);
  /**assignment operator  not implemeted .*/
  void operator=(const mafEventHandler&);

};


#endif 
