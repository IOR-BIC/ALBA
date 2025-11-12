/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEFactory
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEFactory_h
#define __albaVMEFactory_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaObjectFactory.h"
#include "albaPics.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------  
class albaVME;


/** to be used internally for plugging default nodes --- calls a member function directly */
#define albaPlugVMEMacro(vme_type,typeName) \
  RegisterNewVME(vme_type::GetStaticTypeName(), typeName, vme_type::NewObject); \
  if (albaPictureFactory::GetPicsInitialized()) \
  albaPictureFactory::GetPictureFactory()->AddVmePic(vme_type::GetStaticTypeName(),vme_type::GetIcon());


/** Object factory for VMEs.
  This is a simple specialization of albaVMEFactory. Notice that albaVMEFactory is also
  a albaVMEFactory this last does not need to be crated: you should create one or the other but
  not both since the Instance would be overwritten and thus the Plug*<> methods would always
  plug inside one of the two. */
class ALBA_EXPORT albaVMEFactory : public albaObjectFactory
{
public: 
  albaTypeMacro(albaVMEFactory, albaObjectFactory);

	virtual const char* GetDescription() const;

	/* Initialize the factory creating and registering a new instance */
	static int Initialize();
	/** return the instance pointer of the factory. return NULL if not initialized yet */
	static albaVMEFactory *GetInstance();// {if (!m_Instance) Initialize(); return m_Instance;}


	/**
	This function can be used by Application code to register new Objects's to the mflCoreFactory */
	void RegisterNewVME(const char* node_name, const char* typeName, albaCreateObjectFunction createFunction);

	/** return list of names for nodes plugged into this factory */
	static std::vector<std::string> &GetNodeNames();// {return m_NodeNames;}


  albaVME *CreateVMEInstance(const char *type_name);

//   /**
//   This function can be used by Application code to register new Objects's to the mflCoreFactory */
//   void RegisterNewNode(const char* node_name, const char* description, albaCreateObjectFunction createFunction);
// 
//   /** return list of names for nodes plugged into this factory */
//   static std::vector<std::string> &GetNodeNames();// {return m_NodeNames;}
  
protected:
  albaVMEFactory();
  ~albaVMEFactory() { }

  static bool m_Initialized;
  
private:
  albaVMEFactory(const albaVMEFactory&);  // Not implemented.
  void operator=(const albaVMEFactory&);  // Not implemented.
};


/** Plug  a node in the main ALBA Node factory.*/
template <class T>
class albaPlugVME
{
public:
	albaPlugVME(const char *typeName);

};

//------------------------------------------------------------------------------
/** Plug a new Node class into the Node factory.*/
template <class T>
albaPlugVME<T>::albaPlugVME(const char *typeName)
//------------------------------------------------------------------------------
{
	albaVMEFactory *factory = albaVMEFactory::GetInstance();
	if (factory)
	{
		factory->RegisterNewVME(T::GetStaticTypeName(), typeName, T::NewObject);
		// here plug node's icon inside picture factory
		if (albaPictureFactory::GetPicsInitialized())
			albaPictureFactory::GetPictureFactory()->AddVmePic(T::GetStaticTypeName(), T::GetIcon());
	}
}

/** Plug an attribute class into the Node factory.*/
template <class T>
class albaPlugAttribute
{
public:
	albaPlugAttribute(const char *typeName) \
	{ \
		albaVMEFactory *factory = albaVMEFactory::GetInstance(); \
		if (factory) \
			factory->RegisterNewObject(T::GetStaticTypeName(), typeName, T::NewObject); \
	}
};

#endif
