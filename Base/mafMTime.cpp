/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMTime.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:56:33 $
  Version:   $Revision: 1.6 $
  Authors:   Marco Petrone, inspired to vtkTimeStamp (www.vtk.org)
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
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

//-------------------------------------------------------------------------
mafMTime::mafMTime()
//-------------------------------------------------------------------------
{
  vtkNEW(m_VTKTimeStamp);
}

//-------------------------------------------------------------------------
mafMTime::~mafMTime()
//-------------------------------------------------------------------------
{
  vtkDEL(m_VTKTimeStamp);
}
#endif
//-------------------------------------------------------------------------
void mafMTime::Modified()
//-------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  m_VTKTimeStamp->Modified();
#else
  #if defined(WIN32) || defined(_WIN32)

    static long mafMTimeTime = 0;
//    m_ModifiedTime = (unsigned long)InterlockedIncrement(&mafMTimeTime); //SIL. 23-3-2005: to be re-inserted

  #else

    static unsigned long mafMTimeTime = 0;
    static mafMutexLock TimeStampMutex;
    
    TimeStampMutex.Lock();
    m_ModifiedTime = ++mafMTimeTime;
    TimeStampMutex.Unlock();

  #endif
#endif

}

#ifdef MAF_USE_VTK

//-------------------------------------------------------------------------
unsigned long int mafMTime::GetMTime() const
//-------------------------------------------------------------------------
{
  return m_VTKTimeStamp->GetMTime();
}

#else

//-------------------------------------------------------------------------
unsigned long int mafMTime::GetMTime() const
//-------------------------------------------------------------------------
{
  return m_ModifiedTime;
}

#endif







