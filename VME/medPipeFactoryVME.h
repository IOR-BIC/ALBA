/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeFactoryVME.h,v $
  Language:  C++
  Date:      $Date: 2011-05-26 08:27:37 $
  Version:   $Revision: 1.2.2.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __medPipeFactoryVME_h
#define __medPipeFactoryVME_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "medDefines.h"
#include "mafPipeFactoryVME.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------  

/** Object factory for Pipes.
  To make a new VME available in the MAF it must be plugged inside a factory, in particular
  this factory must be of type medPipeFactoryVME to be able to retrieve the list of pipes plugged
  in the factory. Also when using mafPlugPipe<pipe_type> the pipe icon is plugged inside the
  the MAF picture factory. */
class MED_EXPORT medPipeFactoryVME : public mafPipeFactoryVME
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

