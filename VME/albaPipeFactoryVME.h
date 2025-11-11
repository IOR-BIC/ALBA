/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeFactoryVME
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaPipeFactoryVME_h
#define __albaPipeFactoryVME_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaPipeFactory.h"  

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------  

/** Object factory for Pipes.
  To make a new VME available in the ALBA it must be plugged inside a factory, in particular
  this factory must be of type albaPipeFactoryVME to be able to retrieve the list of pipes plugged
  in the factory. Also when using albaPlugPipe<pipe_type> the pipe icon is plugged inside the
  the ALBA picture factory. */
class ALBA_EXPORT albaPipeFactoryVME : public albaPipeFactory
{
public: 
  albaTypeMacro(albaPipeFactoryVME,albaPipeFactory);
  virtual const char* GetDescription() const;
  
  static albaPipeFactoryVME *GetInstance(); //{if (!m_Instance) Initialize(); return m_Instance;}

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();
  
protected:
  albaPipeFactoryVME();
  ~albaPipeFactoryVME() { }

  static bool m_Initialized;
  
  //static albaPipeFactoryVME *m_Instance;

private:
  albaPipeFactoryVME(const albaPipeFactoryVME&);  // Not implemented.
  void operator=(const albaPipeFactoryVME&);  // Not implemented.
};

#endif
