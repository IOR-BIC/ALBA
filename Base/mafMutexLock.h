/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMutexLock.h,v $
  Language:  C++
  Date:      $Date: 2004-11-10 06:59:17 $
  Version:   $Revision: 1.1 $
  Authors:   Based on itkmafMutexLock (www.itk.org), adapted by Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafMutexLock_h
#define __mafMutexLock_h

#include "mafConfigure.h"

#ifdef CMAKE_USE_SPROC_INIT
#include <abi_mutex.h>
#endif

#ifdef CMAKE_USE_PTHREADS_INIT
#include <pthread.h>
#endif
 
#if defined(_WIN32) && !defined(CMAKE_USE_PTHREADS_INIT)
#include "mafWIN32.h"
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

/** Critical section locking class that can be allocated on the stack.
  mafMutexLock allows the locking of variables which are accessed 
  through different threads.
  On Windows 9x/NT platforms mafMutexLock is less flexible, in that
  it does not work across processes, but on the other hand it costs less:
  it does not evoke the 600-cycle x86 ring transition. The 
  mafMutexLock provides a higher-performance locking mechanism on 
  Windows, but won't work across processes.
*/
class MAF_EXPORT mafMutexLock
{
public:
  /** Constructor and destructor left public purposely because of stack allocation. */
  mafMutexLock();
  ~mafMutexLock();
  
  /** Lock access. */
  void Lock( void ) const;

  /** Unlock access. */
  void Unlock( void ) const;

protected:
  mutable FastMutexType   m_FastMutexLock;
};

#endif

