/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafCoreFactory.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:23:13 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafCoreFactory_h
#define __mafCoreFactory_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafObjectFactory.h"

/** Object factory for objects in the MAF library */
class MAF_EXPORT mafCoreFactory : public mafObjectFactory
{
public: 
  mafTypeMacro(mafCoreFactory,mafObjectFactory);
  virtual const char* GetMAFSourceVersion() const;
  virtual const char* GetDescription() const;

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();
  /** return the instance pointer of the factory. return NULL if not iitialized yet */
  static mafCoreFactory *GetInstance() {if (!m_Instance) Initialize(); return m_Instance;}

protected:
  mafCoreFactory();
  ~mafCoreFactory() { }

  static mafCoreFactory *m_Instance;
  
private:
  mafCoreFactory(const mafCoreFactory&);  // Not implemented.
  void operator=(const mafCoreFactory&);  // Not implemented.
};

/** Plug generic object into the MAF Core factory.*/
template <class T>
class MAF_EXPORT mafPlugObject
{
  public:
  mafPlugObject(const char *description) \
  { \
    mafCoreFactory *factory=mafCoreFactory::GetInstance(); \
    if (factory) \
      factory->RegisterNewObject(T::GetStaticTypeName(), description, T::NewObject); \
  }
};

#endif
