/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMTime.h,v $
  Language:  C++
  Date:      $Date: 2005-01-11 17:34:58 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone, inspired to vtkTimeStamp (www.vtk.org)
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafMTime_h
#define __mafMTime_h

#include "mafDefines.h"
#include "mafBase.h" 

#ifdef MAF_USE_VTK
  #include "vtkTimeStamp.h"
#endif

/** mafMTime - record modification timestamp
  mafMTime records a unique time stamp when the method Modified() is 
  called. This time is guaranteed to be monotonically increasing.
  If MAF is build with VTK compilation enabled (MAF_USE_VTK) this object
  uses a mafMTime as counter, to have a unique time stamp against the
  two libraries. */
class MAF_EXPORT mafMTime : public mafBase
{
public:
#ifdef MAF_USE_VTK
  mafMTime() {} 
#else
  mafMTime() {m_ModifiedTime = 0;}; 
#endif
  virtual const char *GetTypeName() {return "mafMTime";};

  /**
    Set this objects time to the current time. The current time is
    just a monotonically increasing unsigned long integer. It is
    possible for this number to wrap around back to zero.
    This should only happen for processes that have been running
    for a very long time, while constantly changing objects
    within the program. When this does occur, the typical consequence
    should be that some filters will update themselves when really
    they don't need to. */
  void Modified();

  /** Return this object's Modified time. */
  unsigned long int GetMTime();

  /** Support comparisons of time stamp objects directly. */
  int operator>(mafMTime& ts) {return (GetMTime() > ts.GetMTime());};
  int operator<(mafMTime& ts) {return (GetMTime() < ts.GetMTime());};

  /** Allow for typecasting to unsigned long. */
  operator unsigned long() {return GetMTime();};

private:

#ifdef MAF_USE_VTK
  vtkTimeStamp m_VTKTimeStamp;
#else
  unsigned long m_ModifiedTime;
#endif
};

#ifdef MAF_USE_VTK

//-------------------------------------------------------------------------
inline unsigned long int mafMTime::GetMTime()
//-------------------------------------------------------------------------
{
  return m_VTKTimeStamp.GetMTime();
}

#else

//-------------------------------------------------------------------------
inline unsigned long int mafMTime::GetMTime()
//-------------------------------------------------------------------------
{
  return m_ModifiedTime;
}

#endif

#endif

