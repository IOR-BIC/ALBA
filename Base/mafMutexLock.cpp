/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMutexLock.cpp,v $
  Language:  C++
  Date:      $Date: 2005-01-14 18:13:48 $
  Version:   $Revision: 1.4 $
  Authors:   Based on itkmafMutexLock (www.itk.org), adapted by Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafMutexLock.h"
#include "mmuUtility.h"

#ifdef CMAKE_USE_SPROC_INIT
#include <abi_mutex.h>
#endif

#ifdef CMAKE_USE_PTHREADS_INIT
#include <pthread.h>
#endif
 
#if defined(_WIN32) && !defined(CMAKE_USE_PTHREADS_INIT)
#include "mafIncludeWIN32.h"
#endif

#ifdef CMAKE_USE_SPROC_INIT
#include <abi_mutex.h>
typedef abilock_t FastMutexType;
#endif

#ifdef CMAKE_USE_PTHREADS_INIT
#include <pthread.h>
typedef pthread_mutex_t FastMutexType;
#endif
 
#if defined(_WIN32) && !defined(CMAKE_USE_PTHREADS_INIT)
#include <winbase.h>
typedef CRITICAL_SECTION FastMutexType;
#endif

#ifndef CMAKE_USE_SPROC_INIT
#ifndef CMAKE_USE_PTHREADS_INIT
#ifndef _WIN32
typedef int FastMutexType;
#endif
#endif
#endif

//----------------------------------------------------------------------------
class mmuPIMPLMutexLock:mmuUtility
//----------------------------------------------------------------------------
{
public:
  mmuPIMPLMutexLock();
  ~mmuPIMPLMutexLock();

  /** Lock access. */
  void Lock( void ) const;

  /** Unlock access. */
  void Unlock( void ) const;
  mutable FastMutexType   m_FastMutexLock;
};

//----------------------------------------------------------------------------
mmuPIMPLMutexLock::mmuPIMPLMutexLock()
//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
mmuPIMPLMutexLock::~mmuPIMPLMutexLock()
//----------------------------------------------------------------------------
{
#if defined(_WIN32) && !defined(CMAKE_USE_PTHREADS_INIT)
  DeleteCriticalSection(&m_FastMutexLock);
#endif

#ifdef CMAKE_USE_PTHREADS_INIT
  pthread_mutex_destroy( &m_FastMutexLock);
#endif
}

//----------------------------------------------------------------------------
void mmuPIMPLMutexLock::Lock() const
//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
void mmuPIMPLMutexLock::Unlock() const
//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
mafMutexLock::mafMutexLock()
//----------------------------------------------------------------------------
{
  m_PIMPLMutexLock = new mmuPIMPLMutexLock;
}

//----------------------------------------------------------------------------
mafMutexLock::~mafMutexLock()
//----------------------------------------------------------------------------
{
  delete m_PIMPLMutexLock;
  m_PIMPLMutexLock = NULL;
}

//----------------------------------------------------------------------------
void mafMutexLock::Lock() const
//----------------------------------------------------------------------------
{
  m_PIMPLMutexLock->Lock();
}

//----------------------------------------------------------------------------
void mafMutexLock::Unlock() const
//----------------------------------------------------------------------------
{
  m_PIMPLMutexLock->Unlock();
}
