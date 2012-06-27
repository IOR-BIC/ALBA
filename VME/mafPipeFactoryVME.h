/*=========================================================================

 Program: MAF2
 Module: mafPipeFactoryVME
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafPipeFactoryVME_h
#define __mafPipeFactoryVME_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafPipeFactory.h"  

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------  

/** Object factory for Pipes.
  To make a new VME available in the MAF it must be plugged inside a factory, in particular
  this factory must be of type mafPipeFactoryVME to be able to retrieve the list of pipes plugged
  in the factory. Also when using mafPlugPipe<pipe_type> the pipe icon is plugged inside the
  the MAF picture factory. */
class MAF_EXPORT mafPipeFactoryVME : public mafPipeFactory
{
public: 
  mafTypeMacro(mafPipeFactoryVME,mafPipeFactory);
  virtual const char* GetMAFSourceVersion() const;
  virtual const char* GetDescription() const;
  
  static mafPipeFactoryVME *GetInstance(); //{if (!m_Instance) Initialize(); return m_Instance;}

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();
  
protected:
  mafPipeFactoryVME();
  ~mafPipeFactoryVME() { }

  static bool m_Initialized;
  
  //static mafPipeFactoryVME *m_Instance;

private:
  mafPipeFactoryVME(const mafPipeFactoryVME&);  // Not implemented.
  void operator=(const mafPipeFactoryVME&);  // Not implemented.
};

#endif
