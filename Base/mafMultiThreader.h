/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafMultiThreader.h,v $
Language:  C++
Date:      $Date: 2005-04-29 06:02:46 $
Version:   $Revision: 1.3 $
Authors:   Based on vtkMultiThreader (www.vtk.org), adapted by Marco Petrone
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafMultiThreader_h
#define __mafMultiThreader_h

#include "mafDefines.h"
#include "mafBase.h"
#include "mafIncludeWIN32.h"

#define MAF_MAX_THREADS 16

#ifdef CMAKE_USE_SPROC_INIT
#include <sys/types.h>   Needed for unix implementation of sproc
#include <unistd.h>   Needed for unix implementation of sproc
#endif

#if defined(CMAKE_USE_PTHREADS_INIT) || defined(CMAKE_HP_PTHREADS_INIT)
#include <pthread.h>   //Needed for PTHREAD implementation of mutex
#include <sys/types.h>   //Needed for unix implementation of pthreads
#include <unistd.h>   //Needed for unix implementation of pthreads
#endif
/**
  If CMAKE_USE_SPROC_INIT is defined, then sproc() will be used to create
  multiple threads on an SGI. If CMAKE_USE_PTHREADS_INIT is defined, then
  pthread_create() will be used to create multiple threads (on
  a sun, for example)

  If CMAKE_USE_PTHREADS_INIT is defined, then the multithreaded
  function is of type void *, and returns NULL.
  Otherwise the type is void which is correct for WIN32
  and SPROC
*/  
#ifdef CMAKE_USE_SPROC_INIT
typedef int mmuThreadProcessIDType;
#endif


#ifdef CMAKE_USE_PTHREADS_INIT
typedef void *(*mmuInternalThreadFunctionType)(void *);
typedef pthread_t mmuThreadProcessIDType;
#endif

#ifdef CMAKE_USE_WIN32_THREADS_INIT
typedef LPTHREAD_START_ROUTINE mmuInternalThreadFunctionType;
typedef HANDLE mmuThreadProcessIDType;
#endif

#if !defined(CMAKE_USE_PTHREADS_INIT) && !defined(CMAKE_USE_WIN32_THREADS_INIT)
typedef void (*mmuInternalThreadFunctionType)(void *);
typedef int mmuThreadProcessIDType;
#endif


class mafMutexLock;

/** mafMultiThreader - A class for performing multithreaded execution.
  mafMultiThreader is a class that provides support for multithreaded
  execution using sproc() on an SGI, or pthread_create on any platform
  supporting POSIX threads.  This class can be used to execute a single
  method on multiple threads, or to specify a method per thread. */
class MAF_EXPORT mafMultiThreader: public mafBase
{
public:
  mafMultiThreader();
  ~mafMultiThreader();

  void Print(std::ostream& os, const int tabs);

  /**
    This is the structure that is passed to the thread that is
    created from the SingleMethodExecute, MultipleMethodExecute or
    the SpawnThread method. It is passed in as a void *, and it is
    up to the method to cast correctly and extract the information.
    The ThreadID is a number between 0 and NumberOfThreads-1 that indicates
    the id of this thread. The NumberOfThreads is this->NumberOfThreads for
    threads created from SingleMethodExecute or MultipleMethodExecute,
    and it is 1 for threads created from SpawnThread.
    The UserData is the (void *)arg passed into the SetSingleMethod,
    SetMultipleMethod, or SpawnThread method.*/
#define mmuThreadInfoStruct mafMultiThreader::mmuThreadInfo
  class mmuThreadInfo
  {
  public:
    int                 m_ThreadID;
    int                 m_NumberOfThreads;
    int*                m_ActiveFlag;
    mafMutexLock*       m_ActiveFlagLock;
    void*               m_UserData;
  };

  typedef void (*mafThreadFunctionType)(mmuThreadInfoStruct *);

  /**
    Create a new thread for the given function. Return a thread id
    which is a number between 0 and MAF_MAX_THREADS - 1. This id should
    be used to kill the thread at a later time. */
  int SpawnThread( mafThreadFunctionType, void *UserData );

  /** Terminate the thread that was created with a SpawnThreadExecute() */
  void TerminateThread( int thread_id );

protected:

  /**
    An array of thread info containing a thread id
    (0, 1, 2, .. MAF_MAX_THREADS-1), the thread count, and a pointer
    to void so that user data can be passed to each thread */
  mmuThreadInfo               m_ThreadInfoArray[MAF_MAX_THREADS];

  /**
    Storage of MutexFunctions and ints used to control spawned 
    threads and the spawned thread ids */
  int                        m_SpawnedThreadActiveFlag[MAF_MAX_THREADS];
  mafMutexLock*              m_SpawnedThreadActiveFlagLock[MAF_MAX_THREADS];
  mmuThreadProcessIDType     m_SpawnedThreadProcessID[MAF_MAX_THREADS];
  mmuThreadInfo              m_SpawnedThreadInfoArray[MAF_MAX_THREADS];
};

#endif





