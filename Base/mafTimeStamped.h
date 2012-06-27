/*=========================================================================

 Program: MAF2
 Module: mafTimeStamped
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafTimeStamped_h
#define __mafTimeStamped_h

#include "mafMTime.h"


//------------------------------------------------------------------------------
// mafTimeStamped
//------------------------------------------------------------------------------
/** class acting as an interface for timestamped objects
  This object simply defines few methods for managing a modification time to be used in 
  for process objects.
  @sa mafMTime
*/
class MAF_EXPORT mafTimeStamped
{
public:
  /**
    Update this objects modification time. The modification time is
    just a monotonically increasing unsigned long integer. It is
    possible for this number to wrap around back to zero.
    This should only happen for processes that have been running
    for a very long time, while constantly changing objects
    within the program. When this does occur, the typical consequence
    should be that some process objects will update themselves when really
    they don't need to. */
  virtual void Modified();

  /** return modification time */
  virtual unsigned long GetMTime();

protected:
  mafMTime          m_MTime;        ///< Last modification time
private:
  
};

#endif /* __mafTimeStamped_h */
