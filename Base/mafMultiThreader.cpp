/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafMultiThreader.cpp,v $
Language:  C++
Date:      $Date: 2005-04-27 16:55:16 $
Version:   $Revision: 1.2 $
Authors:   Based on mafMultiThreader (www.vtk.org), adapted by Marco Petrone
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafMultiThreader.h"
#include "mafMutexLock.h"
#include "mafIndent.h"

// These are the includes necessary for multithreaded rendering on an SGI
// using the sproc() call
#ifdef CMAKE_USE_SPROC_INIT
#include <sys/resource.h>
#include <sys/prctl.h>
#include <wait.h>
#include <errno.h>
#endif

// Need to define "mmuExternCThreadFunctionType" to avoid warning on some
// platforms about passing function pointer to an argument expecting an
// extern "C" function.  Placing the typedef of the function pointer type
// inside an extern "C" block solves this problem.
#if defined(CMAKE_USE_PTHREADS_INIT) || defined(CMAKE_USE_PTHREADS_INIT)
#include <pthread.h>
extern "C" { typedef void *(*mmuExternCThreadFunctionType)(void *); }
#else
typedef mmuInternalThreadFunctionType mmuExternCThreadFunctionType;
#endif

#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Constructor. Default all the methods to NULL. Since the
// ThreadInfoArray is static, the ThreadIDs can be initialized here
// and will not change.
mafMultiThreader::mafMultiThreader()
{
  int i;

  for ( i = 0; i < MAF_MAX_THREADS; i++ )
  {
    m_ThreadInfoArray[i].m_ThreadID           = i;
    m_ThreadInfoArray[i].m_ActiveFlag         = NULL;
    m_ThreadInfoArray[i].m_ActiveFlagLock     = NULL;
    m_SpawnedThreadActiveFlag[i]              = 0;
    m_SpawnedThreadActiveFlagLock[i]          = NULL;
    m_SpawnedThreadInfoArray[i].m_ThreadID    = i;
  }
}

// Destructor. Nothing allocated so nothing needs to be done here.
mafMultiThreader::~mafMultiThreader()
{
}

#ifdef CMAKE_USE_WIN32_THREADS_INIT
struct mmuInternalWin32ThreadData
{
  mafMultiThreader::mafThreadFunctionType f;
  mmuThreadInfoStruct *data;
};
DWORD WINAPI mmuInternalWin32ThreadProc(LPVOID lpParameter)
{
  ((struct mmuInternalWin32ThreadData *)lpParameter)->f(((struct mmuInternalWin32ThreadData *)lpParameter)->data);
  delete ((struct mmuInternalWin32ThreadData *)lpParameter); //remove data
  
  return 0;
}
#endif

