/*=========================================================================

 Program: MAF2
 Module: mafCoreFactory
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
  static mafCoreFactory *GetInstance();// {if (!m_Instance) Initialize(); return m_Instance;}

protected:
  mafCoreFactory();
  ~mafCoreFactory() { }

  static bool m_Initialized;
  // static mafCoreFactory *m_Instance;
  
private:
  mafCoreFactory(const mafCoreFactory&);  // Not implemented.
  void operator=(const mafCoreFactory&);  // Not implemented.
};

/** Plug generic object into the MAF Core factory.*/
template <class T>
class mafPlugObject
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
