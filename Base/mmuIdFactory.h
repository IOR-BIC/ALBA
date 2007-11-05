/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmuIdFactory.h,v $
  Language:  C++
  Date:      $Date: 2007-11-05 12:43:02 $
  Version:   $Revision: 1.5 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmuIdFactory_h
#define __mmuIdFactory_h

#include "mmuUtility.h"
#include "mafDefines.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------

/** @defgroup Events Events and channel Ids defined in the project */ // used for grouping Ids with doxygen
/** @defgroup mafIds Generic Ids defined in the project */ // used for grouping Ids with doxygen

/** Factory for Ids used to generate unique Ids at runtime.
  This object is used to generate unique Ids and is typically accessed by means of MAF macros for ID declaration 
  defined in mafDefines.h (e.g. MAF_ID_DEC() MAF_ID_GLOB() )
  @sa mafEvent
*/
class MAF_EXPORT mmuIdFactory : public mmuUtility
{
public:
  /** Internally used to keep a unique ID for the events */
  static mafID GetNextId(const char *evtname);

  /** Alloc a number of IDs assigning a name to the group */
  //not used
  //static mafID AllocIdGroup(const char *evtname,int num);

  /** this function can be used to obtain an event ID, given its full name */
  static mafID GetId(const char *evtname);

  /** this function can be used to obtain an event Name, given its ID */
  static const char *GetIdName(mafID id);
protected:
  static mafID m_IdCounter;
};

#endif /* __mmuIdFactory_h */
