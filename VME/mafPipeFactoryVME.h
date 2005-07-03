/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeFactoryVME.h,v $
  Language:  C++
  Date:      $Date: 2005-07-03 15:20:11 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
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
  
  static mafPipeFactoryVME *GetInstance() {if (!m_Instance) Initialize(); return m_Instance;}

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();
  
protected:
  mafPipeFactoryVME();
  ~mafPipeFactoryVME() { }
  
  static mafPipeFactoryVME *m_Instance;

private:
  mafPipeFactoryVME(const mafPipeFactoryVME&);  // Not implemented.
  void operator=(const mafPipeFactoryVME&);  // Not implemented.
};

#endif
