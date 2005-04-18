/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEFactory.h,v $
  Language:  C++
  Date:      $Date: 2005-04-18 19:56:27 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEFactory_h
#define __mafVMEFactory_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafNodeFactory.h"
#include "mafVME.h"

/** Object factory for VMEs.
  This is a simple specialization of mafNodeFactory. Notice that mafVMEFactory is also
  a mafNodeFactory this last does not need to be crated: you should create one or the other but
  not both since the Instance would be overwritten and thus the Plug*<> methods would always
  plug inside one of the two. */
class MAF_EXPORT mafVMEFactory : public mafNodeFactory
{
public: 
  mafTypeMacro(mafVMEFactory,mafNodeFactory);
  virtual const char* GetMAFSourceVersion() const;
  virtual const char* GetDescription() const;

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();

  mafVME *CreateVMEInstance(const char *type_name);
  
protected:
  mafVMEFactory();
  ~mafVMEFactory() { }
  
private:
  mafVMEFactory(const mafVMEFactory&);  // Not implemented.
  void operator=(const mafVMEFactory&);  // Not implemented.
};

#endif
