/*=========================================================================

 Program: MAF2
 Module: mafMTime
 Authors: Marco Petrone, inspired to vtkTimeStamp (www.vtk.org)
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
//
// Initialize static member
//
#include "mafMTime.h"
#include "mafDefines.h"
#include "mafMutexLock.h"
#include "mafIncludeWIN32.h"

#ifdef MAF_USE_VTK

#include "vtkTimeStamp.h"

#endif

//-------------------------------------------------------------------------
void mafMTime::Modified()
//-------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK  

  static vtkTimeStamp timeStamp;
  static mafMutexLock TimeStampMutex;

  TimeStampMutex.Lock();
  timeStamp.Modified();
  m_ModifiedTime = timeStamp.GetMTime();
  TimeStampMutex.Unlock();

#else
  //#if defined(WIN32) || defined(_WIN32)

//    static long mafMTimeTime = 0;
//    m_ModifiedTime = (unsigned long)InterlockedIncrement(&mafMTimeTime); //SIL. 23-3-2005: to be re-inserted
//
//  #else

    static unsigned long mafMTimeTime = 0;
    static mafMutexLock TimeStampMutex;
    
    TimeStampMutex.Lock();
    m_ModifiedTime = ++mafMTimeTime;
    TimeStampMutex.Unlock();

//  #endif
#endif

}

//-------------------------------------------------------------------------
unsigned long int mafMTime::GetMTime() const
//-------------------------------------------------------------------------
{
  return m_ModifiedTime;
}





