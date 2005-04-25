/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmuIdFactory.h,v $
  Language:  C++
  Date:      $Date: 2005-04-25 18:20:30 $
  Version:   $Revision: 1.3 $
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

/** Base class for MFL events
  mmuIdFactory are objects sent arround the applications to rise an action. Events
  can be declared by means of MAF_ID_IMP and MAF_ID_DEC. These two macro respectivelly
  define and declare a static variable initialized to a univoque ID obtained by means
  of the GetNextId() function. Events can be declared as: class events (i.e. class
  member variables), global events (i.e. global member variables). In the first case it's
  neccessary to place the MAF_ID_DEC(evtname) in the class declaration scope, and the
  MAF_ID_IMP(evtname) in the object module file (.cxx or .cpp file). The this last it's
  necessaty to explicitelly specify the classname  as name as namespace (e.g 
  MAF_ID_IMP(classname::evtname). Global events can be declared in the global name space. 
  To do this, the MAF_ID_IMP(evtname) can be placed in a shared definition (.h) file, out of
  any scope, and the MAF_ID_DEC(evtname) in an associated .cpp or .cxx file. In synthesis events
  have the visibility of their associated static variables.
  Events are implemented as mmuIdFactory class, which is a vtkObject. Also an mafSmartEvent is 
  defined which can be allocated statically. Smart events are MFL smart pointers of
  MFL events, and thus have properties similar to MFL smart pointers.
  
  Events declaration rules. Events can be declared in three different places:
  - in the receiver object 
  - in the sender object 
  - in a shared place
  The first option is more suited for manager objects, since sender's must know the 
  destination class definition to have the event declaration
  The second option should be used in case of a container class that knows all its parts.
  In general this option should be avoided but in peer to peer comminication, since 
  it means the destination knows the source.
  The third option allows source and destination object to ignore one each other definition
  but has the side effect to creare a global source dependency from the single .h (and .cxx)
  files with shared defined events. Separate definition files could be used to reduce
  interdependency.
  The single ID counter ensure non conflicting event IDs, Indipendently from where/how the 
  event is declared defined.

  @sa mafAgent
*/
class MAF_EXPORT mmuIdFactory : public mmuUtility
{
public:
  /** Internally used to keep a unique ID for the events */
  static mafID GetNextId(const char *evtname);

  /** Alloc a number of IDs assigning a name to the group */
  static mafID AllocIdGroup(const char *evtname,int num);

  /** this function can be used to obtain an event ID, given its full name */
  static mafID GetId(const char *evtname);

  /** this function can be used to obtain an event Name, given its ID */
  static const char *GetIdName(mafID id);
protected:
  static mafID m_IdCounter;
};

#endif /* __mmuIdFactory_h */
