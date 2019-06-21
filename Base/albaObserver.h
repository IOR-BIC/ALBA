/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaObserver
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaObserver_h
#define __albaObserver_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaObject.h"
#include "albaEvent.h"

//------------------------------------------------------------------------------
// albaObserver
//------------------------------------------------------------------------------
/** Interface implementing the Observer of the Subject/Observer design pattern.
  albaObserver is an abstract class implementing the "observer" in the Subject/Observer 
  design pattern. Objective of this object is to provide an interface for listening to events
  issued by subjects.
  An observer must be registered to a subject to create the communication channel 
  between the two. An observer can "observe" many subjects at the same time.
  @sa albaEventBroadcaster albaObserverCallback
*/
class ALBA_EXPORT albaObserver
{
public:
  albaObserver() {}
  virtual ~albaObserver() {}

  /** process the events sent by subjects */
  virtual void OnEvent(albaEventBase *e) = 0;
};

#endif /* __albaObserver_h */
