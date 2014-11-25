/*=========================================================================

 Program: MAF2Medical
 Module: medVMEFactory
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __medVMEFactory_h
#define __medVMEFactory_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafVMEFactory.h"
#include "mafVME.h"

/**
  class name:  medVMEFactory
  Object factory for VMEs.
  This is a simple specialization of mafNodeFactory. Notice that medVMEFactory is also
  a mafNodeFactory this last does not need to be crated: you should create one or the other but
  not both since the Instance would be overwritten and thus the Plug*<> methods would always
  plug inside one of the two. */
class MAF_EXPORT medVMEFactory : public mafVMEFactory
{
public: 
  /** RTTI Macro */
  mafTypeMacro(medVMEFactory, mafVMEFactory);
  /** Retrieve Description string*/
  virtual const char* GetDescription() const;

  /** retrieve the singleton of the factory */
  static medVMEFactory *GetInstance();// {if (!m_Instance) Initialize(); return m_Instance;}

  /** Initialize the factory creating and registering a new instance */
  static int Initialize();

  /** Create an instance of registered vme */
  mafVME *CreateVMEInstance(const char *type_name);
  
protected:
  /** constructor */
  medVMEFactory();
  /** destructor */
  ~medVMEFactory() { }

	// static medVMEFactory *m_Instance;

  static bool m_Initialized;
  
private:
  /** Copy Constructor , not implemented */
  medVMEFactory(const medVMEFactory&);
  /** operator =, not implemented */
  void operator=(const medVMEFactory&);
};

#endif
