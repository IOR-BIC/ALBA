/*=========================================================================

 Program: MAF2
 Module: mafMutexLock
 Authors: Based on itkmafMutexLock (www.itk.org), adapted by Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafMutexLock_h
#define __mafMutexLock_h

#include "mafBase.h" 

class mmuPIMPLMutexLock;

/** Critical section locking class that can be allocated on the stack.
  mafMutexLock allows the locking of variables which are accessed 
  through different threads.
  On Windows 9x/NT platforms mafMutexLock is less flexible, in that
  it does not work across processes, but on the other hand it costs less:
  it does not evoke the 600-cycle x86 ring transition. The 
  mafMutexLock provides a higher-performance locking mechanism on 
  Windows, but won't work across processes.
*/
class MAF_EXPORT mafMutexLock : public mafBase
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
  mmuPIMPLMutexLock *m_PIMPLMutexLock;
};

#endif


