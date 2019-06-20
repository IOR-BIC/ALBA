/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMutexLock
 Authors: Based on itkalbaMutexLock (www.itk.org), adapted by Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaMutexLock.h"
#include "albaUtility.h"

#ifdef CMAKE_USE_SPROC_INIT
#include <abi_mutex.h>
#endif

#ifdef CMAKE_USE_PTHREADS_INIT
#include <pthread.h>
#endif
 
#if defined(_WIN32) && !defined(CMAKE_USE_PTHREADS_INIT)
#include "albaIncludeWIN32.h"
#endif

#ifdef CMAKE_USE_SPROC_INIT
#include <abi_mutex.h>
typedef abilock_t FastMutexType;
#endif

#ifdef CMAKE_USE_PTHREADS_INIT
#include <pthread.h>
typedef pthread_mutex_t FastMutexType;
typedef pthread_mutexattr_t MutexAttr;
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
class mmuPIMPLMutexLock:albaUtility
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
#ifdef CMAKE_USE_PTHREADS_INIT
  MutexAttr m_MutexAttributes;
#endif
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
    pthread_mutexattr_init(&m_MutexAttributes);
    pthread_mutexattr_settype(&m_MutexAttributes, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&(m_FastMutexLock), &m_MutexAttributes);
    //pthread_mutex_init(&(m_FastMutexLock), NULL);
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
#ifndef CMAKE_HP_PTHREADS_INIT
  pthread_mutexattr_destroy(&m_MutexAttributes);
#endif
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
albaMutexLock::albaMutexLock()
//----------------------------------------------------------------------------
{
  m_PIMPLMutexLock = new mmuPIMPLMutexLock;
}

//----------------------------------------------------------------------------
albaMutexLock::~albaMutexLock()
//----------------------------------------------------------------------------
{
  delete m_PIMPLMutexLock;
  m_PIMPLMutexLock = NULL;
}

//----------------------------------------------------------------------------
void albaMutexLock::Lock() const
//----------------------------------------------------------------------------
{
  if (m_PIMPLMutexLock)
    m_PIMPLMutexLock->Lock();
}

//----------------------------------------------------------------------------
void albaMutexLock::Unlock() const
//----------------------------------------------------------------------------
{
  if (m_PIMPLMutexLock)
    m_PIMPLMutexLock->Unlock();
}
