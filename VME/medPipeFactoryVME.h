/*=========================================================================

 Program: MAF2Medical
 Module: medPipeFactoryVME
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __medPipeFactoryVME_h
#define __medPipeFactoryVME_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "medVMEDefines.h"
#include "mafPipeFactoryVME.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------  

/** Object factory for Pipes.
  To make a new VME available in the MAF it must be plugged inside a factory, in particular
  this factory must be of type medPipeFactoryVME to be able to retrieve the list of pipes plugged
  in the factory. Also when using mafPlugPipe<pipe_type> the pipe icon is plugged inside the
  the MAF picture factory. */
class MED_VME_EXPORT medPipeFactoryVME : public mafPipeFactoryVME
{
public: 
  mafTypeMacro(medPipeFactoryVME,mafPipeFactoryVME);
  //virtual const char* GetMAFSourceVersion() const;
  virtual const char* GetDescription() const;
  
  static medPipeFactoryVME *GetInstance();// {if (!m_Instance) Initialize(); return m_Instance;}

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();
  
protected:
  medPipeFactoryVME();
  ~medPipeFactoryVME() { }
  
  static bool m_Initialized;

private:
  medPipeFactoryVME(const medPipeFactoryVME&);  // Not implemented.
  void operator=(const medPipeFactoryVME&);  // Not implemented.
};

#endif

