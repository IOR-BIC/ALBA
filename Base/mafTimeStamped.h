/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTimeStamped.h,v $
  Language:  C++
  Date:      $Date: 2005-03-10 12:01:59 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
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
  mafTimeStamped() {}

  /** increment update modification time */
  void Modified() {m_MTime.Modified();}

  /** return modification time */
  virtual unsigned long GetMTime() {return m_MTime.GetMTime();}

protected:
  mafMTime          m_MTime;        ///< Last modification time
private:
  
};

#endif /* __mafTimeStamped_h */
