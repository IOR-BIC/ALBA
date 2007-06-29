/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medVMEFactory.h,v $
  Language:  C++
  Date:      $Date: 2007-06-29 11:34:31 $
  Version:   $Revision: 1.3 $
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
#include "mafVMEFactory.h"  
#include "mafVME.h"

/** Object factory for VMEs.
  This is a simple specialization of mafNodeFactory. Notice that medVMEFactory is also
  a mafNodeFactory this last does not need to be crated: you should create one or the other but
  not both since the Instance would be overwritten and thus the Plug*<> methods would always
  plug inside one of the two. */
class MAF_EXPORT medVMEFactory : public mafVMEFactory
{
public: 
  mafTypeMacro(medVMEFactory, mafVMEFactory);
  virtual const char* GetDescription() const;

  static medVMEFactory *GetInstance() {if (!m_Instance) Initialize(); return m_Instance;}

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();

  mafVME *CreateVMEInstance(const char *type_name);
  
protected:
  medVMEFactory();
  ~medVMEFactory() { }

	static medVMEFactory *m_Instance;
  
private:
  medVMEFactory(const medVMEFactory&);  // Not implemented.
  void operator=(const medVMEFactory&);  // Not implemented.
};

#endif
