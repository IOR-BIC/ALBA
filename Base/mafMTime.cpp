/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMTime.cpp,v $
  Language:  C++
  Date:      $Date: 2005-01-14 18:13:48 $
  Version:   $Revision: 1.4 $
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

#ifdef MAF_USE_VTK

//-------------------------------------------------------------------------
unsigned long int mafMTime::GetMTime()
//-------------------------------------------------------------------------
{
  return m_VTKTimeStamp->GetMTime();
}

#else

//-------------------------------------------------------------------------
unsigned long int mafMTime::GetMTime()
//-------------------------------------------------------------------------
{
  return m_ModifiedTime;
}

#endif







