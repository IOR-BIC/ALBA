/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMTime.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:56:33 $
  Version:   $Revision: 1.7 $
  Authors:   Marco Petrone, inspired to vtkTimeStamp (www.vtk.org)
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafMTime_h
#define __mafMTime_h

#include "mafConfigure.h"
#include "mafBase.h" 

#ifdef MAF_USE_VTK
class vtkTimeStamp;
#endif

/** mafMTime - record modification timestamp.
  mafMTime records a unique time stamp when the method Modified() is 
  called. This time is guaranteed to be monotonically increasing.
  If MAF is build with VTK compilation enabled (MAF_USE_VTK) this object
  uses a vtkTimeStamp as counter to have a unique time stamp against the
  two libraries.   
@todo
 - fix the InterlockedIncrement - which was temporary commented out (sil)
*/

class MAF_EXPORT mafMTime : public mafBase
{
public:
#ifdef MAF_USE_VTK
  mafMTime();
  ~mafMTime();
#else
  mafMTime() {m_ModifiedTime = 0;}; 
#endif


  virtual const char *GetTypeName() {return "mafMTime";};

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
  int operator>(mafMTime& ts) {return (GetMTime() > ts.GetMTime());};
  int operator<(mafMTime& ts) {return (GetMTime() < ts.GetMTime());};

  /** Allow for typecasting to unsigned long. */
  operator unsigned long() {return GetMTime();};

private:

#ifdef MAF_USE_VTK
  vtkTimeStamp *m_VTKTimeStamp;
#else
  unsigned long m_ModifiedTime;
#endif
  
};

#endif

