/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMutexLock.cpp,v $
  Language:  C++
  Date:      $Date: 2005-01-11 17:34:59 $
  Version:   $Revision: 1.3 $
  Authors:   Based on itkmafMutexLock (www.itk.org), adapted by Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafMutexLock.h"

mafMutexLock::mafMutexLock()
{
#ifdef CMAKE_USE_SPROC_INIT
  init_lock( &m_FastMutexLock );
#endif

#if defined(_WIN32) && !defined(CMAKE_USE_PTHREADS_INIT)
  InitializeCriticalSection(&m_FastMutexLock);
#endif

#ifdef CMAKE_USE_PTHREADS_INIT
#ifdef CMAKE_HP_PTHREADS_INIT
  pthread_mutex_init(&(m_FastMutexLock), pthread_mutexattr_default);
#else
  pthread_mutex_init(&(m_FastMutexLock), NULL);
#endif
#endif

}

mafMutexLock::~mafMutexLock()
{
#if defined(_WIN32) && !defined(CMAKE_USE_PTHREADS_INIT)
  DeleteCriticalSection(&m_FastMutexLock);
#endif

#ifdef CMAKE_USE_PTHREADS_INIT
  pthread_mutex_destroy( &m_FastMutexLock);
#endif
}

// Lock the FastMutexLock
void mafMutexLock::Lock() const
{
#ifdef CMAKE_USE_SPROC
  spin_lock( &m_FastMutexLock );
#endif

#if defined(_WIN32) && !defined(CMAKE_USE_PTHREADS_INIT)
  EnterCriticalSection(&m_FastMutexLock);
#endif

#ifdef CMAKE_USE_PTHREADS_INIT
  pthread_mutex_lock( &m_FastMutexLock);
#endif
}

void mafMutexLock::Unlock() const
{
#ifdef CMAKE_USE_SPROC_INIT
  release_lock( &m_FastMutexLock );
#endif

#if defined(_WIN32) && !defined(CMAKE_USE_PTHREADS_INIT)
  //ReleaseMutex( this->MutexLock );
  LeaveCriticalSection(&m_FastMutexLock);
#endif

#ifdef CMAKE_USE_PTHREADS_INIT
  pthread_mutex_unlock( &m_FastMutexLock);
#endif
}
