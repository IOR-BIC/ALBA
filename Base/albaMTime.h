/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMTime
 Authors: Marco Petrone, inspired to vtkTimeStamp (www.vtk.org)
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaMTime_h
#define __albaMTime_h

#include "albaConfigure.h"
#include "albaBase.h" 
#include "vtkType.h"

#ifdef ALBA_USE_VTK
class vtkTimeStamp;
#endif

/** albaMTime - record modification timestamp.
  albaMTime records a unique time stamp when the method Modified() is 
  called. This time is guaranteed to be monotonically increasing.
  If ALBA is build with VTK compilation enabled (ALBA_USE_VTK) this object
  uses a vtkTimeStamp as counter to have a unique time stamp against the
  two libraries.   
@todo
 - fix the InterlockedIncrement - which was temporary commented out (sil)
*/

class ALBA_EXPORT albaMTime : public albaBase
{
public:
  albaMTime() {m_ModifiedTime = 0;}; 



  virtual const char *GetTypeName() {return "albaMTime";};

  /**
    Update this object modification time. The modification time is
    just a monotonically increasing unsigned long integer. It is
    possible for this number to wrap around back to zero.
    This should only happen for processes that have been running
    for a very long time, while constantly changing objects
    within the program. When this does occur, the typical consequence
    should be that some filters will update themselves when really
    they don't need to. */
  void Modified();

  /** Return this object's Modified time. */
  unsigned long int GetMTime() const;

  /** Support comparisons of time stamp objects directly. */
  int operator>(albaMTime& ts) {return (GetMTime() > ts.GetMTime());};
  int operator<(albaMTime& ts) {return (GetMTime() < ts.GetMTime());};

  /** Allow for typecasting to unsigned long. */
  operator unsigned long() {return GetMTime();};

private:
  unsigned long m_ModifiedTime;
};

#endif

