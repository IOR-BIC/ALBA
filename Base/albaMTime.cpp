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
//
// Initialize static member
//
#include "albaMTime.h"
#include "albaDefines.h"
#include "albaMutexLock.h"
#include "albaIncludeWIN32.h"

#ifdef ALBA_USE_VTK

#include "vtkTimeStamp.h"

#endif

//-------------------------------------------------------------------------
void albaMTime::Modified()
//-------------------------------------------------------------------------
{
#ifdef ALBA_USE_VTK  

  static vtkTimeStamp timeStamp;
  static albaMutexLock TimeStampMutex;

  TimeStampMutex.Lock();
  timeStamp.Modified();
  m_ModifiedTime = timeStamp.GetMTime();
  TimeStampMutex.Unlock();

#else
  //#if defined(WIN32) || defined(_WIN32)

//    static long albaMTimeTime = 0;
//    m_ModifiedTime = (unsigned long)InterlockedIncrement(&albaMTimeTime); //SIL. 23-3-2005: to be re-inserted
//
//  #else

    static unsigned long albaMTimeTime = 0;
    static albaMutexLock TimeStampMutex;
    
    TimeStampMutex.Lock();
    m_ModifiedTime = ++albaMTimeTime;
    TimeStampMutex.Unlock();

//  #endif
#endif

}

//-------------------------------------------------------------------------
vtkMTimeType albaMTime::GetMTime() const
//-------------------------------------------------------------------------
{
  return m_ModifiedTime;
}





