/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medVMEFactory.h,v $
  Language:  C++
  Date:      $Date: 2011-05-26 08:29:56 $
  Version:   $Revision: 1.3.2.2 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __medVMEFactory_h
#define __medVMEFactory_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "medDefines.h"
#include "mafVMEFactory.h"
#include "mafVME.h"

/**
  class name:  medVMEFactory
  Object factory for VMEs.
  This is a simple specialization of mafNodeFactory. Notice that medVMEFactory is also
  a mafNodeFactory this last does not need to be crated: you should create one or the other but
  not both since the Instance would be overwritten and thus the Plug*<> methods would always
  plug inside one of the two. */
class MED_EXPORT medVMEFactory : public mafVMEFactory
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
