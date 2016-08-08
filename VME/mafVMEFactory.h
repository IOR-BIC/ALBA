/*=========================================================================

 Program: MAF2
 Module: mafVMEFactory
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEFactory_h
#define __mafVMEFactory_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafObjectFactory.h"
#include "mafPics.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------  
class mafVME;


/** to be used internally for plugging default nodes --- calls a member function directly */
#define mafPlugVMEMacro(vme_type,typeName) \
  RegisterNewVME(vme_type::GetStaticTypeName(), typeName, vme_type::NewObject); \
  if (mafPictureFactory::GetPicsInitialized()) \
  mafPictureFactory::GetPictureFactory()->AddVmePic(vme_type::GetStaticTypeName(),vme_type::GetIcon());


/** Object factory for VMEs.
  This is a simple specialization of mafVMEFactory. Notice that mafVMEFactory is also
  a mafVMEFactory this last does not need to be crated: you should create one or the other but
  not both since the Instance would be overwritten and thus the Plug*<> methods would always
  plug inside one of the two. */
class MAF_EXPORT mafVMEFactory : public mafObjectFactory
{
public: 
  mafTypeMacro(mafVMEFactory, mafObjectFactory);

	virtual const char* GetMAFSourceVersion() const;
	virtual const char* GetDescription() const;

	/* Initialize the factory creating and registering a new instance */
	static int Initialize();
	/** return the instance pointer of the factory. return NULL if not initialized yet */
	static mafVMEFactory *GetInstance();// {if (!m_Instance) Initialize(); return m_Instance;}


	/**
	This function can be used by Application code to register new Objects's to the mflCoreFactory */
	void RegisterNewVME(const char* node_name, const char* typeName, mafCreateObjectFunction createFunction);

	/** return list of names for nodes plugged into this factory */
	static std::vector<std::string> &GetNodeNames();// {return m_NodeNames;}


  mafVME *CreateVMEInstance(const char *type_name);

//   /**
//   This function can be used by Application code to register new Objects's to the mflCoreFactory */
//   void RegisterNewNode(const char* node_name, const char* description, mafCreateObjectFunction createFunction);
// 
//   /** return list of names for nodes plugged into this factory */
//   static std::vector<std::string> &GetNodeNames();// {return m_NodeNames;}
  
protected:
  mafVMEFactory();
  ~mafVMEFactory() { }

  static bool m_Initialized;
  
private:
  mafVMEFactory(const mafVMEFactory&);  // Not implemented.
  void operator=(const mafVMEFactory&);  // Not implemented.
};


/** Plug  a node in the main MAF Node factory.*/
template <class T>
class mafPlugVME
{
public:
	mafPlugVME(const char *typeName);

};

//------------------------------------------------------------------------------
/** Plug a new Node class into the Node factory.*/
template <class T>
mafPlugVME<T>::mafPlugVME(const char *typeName)
//------------------------------------------------------------------------------
{
	mafVMEFactory *factory = mafVMEFactory::GetInstance();
	if (factory)
	{
		factory->RegisterNewVME(T::GetStaticTypeName(), typeName, T::NewObject);
		// here plug node's icon inside picture factory
		if (mafPictureFactory::GetPicsInitialized())
			mafPictureFactory::GetPictureFactory()->AddVmePic(T::GetStaticTypeName(), T::GetIcon());
	}
}

/** Plug an attribute class into the Node factory.*/
template <class T>
class mafPlugAttribute
{
public:
	mafPlugAttribute(const char *typeName) \
	{ \
		mafVMEFactory *factory = mafVMEFactory::GetInstance(); \
		if (factory) \
			factory->RegisterNewObject(T::GetStaticTypeName(), typeName, T::NewObject); \
	}
};

#endif
