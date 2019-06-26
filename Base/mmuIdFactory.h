/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmuIdFactory
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mmuIdFactory_h
#define __mmuIdFactory_h

#include "albaUtility.h"
#include "albaDefines.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------

/** @defgroup Events Events and channel Ids defined in the project */ // used for grouping Ids with doxygen
/** @defgroup albaIds Generic Ids defined in the project */ // used for grouping Ids with doxygen

/** Factory for Ids used to generate unique Ids at runtime.
  This object is used to generate unique Ids and is typically accessed by means of ALBA macros for ID declaration 
  defined in albaDefines.h (e.g. ALBA_ID_DEC() ALBA_ID_GLOB() )
  @sa albaEvent
*/
class ALBA_EXPORT mmuIdFactory : public albaUtility
{
public:
  /** Internally used to keep a unique ID for the events */
  static albaID GetNextId(const char *evtname);

  /** Alloc a number of IDs assigning a name to the group */
  //not used
  //static albaID AllocIdGroup(const char *evtname,int num);

  /** this function can be used to obtain an event ID, given its full name */
  static albaID GetId(const char *evtname);

  /** this function can be used to obtain an event Name, given its ID */
  static const char *GetIdName(albaID id);
protected:
  static albaID m_IdCounter;

  /** friend test class */
  friend class mmuIdFactoryTest;
};

#endif /* __mmuIdFactory_h */