int mafMultiThreader::SpawnThread( mafThreadFunctionType f, void *UserData )
{
  int id;

  // avoid a warning
  mafThreadFunctionType tf;
  tf = f; tf= tf;
  
#ifdef CMAKE_USE_WIN32_THREADS_INIT
  DWORD              threadId;
#endif

  id = 0;

  while ( id < MAF_MAX_THREADS )
    {
    if ( m_SpawnedThreadActiveFlagLock[id] == NULL )
    {
      m_SpawnedThreadActiveFlagLock[id] = new mafMutexLock;
    }
    m_SpawnedThreadActiveFlagLock[id]->Lock();
    if (m_SpawnedThreadActiveFlag[id] == 0)
    {
      // We've got a usable thread id, so grab it
      m_SpawnedThreadActiveFlag[id] = 1;
      m_SpawnedThreadActiveFlagLock[id]->Unlock();
      break;
    }
    m_SpawnedThreadActiveFlagLock[id]->Unlock();
      
    id++;
  }

  if ( id >= MAF_MAX_THREADS )
  {
    mafErrorMessage( "You have too many active threads!" );
    return MAF_ERROR;
  }

  m_SpawnedThreadInfoArray[id].m_UserData = UserData;
  m_SpawnedThreadInfoArray[id].m_NumberOfThreads = 1;
  m_SpawnedThreadInfoArray[id].m_ActiveFlag = &m_SpawnedThreadActiveFlag[id];
  m_SpawnedThreadInfoArray[id].m_ActiveFlagLock = m_SpawnedThreadActiveFlagLock[id];

  // We are using sproc (on SGIs), pthreads(on Suns or HPs), 
  // CreateThread (on win32), or generating an error  

#ifdef CMAKE_USE_WIN32_THREADS_INIT

  mmuInternalWin32ThreadData *data=new mmuInternalWin32ThreadData;
  data->data= &(m_SpawnedThreadInfoArray[id]);
  data->f=f;
  // Using CreateThread on a PC
  //
  m_SpawnedThreadProcessID[id] = 
      CreateThread(NULL, 0, mmuInternalWin32ThreadProc, data, 0, &threadId);
  mafSleep(10);

  if (m_SpawnedThreadProcessID[id] == NULL)
  {
    mafErrorMacro("Error in thread creation !!!");
  } 
#endif

#ifdef CMAKE_USE_SPROC_INIT
  // Using sproc() on an SGI
  //
  m_SpawnedThreadProcessID[id] = sproc( f, PR_SADDR, ( (void *)(&m_SpawnedThreadInfoArray[id]) ) );
#endif

#ifdef CMAKE_USE_PTHREADS_INIT
  // Using POSIX threads
  //
  pthread_attr_t attr;

#ifdef CMAKE_USE_PTHREADS_INIT
  pthread_attr_create( &attr );
#else  
  pthread_attr_init(&attr);
#ifndef __CYGWIN__
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
#endif
#endif
  
#ifdef CMAKE_USE_PTHREADS_INIT
  pthread_create( &(m_SpawnedThreadProcessID[id]),
                  attr, f,  
                  ( (void *)(&m_SpawnedThreadInfoArray[id]) ) );
#else
  pthread_create( &(m_SpawnedThreadProcessID[id]),
                  &attr,
                  reinterpret_cast<mmuExternCThreadFunctionType>(f),  
                  ( (void *)(&m_SpawnedThreadInfoArray[id]) ) );
#endif

#endif

#ifndef CMAKE_USE_WIN32_THREADS_INIT
#ifndef CMAKE_USE_SPROC_INIT
#ifndef CMAKE_USE_PTHREADS_INIT
  // There is no multi threading, so there is only one thread.
  // This won't work - so give an error message.
  mafErrorMacro( << "Cannot spawn thread in a single threaded environment!" );
  m_SpawnedThreadActiveFlagLock[id]->Delete();
  id = -1;
#endif
#endif
#endif

  return id;
}

void mafMultiThreader::TerminateThread( int ThreadID )
{

  if ( !m_SpawnedThreadActiveFlag[ThreadID] ) {
    return;
  }

  m_SpawnedThreadActiveFlagLock[ThreadID]->Lock();
  m_SpawnedThreadActiveFlag[ThreadID] = 0;
  m_SpawnedThreadActiveFlagLock[ThreadID]->Unlock();

#ifdef CMAKE_USE_WIN32_THREADS_INIT
  WaitForSingleObject(m_SpawnedThreadProcessID[ThreadID], INFINITE);
  CloseHandle(m_SpawnedThreadProcessID[ThreadID]);
#endif

#ifdef CMAKE_USE_SPROC_INIT
  siginfo_t info_ptr;

  waitid( P_PID, (id_t) m_SpawnedThreadProcessID[ThreadID], 
          &info_ptr, WEXITED );
#endif

#ifdef CMAKE_USE_PTHREADS_INIT
  pthread_join( m_SpawnedThreadProcessID[ThreadID], NULL );
#endif

#ifndef CMAKE_USE_WIN32_THREADS_INIT
#ifndef CMAKE_USE_SPROC_INIT
#ifndef CMAKE_USE_PTHREADS_INIT
  // There is no multi threading, so there is only one thread.
  // This won't work - so give an error message.
  mafErrorMacro(<< "Cannot terminate thread in single threaded environment!");
#endif
#endif
#endif

  delete m_SpawnedThreadActiveFlagLock[ThreadID];
  m_SpawnedThreadActiveFlagLock[ThreadID] = NULL;

}

// Print method for the multithreader
void mafMultiThreader::Print(std::ostream& os, const int tabs)
{
  mafIndent indent(tabs);
  os << "Thread system used: " << 
#ifdef CMAKE_USE_PTHREADS_INIT  
   "PTHREADS"
#elif defined CMAKE_USE_SPROC_INIT
    "SPROC"
#elif defined CMAKE_USE_WIN32_THREADS_INIT
    "WIN32 Threads"
#elif defined CMAKE_HP_PTHREADS_INIT
    "HP PThreads"
#else
    "NO THREADS SUPPORT"
#endif
    << std::endl;
}
