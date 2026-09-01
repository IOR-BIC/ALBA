/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTimeStamped
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaTimeStamped_h
#define __albaTimeStamped_h

#include "albaMTime.h"


//------------------------------------------------------------------------------
// albaTimeStamped
//------------------------------------------------------------------------------
/** class acting as an interface for timestamped objects
  This object simply defines few methods for managing a modification time to be used in 
  for process objects.
  @sa albaMTime
*/
class ALBA_EXPORT albaTimeStamped
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
  virtual vtkMTimeType GetMTime();

protected:
  albaMTime          m_MTime;        ///< Last modification time
private:
  
};

#endif /* __albaTimeStamped_h */
