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
#ifndef __albaMutexLock_h
#define __albaMutexLock_h

#include "albaBase.h" 

class mmuPIMPLMutexLock;

/** Critical section locking class that can be allocated on the stack.
  albaMutexLock allows the locking of variables which are accessed 
  through different threads.
  On Windows 9x/NT platforms albaMutexLock is less flexible, in that
  it does not work across processes, but on the other hand it costs less:
  it does not evoke the 600-cycle x86 ring transition. The 
  albaMutexLock provides a higher-performance locking mechanism on 
  Windows, but won't work across processes.
*/
class ALBA_EXPORT albaMutexLock : public albaBase
{
public:
  /** Constructor and destructor left public purposely because of stack allocation. */
  albaMutexLock();
  ~albaMutexLock();
  
  /** Lock access. */
  void Lock( void ) const;

  /** Unlock access. */
  void Unlock( void ) const;
protected:
  mmuPIMPLMutexLock *m_PIMPLMutexLock;
};

#endif


