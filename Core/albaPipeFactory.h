/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeFactory
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaPipeFactory_h
#define __albaPipeFactory_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaObjectFactory.h"

/** to be used internally for plugging default pipes --- calls a member function directly */
#define albaPlugPipeMacro(pipe_type,descr) \
  RegisterNewPipe(pipe_type::GetStaticTypeName(), descr, pipe_type::NewObject);
  

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------  
class albaPipe;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,std::string);
#endif

/** Object factory for Pipes.
  To make a new VME available in the ALBA it must be plugged inside a factory, in particular
  this factory must be of type albaPipeFactory to be able to retrieve the list of pipes plugged
  in the factory. Also when using albaPlugPipe<pipe_type> the pipe icon is plugged inside the
  the ALBA picture factory. */
class ALBA_EXPORT albaPipeFactory : public albaObjectFactory
{
public: 
  albaTypeMacro(albaPipeFactory,albaObjectFactory);
  virtual const char* GetDescription() const;

  /* Initialize the factory creating and registering a new instance */
  static int Initialize();
  /** return the instance pointer of the factory. return NULL if not initialized yet */
  static albaPipeFactory *GetInstance();// {if (!m_Instance) Initialize(); return m_Instance;}

  /** create an instance of the pipe give its type name */
  static albaPipe *CreatePipeInstance(const char *type_name);
   
   /**
    This function can be used by Application code to register new Objects's to the mflCoreFactory */
  void RegisterNewPipe(const char* pipe_name, const char* description, albaCreateObjectFunction createFunction);

  /** return list of names for pipes plugged into this factory */
  static std::vector<std::string> &GetPipeNames();// {return m_PipeNames;}

protected:
  albaPipeFactory();
  ~albaPipeFactory() { }

  // static albaPipeFactory *m_Instance;
  static bool m_Initialized;
  // static std::vector<std::string> m_PipeNames; 
  
private:
  albaPipeFactory(const albaPipeFactory&);  // Not implemented.
  void operator=(const albaPipeFactory&);  // Not implemented.
};

/** Plug  a pipe in the main ALBA Pipe factory.*/
template <class T>
class albaPlugPipe
{
  public:
  albaPlugPipe(const char *description);
  
};

//------------------------------------------------------------------------------
/** Plug a new Pipe class into the Pipe factory.*/
template <class T>
albaPlugPipe<T>::albaPlugPipe(const char *description)
//------------------------------------------------------------------------------
{ 
  albaPipeFactory *factory=albaPipeFactory::GetInstance();
  if (factory)
  {
    factory->RegisterNewPipe(T::GetStaticTypeName(), description, T::NewObject);
  }
}

#endif
