/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCoreFactory
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaCoreFactory_h
#define __albaCoreFactory_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaObjectFactory.h"

/** Object factory for objects in the ALBA library */
class ALBA_EXPORT albaCoreFactory : public albaObjectFactory
{
public: 
  albaTypeMacro(albaCoreFactory,albaObjectFactory);
  virtual const char* GetDescription() const;

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();
  /** return the instance pointer of the factory. return NULL if not iitialized yet */
  static albaCoreFactory *GetInstance();// {if (!m_Instance) Initialize(); return m_Instance;}

protected:
  albaCoreFactory();
  ~albaCoreFactory() { }

  static bool m_Initialized;
  // static albaCoreFactory *m_Instance;
  
private:
  albaCoreFactory(const albaCoreFactory&);  // Not implemented.
  void operator=(const albaCoreFactory&);  // Not implemented.
};

/** Plug generic object into the ALBA Core factory.*/
template <class T>
class albaPlugObject
{
  public:
  albaPlugObject(const char *description) \
  { \
    albaCoreFactory *factory=albaCoreFactory::GetInstance(); \
    if (factory) \
      factory->RegisterNewObject(T::GetStaticTypeName(), description, T::NewObject); \
  }
};

#endif
