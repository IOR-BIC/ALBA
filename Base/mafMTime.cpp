/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMTime.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-19 18:12:51 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone, inspired to vtkTimeStamp (www.vtk.org)
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
//
// Initialize static member
//
#include "mafMTime.h"
#include "mafMutexLock.h"

#ifdef MAF_USE_VTK
  #include "vtkTimeStamp.h"
#endif

//-------------------------------------------------------------------------
void mafMTime::Modified()
//-------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK

  m_VTKTimeStamp.Modified();

#else

#if defined(WIN32) || defined(_WIN32)

  static LONG mafMTimeTime = 0;
  m_ModifiedTime = (unsigned long)InterlockedIncrement(&mafMTimeTime);

#else

  static unsigned long mafMTimeTime = 0;
  static mafMutexLock TimeStampMutex;
  
  TimeStampMutex.Lock();
  m_ModifiedTime = ++mafMTimeTime;
  TimeStampMutex.Unlock();

#endif

#endif

}









